#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: ex6.c,v 1.11 1997/09/26 15:14:40 balay Exp bsmith $";
#endif

static char help[] = "Writes an array to a file, then reads an array from\n\
a file, then forms a vector.\n\n";

#include "vec.h"
#include "pinclude/pviewer.h"

int main(int argc,char **args)
{
  int     i, ierr, m = 10, flg, fd, size;
  Scalar  *avec, *array;
  Vec     vec;
  Viewer  view_out, view_in;

  PetscInitialize(&argc,&args,(char *)0,help);
  OptionsGetInt(PETSC_NULL,"-m",&m,&flg);

  /* ---------------------------------------------------------------------- */
  /*          PART 1: Write some data to a file in binary format            */
  /* ---------------------------------------------------------------------- */

  /* Allocate array and set values */
  array = (Scalar *) PetscMalloc( m*sizeof(Scalar) ); CHKPTRA(array);
  for (i=0; i<m; i++) {
    array[i] = i*10.0;
  }

  /* Open viewer for binary output */
  ierr = ViewerFileOpenBinary(PETSC_COMM_WORLD,"input.dat",BINARY_CREATE,&view_out);
         CHKERRA(ierr);
  ierr = ViewerBinaryGetDescriptor(view_out,&fd); CHKERRA(ierr);

  /* Write binary output */
  ierr = PetscBinaryWrite(fd,&m,1,PETSC_INT,0); CHKERRA(ierr);
  ierr = PetscBinaryWrite(fd,array,m,PETSC_SCALAR,0); CHKERRA(ierr);

  /* Destroy the output viewer and work array */
  ierr = ViewerDestroy(view_out); CHKERRA(ierr);
  PetscFree(array);

  /* ---------------------------------------------------------------------- */
  /*          PART 2: Read data from file and form a vector                 */
  /* ---------------------------------------------------------------------- */

  /* Open input binary viewer */
  ierr = ViewerFileOpenBinary(PETSC_COMM_SELF,"input.dat",BINARY_RDONLY,&view_in); 
         CHKERRA(ierr);
  ierr = ViewerBinaryGetDescriptor(view_in,&fd); CHKERRA(ierr);

  /* Create vector and get pointer to data space */
  ierr = VecCreate(PETSC_COMM_SELF,m,&vec); CHKERRA(ierr);
  ierr = VecGetArray(vec,&avec); CHKERRA(ierr);

  /* Read data into vector */
  ierr = PetscBinaryRead(fd,&size,1,PETSC_INT); CHKERRQ(ierr);
  if (size <=0) SETERRA(1,0,"Error: Must have array length > 0");

  printf("reading data in binary from input.dat, size =%d ...\n",size); 
  ierr = PetscBinaryRead(fd,avec,size,PETSC_SCALAR); CHKERRA(ierr);

  /* View vector */
  ierr = VecRestoreArray(vec,&avec); CHKERRA(ierr);
  ierr = VecView(vec,VIEWER_STDOUT_SELF); CHKERRA(ierr);

  /* Free data structures */
  ierr = VecDestroy(vec); CHKERRA(ierr);
  ierr = ViewerDestroy(view_in); CHKERRA(ierr);
  PetscFinalize();
  return 0;
}

