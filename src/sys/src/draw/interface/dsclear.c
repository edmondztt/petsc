#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: dsclear.c,v 1.12 1997/10/10 04:04:31 bsmith Exp bsmith $";
#endif
/*
       Provides the calling sequences for all the basic Draw routines.
*/
#include "src/draw/drawimpl.h"  /*I "draw.h" I*/

#undef __FUNC__  
#define __FUNC__ "DrawSynchronizedClear" 
/*@
   DrawSynchronizedClear - Clears graphical output. All processors must call this routine.
       Does not return until the drawable is clear.

   Input Parameters:
.  draw - the drawing context

.keywords: draw, clear
@*/
int DrawSynchronizedClear(Draw draw)
{
  int ierr;
  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,DRAW_COOKIE);
  if (draw->type == DRAW_NULLWINDOW) PetscFunctionReturn(0);
  if (draw->ops.syncclear) {
    ierr = (*draw->ops.syncclear)(draw);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}
