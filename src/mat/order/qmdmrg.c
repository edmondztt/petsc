/* qmdmrg.f -- translated by f2c (version 19931217).*/

#include "petsc.h"

/******************************************************************/
/***********     QMDMRG ..... QUOT MIN DEG MERGE       ************/
/******************************************************************/
/*    PURPOSE - THIS ROUTINE MERGES INDISTINGUISHABLE NODES IN   */
/*              THE MINIMUM DEGREE ORDERING ALGORITHM.           */
/*              IT ALSO COMPUTES THE NEW DEGREES OF THESE        */
/*              NEW SUPERNODES.                                  */
/*                                                               */
/*    INPUT PARAMETERS -                                         */
/*       (XADJ, ADJNCY) - THE ADJACENCY STRUCTURE.               */
/*       DEG0 - THE NUMBER OF NODES IN THE GIVEN SET.            */
/*       (NHDSZE, NBRHD) - THE SET OF ELIMINATED SUPERNODES      */
/*              ADJACENT TO SOME NODES IN THE SET.               */
/*                                                               */
/*    UPDATED PARAMETERS -                                       */
/*       DEG - THE DEGREE VECTOR.                                */
/*       QSIZE - SIZE OF INDISTINGUISHABLE NODES.                */
/*       QLINK - LINKED LIST FOR INDISTINGUISHABLE NODES.        */
/*       MARKER - THE GIVEN SET IS GIVEN BY THOSE NODES WITH     */
/*              MARKER VALUE SET TO 1.  THOSE NODES WITH DEGREE  */
/*              UPDATED WILL HAVE MARKER VALUE SET TO 2.         */
/*                                                               */
/*    WORKING PARAMETERS -                                       */
/*       RCHSET - THE REACHABLE SET.                             */
/*       OVRLP -  TEMP VECTOR TO STORE THE INTERSECTION OF TWO   */
/*              REACHABLE SETS.                                  */
/*                                                               */
/*****************************************************************/
#undef __FUNC__  
#define __FUNC__ "qmdmrg" 
int qmdmrg(int *xadj, int *adjncy, int *deg, 
	int *qsize, int *qlink, int *marker, int *deg0, 
	int *nhdsze, int *nbrhd, int *rchset, int *ovrlp)
{
    /* System generated locals */
    int i__1, i__2, i__3;

    /* Local variables */
    static int head, inhd, irch, node, mark, link, root, j, lnode, nabor, 
	    jstop, jstrt, rchsze, mrgsze, novrlp, iov, deg1;

    PetscFunctionBegin;
    /* Parameter adjustments */
    --ovrlp;
    --rchset;
    --nbrhd;
    --marker;
    --qlink;
    --qsize;
    --deg;
    --adjncy;
    --xadj;

    if (*nhdsze <= 0) {
	PetscFunctionReturn(0);
    }
    i__1 = *nhdsze;
    for (inhd = 1; inhd <= i__1; ++inhd) {
	root = nbrhd[inhd];
	marker[root] = 0;
    }
/*       LOOP THROUGH EACH ELIMINATED SUPERNODE IN THE SET     */
/*       (NHDSZE, NBRHD).                                      */
    i__1 = *nhdsze;
    for (inhd = 1; inhd <= i__1; ++inhd) {
	root = nbrhd[inhd];
	marker[root] = -1;
	rchsze = 0;
	novrlp = 0;
	deg1 = 0;
L200:
	jstrt = xadj[root];
	jstop = xadj[root + 1] - 1;
/*          DETERMINE THE REACHABLE SET AND ITS INTERSECT-     */
/*          ION WITH THE INPUT REACHABLE SET.                  */
	i__2 = jstop;
	for (j = jstrt; j <= i__2; ++j) {
	    nabor = adjncy[j];
	    root = -nabor;
	    if (nabor < 0) {
		goto L200;
	    } else if (nabor == 0) {
		goto L700;
	    } else {
		goto L300;
	    }
L300:
	    mark = marker[nabor];
	    if (mark < 0) {
		goto L600;
	    } else if (mark == 0) {
		goto L400;
	    } else {
		goto L500;
	    }
L400:
	    ++rchsze;
	    rchset[rchsze] = nabor;
	    deg1 += qsize[nabor];
	    marker[nabor] = 1;
	    goto L600;
L500:
	    if (mark > 1) {
		goto L600;
	    }
	    ++novrlp;
	    ovrlp[novrlp] = nabor;
	    marker[nabor] = 2;
L600:
	    ;
	}
/*          FROM THE OVERLAPPED SET, DETERMINE THE NODES        */
/*          THAT CAN BE MERGED TOGETHER.                        */
L700:
	head = 0;
	mrgsze = 0;
	i__2 = novrlp;
	for (iov = 1; iov <= i__2; ++iov) {
	    node = ovrlp[iov];
	    jstrt = xadj[node];
	    jstop = xadj[node + 1] - 1;
	    i__3 = jstop;
	    for (j = jstrt; j <= i__3; ++j) {
		nabor = adjncy[j];
		if (marker[nabor] != 0) {
		    goto L800;
		}
		marker[node] = 1;
		goto L1100;
L800:
		;
	    }
/*             NODE BELONGS TO THE NEW MERGED SUPERNODE.      */
/*             UPDATE THE VECTORS QLINK AND QSIZE.            */
	    mrgsze += qsize[node];
	    marker[node] = -1;
	    lnode = node;
L900:
	    link = qlink[lnode];
	    if (link <= 0) {
		goto L1000;
	    }
	    lnode = link;
	    goto L900;
L1000:
	    qlink[lnode] = head;
	    head = node;
L1100:
	    ;
	}
	if (head <= 0) {
	    goto L1200;
	}
	qsize[head] = mrgsze;
	deg[head] = *deg0 + deg1 - 1;
	marker[head] = 2;
/*          RESET MARKER VALUES.          */
L1200:
	root = nbrhd[inhd];
	marker[root] = 0;
	if (rchsze <= 0) {
	    goto L1400;
	}
	i__2 = rchsze;
	for (irch = 1; irch <= i__2; ++irch) {
	    node = rchset[irch];
	    marker[node] = 0;
	}
L1400:
	;
    }
    PetscFunctionReturn(0);
}

