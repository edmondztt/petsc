
#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: aobasic.c,v 1.24 1997/09/19 22:15:42 bsmith Exp bsmith $";
#endif

/*
    The most basic AO application ordering routines. These store the 
  entire orderings on each processor.
*/

#include "src/ao/aoimpl.h"
#include "pinclude/pviewer.h"
#include "sys.h"

typedef struct {
  int N;
  int *app,*petsc;  /* app[i] is the partner for the ith PETSc slot */
                    /* petsc[j] is the partner for the jth app slot */
} AO_Basic;

#undef __FUNC__  
#define __FUNC__ "AODestroy_Basic" /* ADIC Ignore */
int AODestroy_Basic(PetscObject obj)
{
  AO       ao = (AO) obj;
  AO_Basic *aodebug = (AO_Basic *) ao->data;

  PetscFunctionBegin;
  PetscFree(aodebug->app);
  PetscFree(ao->data); 
  PLogObjectDestroy(ao);
  PetscHeaderDestroy(ao);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "AOView_Basic" /* ADIC Ignore */
int AOView_Basic(PetscObject obj,Viewer viewer)
{
  AO          ao = (AO) obj;
  int         rank,ierr,i;
  ViewerType  vtype;
  FILE        *fd;
  AO_Basic    *aodebug = (AO_Basic*) ao->data;

  PetscFunctionBegin;
  MPI_Comm_rank(ao->comm,&rank); if (rank) PetscFunctionReturn(0);

  if (!viewer) {
    viewer = VIEWER_STDOUT_SELF; 
  }

  ierr = ViewerGetType(viewer,&vtype); CHKERRQ(ierr);
  if (vtype  == ASCII_FILE_VIEWER || vtype == ASCII_FILES_VIEWER) { 
    ierr = ViewerASCIIGetPointer(viewer,&fd); CHKERRQ(ierr);
    fprintf(fd,"Number of elements in ordering %d\n",aodebug->N);
    fprintf(fd,"   App.   PETSc\n");
    for ( i=0; i<aodebug->N; i++ ) {
      fprintf(fd,"%d   %d    %d\n",i,aodebug->app[i],aodebug->petsc[i]);
    }
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "AOPetscToApplication_Basic"  /* ADIC Ignore */
int AOPetscToApplication_Basic(AO ao,int n,int *ia)
{
  int      i;
  AO_Basic *aodebug = (AO_Basic *) ao->data;

  PetscFunctionBegin;
  for ( i=0; i<n; i++ ) {
    if (ia[i] >= 0) {ia[i] = aodebug->app[ia[i]];}
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "AOApplicationToPetsc_Basic" /* ADIC Ignore */
int AOApplicationToPetsc_Basic(AO ao,int n,int *ia)
{
  int      i;
  AO_Basic *aodebug = (AO_Basic *) ao->data;

  PetscFunctionBegin;
  for ( i=0; i<n; i++ ) {
    if (ia[i] >= 0) {ia[i] = aodebug->petsc[ia[i]];}
  }
  PetscFunctionReturn(0);
}

static struct _AOOps myops = {AOPetscToApplication_Basic,
                              AOApplicationToPetsc_Basic};

#undef __FUNC__  
#define __FUNC__ "AOCreateBasic" /* ADIC Ignore */
/*@C
   AOCreateBasic - Creates a basic application ordering using two integer arrays.

   Input Parameters:
.  comm - MPI communicator that is to share AO
.  napp - size of integer arrays
.  myapp - integer array that defines an ordering
.  mypetsc - integer array that defines another ordering

   Output Parameter:
.  aoout - the new application ordering

   Options Database Key:
$   -ao_view : call AOView() at the conclusion of AOCreateBasic()

.keywords: AO, create

.seealso: AOCreateBasicIS(), AODestroy()
@*/
int AOCreateBasic(MPI_Comm comm,int napp,int *myapp,int *mypetsc,AO *aoout)
{
  AO_Basic  *aodebug;
  AO        ao;
  int       *lens,size,rank,N,i,flg1,ierr;
  int       *allpetsc,*allapp,*disp,ip,ia;

  PetscFunctionBegin;
  *aoout = 0;
  PetscHeaderCreate(ao, _p_AO,AO_COOKIE,AO_BASIC,comm,AODestroy,AOView); 
  PLogObjectCreate(ao);
  aodebug            = PetscNew(AO_Basic);
  PLogObjectMemory(ao,sizeof(struct _p_AO) + sizeof(AO_Basic));

  PetscMemcpy(&ao->ops,&myops,sizeof(myops));
  ao->destroy = AODestroy_Basic;
  ao->view    = AOView_Basic;
  ao->data    = (void *)aodebug;

  /* transmit all lengths to all processors */
  MPI_Comm_size(comm,&size);
  MPI_Comm_rank(comm,&rank);
  lens = (int *) PetscMalloc( 2*size*sizeof(int) ); CHKPTRQ(lens);
  disp = lens + size;
  MPI_Allgather(&napp,1,MPI_INT,lens,1,MPI_INT,comm);
  N =  0;
  for ( i=0; i<size; i++ ) {
    disp[i] = N;
    N += lens[i];
  }
  aodebug->N = N;

  /* get all indices on all processors */
  allpetsc = (int *) PetscMalloc( 2*N*sizeof(int) ); CHKPTRQ(allpetsc);
  allapp   = allpetsc + N;
  MPI_Allgatherv(mypetsc,napp,MPI_INT,allpetsc,lens,disp,MPI_INT,comm);
  MPI_Allgatherv(myapp,napp,MPI_INT,allapp,lens,disp,MPI_INT,comm);
  PetscFree(lens);

  /* generate a list of application and PETSc node numbers */
  aodebug->app = (int *) PetscMalloc(2*N*sizeof(int));CHKPTRQ(aodebug->app);
  PLogObjectMemory(ao,2*N*sizeof(int));
  aodebug->petsc = aodebug->app + N;
  PetscMemzero(aodebug->app,2*N*sizeof(int));
  for ( i=0; i<N; i++ ) {
    ip = allpetsc[i]; ia = allapp[i];
    /* check there are no duplicates */
    if (aodebug->app[ip]) SETERRQ(1,0,"Duplicate in ordering");
    aodebug->app[ip] = ia + 1;
    if (aodebug->petsc[ia]) SETERRQ(1,0,"Duplicate in ordering");
    aodebug->petsc[ia] = ip + 1;
  }
  PetscFree(allpetsc);
  /* shift indices down by one */
  for ( i=0; i<N; i++ ) {
    aodebug->app[i]--;
    aodebug->petsc[i]--;
  }

  ierr = OptionsHasName(PETSC_NULL,"-ao_view",&flg1); CHKERRQ(ierr);
  if (flg1) {ierr = AOView(ao,VIEWER_STDOUT_SELF); CHKERRQ(ierr);}

  *aoout = ao; PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "AOCreateBasicIS" /* ADIC Ignore */
/*@C
   AOCreateBasicIS - Creates a basic application ordering using two index sets.

   Input Parameters:
.  comm - MPI communicator that is to share AO
.  isapp - index set that defines an ordering
.  ispetsc - index set that defines another ordering

   Output Parameter:
.  aoout - the new application ordering

   Options Database Key:
$   -ao_view : call AOView() at the conclusion of AOCreateBasicIS()

.keywords: AO, create

.seealso: AOCreateBasic(),  AODestroy()
@*/
int AOCreateBasicIS(MPI_Comm comm,IS isapp,IS ispetsc,AO *aoout)
{
  int       *mypetsc,*myapp,ierr,napp,npetsc;

  PetscFunctionBegin;
  ierr = ISGetSize(isapp,&napp); CHKERRQ(ierr);
  ierr = ISGetSize(ispetsc,&npetsc); CHKERRQ(ierr);
  if (napp != npetsc) SETERRQ(1,0,"Local IS lengths must match");

  ierr = ISGetIndices(isapp,&myapp); CHKERRQ(ierr);
  ierr = ISGetIndices(ispetsc,&mypetsc); CHKERRQ(ierr);

  ierr = AOCreateBasic(comm,napp,myapp,mypetsc,aoout); CHKERRQ(ierr);

  ierr = ISRestoreIndices(isapp,&myapp); CHKERRQ(ierr);
  ierr = ISRestoreIndices(ispetsc,&mypetsc); CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

