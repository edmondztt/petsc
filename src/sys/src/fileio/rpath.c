#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: rpath.c,v 1.10 1997/08/22 15:11:48 bsmith Exp bsmith $";
#endif
/*
      Code for manipulating files.
*/
#include "src/sys/src/files.h"


#undef __FUNC__  
#define __FUNC__ "PetscGetRelativePath"
/*@C
   PetscGetRelativePath - Given a filename, returns the relative path (removes
   all directory specifiers).

   Input parameters:
.  fullpath  - full pathname
.  path      - pointer to buffer to hold relative pathname
.  flen     - size of path

.keywords: system, get, relative, path

.seealso: PetscGetFullPath()
@*/
int PetscGetRelativePath( char *fullpath, char *path, int flen )
{
  char  *p;

  PetscFunctionBegin;
  /* Find string after last '/' or entire string if no '/' */
  p = PetscStrrchr( fullpath, '/' );
  PetscStrncpy( path, p, flen );
  PetscFunctionReturn(0);
}
