#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: cr.c,v 1.44 1999/01/31 16:08:42 bsmith Exp bsmith $";
#endif

/*                       
           This implements Preconditioned Conjugate Residuals.       
*/
#include "src/sles/ksp/kspimpl.h"

#undef __FUNC__  
#define __FUNC__ "KSPSetUp_CR"
static int KSPSetUp_CR(KSP ksp)
{
  int ierr;

  PetscFunctionBegin;
  if (ksp->pc_side == PC_RIGHT) {SETERRQ(2,0,"no right preconditioning for KSPCR");}
  else if (ksp->pc_side == PC_SYMMETRIC) {SETERRQ(2,0,"no symmetric preconditioning for KSPCR");}
  ierr = KSPDefaultGetWork( ksp, 9  ); CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "KSPSolve_CR"
static int  KSPSolve_CR(KSP ksp,int *its)
{
  int          i = 0, maxit,pres, hist_len, cerr = 0, ierr;
  MatStructure pflag;
  double       *history, dp;
  Scalar       lambda, alpha0, alpha1; 
  Scalar       btop, bbot, bbotold, tmp, zero = 0.0, mone = -1.0;
  Vec          X,B,R,Pm1,P,Pp1,Sm1,S,Qm1,Q,Qp1,T, Tmp;
  Mat          Amat, Pmat;

  PetscFunctionBegin;
  ksp->its = 0;

  pres    = ksp->use_pres;
  maxit   = ksp->max_it;
  history = ksp->residual_history;
  hist_len= ksp->res_hist_size;
  X       = ksp->vec_sol;
  B       = ksp->vec_rhs;
  R       = ksp->work[0];
  Pm1     = ksp->work[1];
  P       = ksp->work[2];
  Pp1     = ksp->work[3];
  Qm1     = ksp->work[4];
  Q       = ksp->work[5];
  Qp1 = T = ksp->work[6];
  Sm1     = ksp->work[7];
  S       = ksp->work[8];

  ierr = PCGetOperators(ksp->B,&Amat,&Pmat,&pflag); CHKERRQ(ierr);
  bbotold = 1.0; /* a hack */
  if (!ksp->guess_zero) {
    ierr = MatMult(Amat,X,R); CHKERRQ(ierr);    /*   r <- b - Ax       */
    ierr = VecAYPX(&mone,B,R); CHKERRQ(ierr);
  } else { 
    ierr = VecCopy(B,R); CHKERRQ(ierr);         /*    r <- b (x is 0)  */
  }
  ierr = VecSet(&zero,Pm1); CHKERRQ(ierr);      /*    pm1 <- 0         */
  ierr = VecSet(&zero,Sm1); CHKERRQ(ierr);      /*    sm1 <- 0         */
  ierr = VecSet(&zero,Qm1); CHKERRQ(ierr);      /*    Qm1 <- 0         */
  ierr = PCApply(ksp->B,R,P); CHKERRQ(ierr);    /*     p <- Br         */
  if (!ksp->avoidnorms) {
    if (pres) {
      ierr = VecNorm(P,NORM_2,&dp); CHKERRQ(ierr);/*    dp <- z'*z       */
    } else {
      ierr = VecNorm(R,NORM_2,&dp); CHKERRQ(ierr);/*    dp <- r'*r       */
    }
  }
  if ((*ksp->converged)(ksp,0,dp,ksp->cnvP)) {*its = 0; PetscFunctionReturn(0);}
  KSPMonitor(ksp,0,dp);
  PetscAMSTakeAccess(ksp);
  ksp->rnorm              = dp;
  PetscAMSGrantAccess(ksp);
  if (history) history[0] = dp;
  ierr = MatMult(Amat,P,Q); CHKERRQ(ierr);      /*    q <- A p          */

  for ( i=0; i<maxit; i++) {
    ksp->its++;

    ierr   = PCApply(ksp->B,Q,S); CHKERRQ(ierr);  /*     s <- Bq          */
    ierr   = VecDot(R,S,&btop); CHKERRQ(ierr);    /*                      */
    ierr   = VecDot(Q,S,&bbot); CHKERRQ(ierr);    /*     lambda =         */
    lambda = btop/bbot;
    ierr   = VecAXPY(&lambda,P,X); CHKERRQ(ierr); /*   x <- x + lambda p  */
    tmp    = -lambda; 
    ierr   = VecAXPY(&tmp,Q,R); CHKERRQ(ierr);     /*   r <- r - lambda q  */
    if (!ksp->avoidnorms) {
      ierr   = VecNorm(R,NORM_2,&dp); CHKERRQ(ierr); /*   dp <- r'*r         */
    } else { dp = 0.0; }
    PetscAMSTakeAccess(ksp);
    ksp->rnorm = dp;
    PetscAMSGrantAccess(ksp);
    if (history && hist_len > i + 1) history[i+1] = dp;
    KSPMonitor(ksp,i+1,dp);
    cerr   = (*ksp->converged)(ksp,i+1,dp,ksp->cnvP);
    if (cerr) break;
    ierr   = MatMult(Amat,S,T); CHKERRQ(ierr);    /*   T <-   As          */
    ierr   = VecDot(T,S,&btop); CHKERRQ(ierr);
    alpha0 = btop/bbot;
    ierr   = VecDot(T,Sm1,&btop); CHKERRQ(ierr);       
    alpha1 = btop/bbotold; 

    tmp = -alpha0; ierr = VecWAXPY(&tmp,P,S,Pp1); CHKERRQ(ierr);
    tmp = -alpha1; ierr = VecAXPY(&tmp,Pm1,Pp1); CHKERRQ(ierr);
    /* MM(Pp1,Qp1); use 3 term recurrence relation instead */
    tmp = -alpha0; ierr = VecAXPY(&tmp,Q,Qp1); CHKERRQ(ierr);
    tmp = -alpha1; ierr = VecAXPY(&tmp,Qm1,Qp1); CHKERRQ(ierr);
    /* scale the search direction !! Not mentioned in any reference */
    ierr = VecNorm(Pp1,NORM_2,&dp); CHKERRQ(ierr);
    tmp  = 1.0/dp; ierr = VecScale(&tmp,Pp1); CHKERRQ(ierr);
    ierr = VecScale(&tmp,Qp1); CHKERRQ(ierr);
    /* rotate work vectors */
    Tmp = Sm1; Sm1 = S; S = Tmp;
    Tmp = Pm1; Pm1 = P; P = Pp1; Pp1 = Tmp;
    Tmp = Qm1; Qm1 = Q; Q = Qp1; Qp1 = T = Tmp;
    bbotold = bbot; 
  }
  if (i == maxit) i--;
  if (history) ksp->res_act_size = (hist_len < i + 1) ? hist_len : i + 1;
  if (cerr <= 0) *its = -(i+1);
  else           *its = i + 1;
  PetscFunctionReturn(0);
}

EXTERN_C_BEGIN
#undef __FUNC__  
#define __FUNC__ "KSPCreate_CR"
int KSPCreate_CR(KSP ksp)
{
  PetscFunctionBegin;
  ksp->pc_side                   = PC_LEFT;
  ksp->calc_res                  = 1;
  ksp->ops->setup                = KSPSetUp_CR;
  ksp->ops->solve                = KSPSolve_CR;
  ksp->ops->destroy              = KSPDefaultDestroy;
  ksp->converged                 = KSPDefaultConverged;
  ksp->ops->buildsolution        = KSPDefaultBuildSolution;
  ksp->ops->buildresidual        = KSPDefaultBuildResidual;
  ksp->ops->view                 = 0;
  PetscFunctionReturn(0);
}
EXTERN_C_END
