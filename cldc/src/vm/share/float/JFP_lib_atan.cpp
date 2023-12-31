/*
 * Copyright  1990-2007 Sun Microsystems, Inc. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Sun Microsystems, Inc., 4150 Network Circle, Santa
 * Clara, CA 95054 or visit www.sun.com if you need additional
 * information or have any questions.
 */

/* atan(x)
 * Method
 *   1. Reduce x to positive by atan(x) = -atan(-x).
 *   2. According to the integer k=4t+0.25 chopped, t=x, the argument
 *      is further reduced to one of the following intervals and the
 *      arctangent of t is evaluated by the corresponding formula:
 *
 *      [0,7/16]      atan(x) = t-t^3*(a1+t^2*(a2+...(a10+t^2*a11)...)
 *      [7/16,11/16]  atan(x) = atan(1/2) + atan( (t-0.5)/(1+t/2) )
 *      [11/16.19/16] atan(x) = atan( 1 ) + atan( (t-1)/(1+t) )
 *      [19/16,39/16] atan(x) = atan(3/2) + atan( (t-1.5)/(1+1.5t) )
 *      [39/16,INF]   atan(x) = atan(INF) + atan( -1/t )
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following
 * constants. The decimal values may be used, provided that the
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "Globals.hpp"

#if ENABLE_FLOAT && (ENABLE_CLDC_111 || ENABLE_EXTENDED_API)

static const double atanhi[] = {
  4.63647609000806093515e-01, /* atan(0.5)hi 0x3FDDAC67, 0x0561BB4F */
  7.85398163397448278999e-01, /* atan(1.0)hi 0x3FE921FB, 0x54442D18 */
  9.82793723247329054082e-01, /* atan(1.5)hi 0x3FEF730B, 0xD281F69B */
  1.57079632679489655800e+00, /* atan(inf)hi 0x3FF921FB, 0x54442D18 */
};

static const double atanlo[] = {
  2.26987774529616870924e-17, /* atan(0.5)lo 0x3C7A2B7F, 0x222F65E2 */
  3.06161699786838301793e-17, /* atan(1.0)lo 0x3C81A626, 0x33145C07 */
  1.39033110312309984516e-17, /* atan(1.5)lo 0x3C700788, 0x7AF0CBBD */
  6.12323399573676603587e-17, /* atan(inf)lo 0x3C91A626, 0x33145C07 */
};

static const double aT[] = {
  3.33333333333329318027e-01, /* 0x3FD55555, 0x5555550D */
 -1.99999999998764832476e-01, /* 0xBFC99999, 0x9998EBC4 */
  1.42857142725034663711e-01, /* 0x3FC24924, 0x920083FF */
 -1.11111104054623557880e-01, /* 0xBFBC71C6, 0xFE231671 */
  9.09088713343650656196e-02, /* 0x3FB745CD, 0xC54C206E */
 -7.69187620504482999495e-02, /* 0xBFB3B0F2, 0xAF749A6D */
  6.66107313738753120669e-02, /* 0x3FB10D66, 0xA0D03D51 */
 -5.83357013379057348645e-02, /* 0xBFADDE2D, 0x52DEFD9A */
  4.97687799461593236017e-02, /* 0x3FA97B4B, 0x24760DEB */
 -3.65315727442169155270e-02, /* 0xBFA2B444, 0x2C6A6C2F */
  1.62858201153657823623e-02, /* 0x3F90AD3A, 0xE322DA11 */
};

static const double
atan_one   = 1.0,
atan_huge   = 1.0e300;

#ifdef __cplusplus
extern "C" {
#endif

double jvm_fplib_atan(double x) {
  double w,s1,s2,z;
  int ix,hx,id;

  hx = __JHI(x);
  ix = hx&0x7fffffff;
  if(ix>=0x44100000) {	/* if |x| >= 2^66 */
    if(ix>0x7ff00000||
       (ix==0x7ff00000&&(__JLO(x)!=0)))
      return jvm_dadd(x, x);		/* NaN */
    if(hx>0) return          jvm_dadd(atanhi[3], atanlo[3]);
    else     return jvm_dneg(jvm_dadd(atanhi[3], atanlo[3]));
  } if (ix < 0x3fdc0000) {	/* |x| < 0.4375 */
    if (ix < 0x3e200000) {	/* |x| < 2^-29 */
      if(jvm_dcmpl(jvm_dadd(atan_huge, x), atan_one) > 0) return x;	/* raise inexact */
    }
    id = -1;
  } else {
    x = jvm_fabs(x);
    if (ix < 0x3ff30000) {		/* |x| < 1.1875 */
      if (ix < 0x3fe60000) {	/* 7/16 <=|x|<11/16 */
        id = 0; x = jvm_ddiv(jvm_dsub(jvm_dmul(2.0, x), atan_one), jvm_dadd(2.0, x));
      } else {			/* 11/16<=|x|< 19/16 */
        id = 1; x  = jvm_ddiv(jvm_dsub(x, atan_one), jvm_dadd(x, atan_one));
      }
    } else {
      if (ix < 0x40038000) {	/* |x| < 2.4375 */
        id = 2; x  = jvm_ddiv(jvm_dsub(x, 1.5), jvm_dadd(atan_one, jvm_dmul(1.5, x)));
      } else {			/* 2.4375 <= |x| < 2^66 */
        id = 3; x  = jvm_ddiv(-1.0, x);
      }
    }}
  /* end of argument reduction */
  z = jvm_dmul(x, x);
  w = jvm_dmul(z, z);
  /* break sum from i=0 to 10 aT[i]z**(i+1) into odd and even poly */
  s1 = jvm_dmul(z, jvm_dadd(aT[0], jvm_dmul(w, jvm_dadd(aT[2], jvm_dmul(w, jvm_dadd(aT[4], jvm_dmul(w, jvm_dadd(aT[6], jvm_dmul(w, jvm_dadd(aT[8], jvm_dmul(w, aT[10])))))))))));
  s2 = jvm_dmul(w, jvm_dadd(aT[1], jvm_dmul(w, jvm_dadd(aT[3], jvm_dmul(w, jvm_dadd(aT[5], jvm_dmul(w, jvm_dadd(aT[7], jvm_dmul(w, aT[9])))))))));
  if (id<0) return jvm_dsub(x, jvm_dmul(x, jvm_dadd(s1, s2)));
  else {
    z = jvm_dsub(atanhi[id], jvm_dsub(jvm_dsub(jvm_dmul(x, jvm_dadd(s1, s2)), atanlo[id]), x));
    return (hx<0)? jvm_dneg(z):z;
  }
}

#ifdef __cplusplus
}
#endif

#endif // ENABLE_FLOAT && ENABLE_CLDC_111
