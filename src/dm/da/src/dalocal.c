#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: dalocal.c,v 1.9 1997/08/22 15:18:43 bsmith Exp bsmith $";
#endif
 
/*
  Code for manipulating distributed regular arrays in parallel.
*/

#include "src/da/daimpl.h"    /*I   "da.h"   I*/

#undef __FUNC__  
#define __FUNC__ "DAGetLocalVector"
/*@C
   DAGetLocalVector - Gets a local vector (including ghost points) for a 
   distributed array.  Additional vectors of the same type can be created 
   with VecDuplicate().

   Input Parameter:
.  da - the distributed array

   Output Parameter:
.  l - the distributed vector

.keywords: distributed array, get, local, vector

.seealso: DAGetDistributedVector(), VecDuplicate(), VecDuplicateVecs()
@*/
int   DAGetLocalVector(DA da,Vec* l)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(da,DA_COOKIE);
  *l = da->local;
  PetscFunctionReturn(0);
}

