/* $Id: dot.h,v 1.6 1997/09/18 16:06:38 balay Exp bsmith $ */

#ifndef DOT

#if defined(USE_FORTRAN_KERNELS)

#if defined(HAVE_FORTRAN_CAPS)
#define fortranmdot4_      FORTRANMDOT4
#define fortranmdot3_      FORTRANMDOT3
#define fortranmdot2_      FORTRANMDOT2
#define fortranmdot1_      FORTRANMDOT1
#define fortrannormsqr_    FORTRANNORMSQR
#define fortranmultaij_    FORTRANMULTAIJ
#define fortranmultaddaij_ FORTRANMULTADDAIJ
#define fortransolveaij_   FORTRANSOLVEAIJ
#elif !defined(HAVE_FORTRAN_UNDERSCORE)
#define fortranmdot4_      fortranmdot4
#define fortranmdot3_      fortranmdot3
#define fortranmdot2_      fortranmdot2
#define fortranmdot1_      fortranmdot1
#define fortrannormsqr_    fortrannormsqr
#define fortranmultaij_    fortranmultaij
#define fortranmultaddaij_ fortranmultaddaij
#define fortransolveaij_   fortransolveaij
#endif

#if defined(__cplusplus)
extern "C" {
#endif
extern void fortranmdot4_(void *, void *,void *,void *,void *,int *,
                           void *, void *,void *,void *);
extern void fortranmdot3_(void *,void *,void *,void *,int *,
                           void *, void *,void *);
extern void fortranmdot2_(void *,void *,void *,int *,
                           void *, void *);
extern void fortranmdot1_(void *,void *,int *,
                           void *);
extern void fortrannormsqr_(void *,int *,void *);
extern void fortranmultaij_(int *,void*,int *,int *,void *,void*);
extern void fortranmultaddaij_(int *,void*,int *,int *,void *,void*,void*);
extern void fortransolveaij_(int *,void*,int *,int *,int*,void *,void*);
#if defined(__cplusplus)
}
#endif
#endif

/* ------------------------------------------------------------------- */


#if !defined(USE_PETSC_COMPLEX)

#ifdef USE_UNROLL_KERNELS
#define DOT(sum,x,y,n) {\
switch (n & 0x3) {\
case 3: sum += *x++ * *y++;\
case 2: sum += *x++ * *y++;\
case 1: sum += *x++ * *y++;\
n -= 4;case 0:break;}\
while (n>0) {sum += x[0]*y[0]+x[1]*y[1]+x[2]*y[2]+x[3]*y[3];x+=4;y+=4;\
n -= 4;}}
#define DOT2(sum1,sum2,x,y1,y2,n) {\
if(n&0x1){sum1+=*x**y1++;sum2+=*x++**y2++;n--;}\
while (n>0) {sum1+=x[0]*y1[0]+x[1]*y1[1];sum2+=x[0]*y2[0]+x[1]*y2[1];x+=2;\
y1+=2;y2+=2;n -= 2;}}
#define SQR(sum,x,n) {\
switch (n & 0x3) {\
case 3: sum += *x * *x;x++;\
case 2: sum += *x * *x;x++;\
case 1: sum += *x * *x;x++;\
n -= 4;case 0:break;}\
while (n>0) {sum += x[0]*x[0]+x[1]*x[1]+x[2]*x[2]+x[3]*x[3];x+=4;\
n -= 4;}}

#elif defined(USE_WHILE_KERNELS)
#define DOT(sum,x,y,n) {
while(n--) sum+= *x++ * *y++;}
#define DOT2(sum1,sum2,x,y1,y2,n) {\
while(n--){sum1+= *x**y1++;sum2+=*x++**y2++;}}
#define SQR(sum,x,n)   {\
while(n--) {sum+= *x * *x; x++;}}

#elif defined(USE_BLAS_KERNELS)
extern double ddot_();
#define DOT(sum,x,y,n) {int one=1;\
sum=ddot_(&n,x,&one,y,&one);}
#define DOT2(sum1,sum2,x,y1,y2,n) {int __i;\
for(__i=0;__i<n;__i++){sum1+=x[__i]*y1[__i];sum2+=x[__i]*y2[__i];}}
#define SQR(sum,x,n)   {int one=1;\
sum=ddot_(&n,x,&one,x,&one);}

#else
#define DOT(sum,x,y,n) {int __i;\
for(__i=0;__i<n;__i++)sum+=x[__i]*y[__i];}
#define DOT2(sum1,sum2,x,y1,y2,n) {int __i;\
for(__i=0;__i<n;__i++){sum1+=x[__i]*y1[__i];sum2+=x[__i]*y2[__i];}}
#define SQR(sum,x,n)   {int __i;\
for(__i=0;__i<n;__i++)sum+=x[__i]*x[__i];}
#endif

#else

#ifdef USE_UNROLL_KERNELS
#define DOT(sum,x,y,n) {\
switch (n & 0x3) {\
case 3: sum += *x * conj(*y); x++; y++;\
case 2: sum += *x * conj(*y); x++; y++;\
case 1: sum += *x * conj(*y); x++; y++;\
n -= 4;case 0:break;}\
while (n>0) {sum += x[0]*conj(y[0])+x[1]*conj(y[1])+x[2]*conj(y[2])+x[3]*conj(y[3]);x+=4;y+=4;\
n -= 4;}}
#define DOT2(sum1,sum2,x,y1,y2,n) {\
if(n&0x1){sum1+=*x*conj(*y1)++;sum2+=*x++*conj(*y2)++;n--;}\
while (n>0) {sum1+=x[0]*conj(y1[0])+x[1]*conj(y1[1]);sum2+=x[0]*conj(y2[0])+x[1]*conj(y2[1]);x+=2;\
y1+=2;y2+=2;n -= 2;}}
#define SQR(sum,x,n) {\
switch (n & 0x3) {\
case 3: sum += *x * conj(*x);x++;\
case 2: sum += *x * conj(*x);x++;\
case 1: sum += *x * conj(*x);x++;\
n -= 4;case 0:break;}\
while (n>0) {sum += x[0]*conj(x[0])+x[1]*conj(x[1])+x[2]*conj(x[2])+x[3]*conj(x[3]);x+=4;\
n -= 4;}}

#elif defined(USE_WHILE_KERNELS)
#define DOT(sum,x,y,n) {
while(n--) sum+= *x++ * conj(*y++);}
#define DOT2(sum1,sum2,x,y1,y2,n) {\
while(n--){sum1+= *x*conj(*y1);sum2+=*x*conj(*y2); x++; y1++; y2++;}}
#define SQR(sum,x,n)   {\
while(n--) {sum+= *x * conj(*x); x++;}}

#else
#define DOT(sum,x,y,n) {int __i;\
for(__i=0;__i<n;__i++)sum+=x[__i]*conj(y[__i]);}
#define DOT2(sum1,sum2,x,y1,y2,n) {int __i;\
for(__i=0;__i<n;__i++){sum1+=x[__i]*conj(y1[__i]);sum2+=x[__i]*conj(y2[__i]);}}
#define SQR(sum,x,n)   {int __i;\
for(__i=0;__i<n;__i++)sum+=x[__i]*conj(x[__i]);}
#endif

#endif

#endif
