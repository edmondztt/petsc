#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: flush.c,v 1.11 1997/08/22 15:17:45 bsmith Exp bsmith $";
#endif

#include "petsc.h"  /*I "petsc.h" I*/

struct _p_Viewer {
  PETSCHEADER
  int         (*flush)(Viewer);
};

#undef __FUNC__  
#define __FUNC__ "ViewerFlush"
/*@
   ViewerFlush - Flushes a viewer (i.e. tries to dump all the 
   data that has been printed through a viewer).

   Input Parameters:
.  viewer - the viewer to be flushed

.keywords: Viewer, flush

.seealso: ViewerMatlabOpen(), ViewerFileOpenASCII()
@*/
int ViewerFlush(Viewer v)
{
  int ierr;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(v,VIEWER_COOKIE);
  if (v->flush) {
    ierr = (*v->flush)(v);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}


