#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: dagtol.c,v 1.8 1997/07/09 21:00:44 balay Exp bsmith $";
#endif
 
/*
  Code for manipulating distributed regular arrays in parallel.
*/

#include "src/da/daimpl.h"    /*I   "da.h"   I*/

#undef __FUNC__  
#define __FUNC__ "DAGlobalToLocalBegin"
/*@
   DAGlobalToLocalBegin - Maps values from the global vector to the local
   patch; the ghost points are included. Must be followed by 
   DAGlobalToLocalEnd() to complete the exchange.

   Input Parameters:
.  da - the distributed array context
.  g - the global vector
.  mode - one of INSERT_VALUES or ADD_VALUES

   Output Parameter:
.  l  - the local values

.keywords: distributed array, global to local, begin

.seealso: DAGlobalToLocalEnd(), DALocalToGlobal(), DACreate2d()
@*/
int DAGlobalToLocalBegin(DA da,Vec g, InsertMode mode,Vec l)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(da,DA_COOKIE);
  ierr = VecScatterBegin(g,l,mode,SCATTER_FORWARD,da->gtol); CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DAGlobalToLocalEnd"
/*@
   DAGlobalToLocalEnd - Maps values from the global vector to the local
   patch; the ghost points are included. Must be preceeded by 
   DAGlobalToLocalBegin().

   Input Parameters:
.  da - the distributed array context
.  g - the global vector
.  mode - one of INSERT_VALUES or ADD_VALUES

   Output Parameter:
.  l  - the local values

.keywords: distributed array, global to local, end

.seealso: DAGlobalToLocalBegin(), DALocalToGlobal(), DACreate2d()
@*/
int DAGlobalToLocalEnd(DA da,Vec g, InsertMode mode,Vec l)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(da,DA_COOKIE);
  ierr = VecScatterEnd(g,l,mode,SCATTER_FORWARD,da->gtol); CHKERRQ(ierr);
  PetscFunctionReturn(0);
}

