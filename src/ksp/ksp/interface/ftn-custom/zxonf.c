#include "zpetsc.h"
#include "petscksp.h"

#if defined(PETSC_HAVE_FORTRAN_CAPS)
#define ksplgmonitorcreate_        KSPLGMONITORCREATE
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define ksplgmonitorcreate_        ksplgmonitorcreate
#endif

EXTERN_C_BEGIN

/*
   Possible bleeds memory but cannot be helped.
*/
void PETSC_STDCALL ksplgmonitorcreate_(CHAR host PETSC_MIXED_LEN(len1),
                    CHAR label PETSC_MIXED_LEN(len2),int *x,int *y,int *m,int *n,PetscDrawLG *ctx,
                    PetscErrorCode *ierr PETSC_END_LEN(len1) PETSC_END_LEN(len2))
{
  char   *t1,*t2;

  FIXCHAR(host,len1,t1);
  FIXCHAR(label,len2,t2);
  *ierr = KSPLGMonitorCreate(t1,t2,*x,*y,*m,*n,ctx);
}



EXTERN_C_END
