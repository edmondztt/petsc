#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: draw.c,v 1.44 1997/08/22 15:15:58 bsmith Exp bsmith $";
#endif
/*
       Provides the calling sequences for all the basic Draw routines.
*/
#include "src/draw/drawimpl.h"  /*I "draw.h" I*/

#undef __FUNC__  
#define __FUNC__ "DrawResizeWindow"
/*@
   DrawResizeWindow - Allows one to resize a window from a program.

  Input Parameter:
.  draw - the window
.  w,h - the new width and height of the window

@*/
int DrawResizeWindow(Draw draw,int w,int h)
{
  int ierr;
  PetscFunctionBegin;
  if (draw->ops.resizewindow) {
    ierr = (*draw->ops.resizewindow)(draw,w,h);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawCheckResizedWindow" 
/*@
   DrawCheckResizedWindow - Checks if the user has resized the window.

  Input Parameter:
.  draw - the window

     Notes: 
      All processors that share this drawable must call this routine.

@*/
int DrawCheckResizedWindow(Draw draw)
{
  int ierr;
  PetscFunctionBegin;
  if (draw->ops.checkresizedwindow) {
    ierr = (*draw->ops.checkresizedwindow)(draw);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawGetTitle" 
/*@C
   DrawGetTitle - Gets pointer to title of a Draw context.

   Input Parameter:
.    draw - the graphics context

   Output Parameter:
.    title - the title
@*/
int DrawGetTitle(Draw draw,char **title)
{
  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,DRAW_COOKIE);
  *title = draw->title;
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawSetTitle" 
/*@C
   DrawSetTitle - Sets the title of a Draw context.

   Input Parameters:
.    draw - the graphics context
.    title - the title

   Note: A copy of the string is made, so you may destroy the 
         title string after calling this routine.
@*/
int DrawSetTitle(Draw draw,char *title)
{
  int ierr;
  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,DRAW_COOKIE);
  if (draw->title) PetscFree(draw->title);
  if (title) {
    int len = PetscStrlen(title);
    draw->title = (char *) PetscMalloc((len+1)*sizeof(char*));CHKPTRQ(draw->title);
    PLogObjectMemory(draw,(len+1)*sizeof(char*))
    PetscStrcpy(draw->title,title);
  } else {
    draw->title = 0;
  }
  if (draw->ops.settitle) {
    ierr = (*draw->ops.settitle)(draw,title);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawAppendTitle" 
/*@C
   DrawAppendTitle - Appends to the title of a Draw context.

   Input Parameters:
.    draw - the graphics context
.    title - the title

   Note: A copy of the string is made, so you may destroy the 
         title string after calling this routine.
@*/
int DrawAppendTitle(Draw draw,char *title)
{
  int  ierr;
  char *newtitle;
  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,DRAW_COOKIE);
  if (!title) PetscFunctionReturn(0);

  if (draw->title) {
    int len  = PetscStrlen(title) + PetscStrlen(draw->title);
    newtitle = (char *) PetscMalloc( (len + 1)*sizeof(char*) );CHKPTRQ(newtitle);
    PLogObjectMemory(draw,(len+1)*sizeof(char*));
    PetscStrcpy(newtitle,draw->title);
    PetscStrcat(newtitle,title);
    PetscFree(draw->title);
    draw->title = newtitle;
  } else {
    int len     = PetscStrlen(title);
    draw->title = (char *) PetscMalloc((len + 1)*sizeof(char*));CHKPTRQ(draw->title);
    PLogObjectMemory(draw,(len+1)*sizeof(char*));
    PetscStrcpy(draw->title,title);
  }
  if (draw->ops.settitle) {
    ierr = (*draw->ops.settitle)(draw,draw->title);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawDestroy" 
/*@C
   DrawDestroy - Deletes a draw context.

   Input Parameters:
.  draw - the drawing context

.keywords: draw, destroy
@*/
int DrawDestroy(Draw draw)
{
  int ierr;
  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,DRAW_COOKIE);
  if (--draw->refct > 0) PetscFunctionReturn(0);
  if (draw->destroy) {
    ierr = (*draw->destroy)((PetscObject)draw);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawCreatePopUp" 
/*@C
   DrawCreatePopUp - Creates a popup window associated with 
      a Draw window.

  Input Parameter:
.  draw - the original window

  Output Parameter:
.  popup - the new popup window
@*/
int DrawCreatePopUp(Draw draw,Draw *popup)
{
  int ierr;
  PetscFunctionBegin;
  PetscValidHeaderSpecific(draw,DRAW_COOKIE);
  PetscValidPointer(popup);

  if (draw->popup) {*popup = draw->popup; PetscFunctionReturn(0);}
  if (draw->ops.createpopup) {
    ierr = (*draw->ops.createpopup)(draw,popup);CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawDestroy_Null" 
int DrawDestroy_Null(PetscObject obj)
{
  PetscFunctionBegin;
  PLogObjectDestroy(obj);
  PetscHeaderDestroy(obj); 
  PetscFunctionReturn(0);
}

#undef __FUNC__  
#define __FUNC__ "DrawOpenNull" 
/*
  DrawOpenNull - Opens a null drawing context. All draw commands to 
  it are ignored.

  Output Parameter:
. win - the drawing context
*/
int DrawOpenNull(MPI_Comm comm,Draw *win)
{
  Draw draw;
  PetscFunctionBegin;
  *win = 0;
  PetscHeaderCreate(draw,_p_Draw,DRAW_COOKIE,DRAW_NULLWINDOW,comm,DrawDestroy,0);
  PLogObjectCreate(draw);
  PetscMemzero(&draw->ops,sizeof(struct _DrawOps));
  draw->destroy = DrawDestroy_Null;
  draw->view    = 0;
  draw->pause   = 0;
  draw->coor_xl = 0.0;  draw->coor_xr = 1.0;
  draw->coor_yl = 0.0;  draw->coor_yr = 1.0;
  draw->port_xl = 0.0;  draw->port_xr = 1.0;
  draw->port_yl = 0.0;  draw->port_yr = 1.0;
  draw->popup   = 0;
  *win = draw;
  PetscFunctionReturn(0);
}






