#include <petsc/private/dmpleximpl.h>   /*I      "petscdmplex.h"   I*/
#include <petsc/private/isimpl.h>
#include <petsc/private/vecimpl.h>
#include <petscviewerhdf5.h>

#if defined(PETSC_HAVE_HDF5)
PetscErrorCode DMPlexLoad_HDF5_Xdmf_Internal(DM dm, PetscViewer viewer)
{
  Vec             coordinates;
  IS              cells;
  PetscInt        dim, spatialDim, N, numCells, numVertices, numCorners, bs;
  //hid_t           fileId;
  PetscMPIInt     rank;
  MPI_Comm        comm;
  PetscErrorCode  ierr;

  PetscFunctionBegin;
  ierr = PetscObjectGetComm((PetscObject)dm, &comm);CHKERRQ(ierr);
  ierr = MPI_Comm_rank(comm, &rank);CHKERRQ(ierr);

  /* Read topology */
  ierr = PetscViewerHDF5ReadAttribute(viewer, "/viz/topology/cells", "cell_dim", PETSC_INT, (void *) &dim);CHKERRQ(ierr);
  ierr = PetscViewerHDF5ReadAttribute(viewer, "/viz/topology/cells", "cell_corners", PETSC_INT, (void *) &numCorners);CHKERRQ(ierr);
  ierr = PetscViewerHDF5PushGroup(viewer, "/viz/topology");CHKERRQ(ierr);
  ierr = ISCreate(comm, &cells);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject) cells, "cells");CHKERRQ(ierr);
  ierr = ISLoad(cells, viewer);CHKERRQ(ierr);
  ierr = ISGetLocalSize(cells, &numCells);CHKERRQ(ierr);
  ierr = ISGetBlockSize(cells, &bs);CHKERRQ(ierr);
  if (bs != numCorners) SETERRQ(comm, PETSC_ERR_FILE_UNEXPECTED, "cell_corners and 2-dimension of cells not equal");
  ierr = PetscViewerHDF5PopGroup(viewer);CHKERRQ(ierr);
  numCells /= numCorners;

  /* Read geometry */
  ierr = PetscViewerHDF5PushGroup(viewer, "/geometry");CHKERRQ(ierr);
  ierr = VecCreate(comm, &coordinates);CHKERRQ(ierr);
  ierr = PetscObjectSetName((PetscObject) coordinates, "vertices");CHKERRQ(ierr);
  ierr = PetscViewerHDF5ReadSizes(viewer, "vertices", &spatialDim, &N);CHKERRQ(ierr);
  ierr = VecSetSizes(coordinates, PETSC_DECIDE, N);CHKERRQ(ierr);
  ierr = VecSetBlockSize(coordinates, spatialDim);CHKERRQ(ierr);
  ierr = VecLoad(coordinates, viewer);CHKERRQ(ierr);
  ierr = VecGetLocalSize(coordinates, &numVertices);CHKERRQ(ierr);
  ierr = VecGetBlockSize(coordinates, &spatialDim);CHKERRQ(ierr);
  ierr = PetscViewerHDF5PopGroup(viewer);CHKERRQ(ierr);
  numVertices /= spatialDim;

  /* Check that maximum index referred in cells is in line with global number of vertices */
  {
    PetscInt max1, max2;
    ierr = ISGetMinMax(cells, NULL, &max1);CHKERRQ(ierr);
    ierr = VecGetSize(coordinates, &max2);CHKERRQ(ierr);
    max2 /= spatialDim; max2--;
    if (max1 > max2) SETERRQ2(PETSC_COMM_SELF, PETSC_ERR_ARG_OUTOFRANGE, "maximum index in cells = %d > %d = total number of vertices - 1", max1, max2);
  }

  {
    const PetscReal *coordinates_arr;
    const PetscInt *cells_arr;
    PetscSF sfVert=NULL;

    ierr = VecGetArrayRead(coordinates, &coordinates_arr);CHKERRQ(ierr);
    ierr = ISGetIndices(cells, &cells_arr);CHKERRQ(ierr);
    ierr = DMSetDimension(dm, dim);CHKERRQ(ierr);
    ierr = DMPlexBuildFromCellList_Parallel_Internal(dm, spatialDim, numCells, numVertices, numCorners, cells_arr, PETSC_TRUE, &sfVert);CHKERRQ(ierr);
    ierr = DMPlexBuildCoordinates_Parallel_Internal( dm, spatialDim, numCells, numVertices, sfVert, coordinates_arr);CHKERRQ(ierr);
    ierr = VecRestoreArrayRead(coordinates, &coordinates_arr);CHKERRQ(ierr);
    ierr = ISRestoreIndices(cells, &cells_arr);CHKERRQ(ierr);
    ierr = PetscSFDestroy(&sfVert);CHKERRQ(ierr);
  }
  ierr = ISDestroy(&cells);CHKERRQ(ierr);
  ierr = VecDestroy(&coordinates);CHKERRQ(ierr);

  /* scale coordinates - unlike in DMPlexLoad_HDF5_Internal, this can only be done after DM is populated */
  {
    PetscReal lengthScale;

    ierr = DMPlexGetScale(dm, PETSC_UNIT_LENGTH, &lengthScale);CHKERRQ(ierr);
    ierr = DMGetCoordinates(dm, &coordinates);CHKERRQ(ierr);
    ierr = VecScale(coordinates, 1.0/lengthScale);CHKERRQ(ierr);
  }

  /* Read Labels */
  ierr = DMPlexLoadLabels_HDF5_Internal(dm, viewer);CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
#endif
