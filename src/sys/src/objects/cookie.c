#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: cookie.c,v 1.13 1997/08/22 15:11:48 bsmith Exp bsmith $";
#endif

#include "petsc.h"  /*I "petsc.h" I*/
int LARGEST_PETSC_COOKIE = LARGEST_PETSC_COOKIE_PREDEFINED;

#undef __FUNC__  
#define __FUNC__ "PetscRegisterCookie"
/*@
    PetscRegisterCookie - Registers a new cookie for use with a
    newly created PETSc object class.  The user should pass in
    a variable initialized to zero; then it will be assigned a cookie.
    Repeated calls to this routine with the same variable will 
    not change the cookie. 

    Output Parameter:
.   cookie - the cookie you have been assigned

    Note:
    The initial cookie variable MUST be set to zero on the
    first call to this routine.

.keywords:  register, cookie
@*/
int PetscRegisterCookie(int *cookie)
{
  PetscFunctionBegin;
  if (LARGEST_PETSC_COOKIE >= LARGEST_PETSC_COOKIE_ALLOWED) { 
    SETERRQ(1,0,"You have used too many PETSc cookies");
  }
  if (!*cookie) *cookie = LARGEST_PETSC_COOKIE++;
  PetscFunctionReturn(0);
}
