#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: gmres2.c,v 1.5 1997/08/22 15:11:26 bsmith Exp bsmith $";
#endif
#include <math.h>
#include "src/ksp/impls/gmres/gmresp.h"       /*I  "ksp.h"  I*/

#undef __FUNC__  
#define __FUNC__ "KSPGMRESSetRestart" 
/*@
    KSPGMRESSetRestart - Sets the number of search directions 
    for GMRES before restart.

    Input Parameters:
.   ksp - the iterative context
.   max_k - the number of directions

    Options Database Key:
$   -ksp_gmres_restart <max_k>

    Note:
    The default value of max_k = 30.

.keywords: GMRES, set, restart

.seealso: KSPGMRESSetOrthogonalization(), KSPGMRESSetPreallocateVectors()
@*/
int KSPGMRESSetRestart(KSP ksp,int max_k )
{
  KSP_GMRES *gmres;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(ksp,KSP_COOKIE);
  gmres = (KSP_GMRES *)ksp->data;
  if (ksp->type != KSPGMRES) PetscFunctionReturn(0);
  gmres->max_k = max_k;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "KSPGMRESSetOrthogonalization" 
/*@C
  KSPGMRESSetOrthogonalization - Sets the orthogonalization routine used by GMRES.

  Input Parameters:
.   ksp   - iterative context obtained from KSPCreate
.   fcn   - Orthogonalization function

  Notes:
  Several orthogonalization routines are predefined.
$    KSPGMRESModifiedGramSchmidtOrthogonalization() - default.

$    KSPGMRESUnmodifiedGramSchmidtOrthogonalization() - 
       NOT recommended; however, for some problems, particularly
       when using parallel distributed vectors, this may be
       significantly faster.

$    KSPGMRESIROrthogonalization() - interative refinement
       version of KSPGMRESUnmodifiedGramSchmidtOrthogonalization(),
       which may be more numerically stable.

  Options Database Keys:
$  -ksp_gmres_unmodifiedgramschmidt
$  -ksp_gmres_irorthog

.keywords: GMRES, set, orthogonalization, Gram-Schmidt, iterative refinement

.seealso: KSPGMRESSetRestart(), KSPGMRESSetPreallocateVectors()
@*/
int KSPGMRESSetOrthogonalization( KSP ksp,int (*fcn)(KSP,int) )
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(ksp,KSP_COOKIE);
  if (ksp->type == KSPGMRES) {
    ((KSP_GMRES *)ksp->data)->orthog = fcn;
  }
  PetscFunctionReturn(0);
}






