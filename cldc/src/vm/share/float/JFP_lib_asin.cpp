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

/* asin(x)
 * Return asin function of x.
 *
 * Method :
 *	Since  asin(x) = x + x^3/6 + x^5*3/40 + x^7*15/336 + ...
 *	we approximate asin(x) on [0,0.5] by
 *		asin(x) = x + x*x^2*R(x^2)
 *	where
 *		R(x^2) is a rational approximation of (asin(x)-x)/x^3
 *	and its remez error is bounded by
 *		|(asin(x)-x)/x^3 - R(x^2)| < 2^(-58.75)
 *
 *	For x in [0.5,1]
 *		asin(x) = pi/2-2*asin(sqrt((1-x)/2))
 *	Let y = (1-x), z = y/2, s := sqrt(z), and pio2_hi+pio2_lo=pi/2;
 *	then for x>0.98
 *		asin(x) = pi/2 - 2*(s+s*z*R(z))
 *			= pio2_hi - (2*(s+s*z*R(z)) - pio2_lo)
 *	For x<=0.98, let pio4_hi = pio2_hi/2, then
 *		f = hi part of s;
 *		c = sqrt(z) - f = (z-f*f)/(s+f) 	...f+c=sqrt(z)
 *	and
 *		asin(x) = pi/2 - 2*(s+s*z*R(z))
 *			= pio4_hi+(pio4-2s)-(2s*z*R(z)-pio2_lo)
 *			= pio4_hi+(pio4-2f)-(2s*z*R(z)-(pio2_lo+2c))
 *
 * Special cases:
 *	if x is NaN, return x itself;
 *	if |x|>1, return NaN with invalid signal.
 *
 */

#include "jvmconfig.h"

#include "BuildFlags.hpp"
#include "GlobalDefinitions.hpp"
#include "GlobalDefinitions_c.hpp"
#include "Globals.hpp"

#if ENABLE_FLOAT && (ENABLE_CLDC_111 || ENABLE_EXTENDED_API)

static const double
as_one =  1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
as_huge =  1.000e+300,
as_pio2_hi =  1.57079632679489655800e+00, /* 0x3FF921FB, 0x54442D18 */
as_pio2_lo =  6.12323399573676603587e-17, /* 0x3C91A626, 0x33145C07 */
as_pio4_hi =  7.85398163397448278999e-01, /* 0x3FE921FB, 0x54442D18 */
	/* coefficient for R(x^2) */
as_pS0 =  1.66666666666666657415e-01, /* 0x3FC55555, 0x55555555 */
as_pS1 = -3.25565818622400915405e-01, /* 0xBFD4D612, 0x03EB6F7D */
as_pS2 =  2.01212532134862925881e-01, /* 0x3FC9C155, 0x0E884455 */
as_pS3 = -4.00555345006794114027e-02, /* 0xBFA48228, 0xB5688F3B */
as_pS4 =  7.91534994289814532176e-04, /* 0x3F49EFE0, 0x7501B288 */
as_pS5 =  3.47933107596021167570e-05, /* 0x3F023DE1, 0x0DFDF709 */
as_qS1 = -2.40339491173441421878e+00, /* 0xC0033A27, 0x1C8A2D4B */
as_qS2 =  2.02094576023350569471e+00, /* 0x40002AE5, 0x9C598AC8 */
as_qS3 = -6.88283971605453293030e-01, /* 0xBFE6066C, 0x1B8D0159 */
as_qS4 =  7.70381505559019352791e-02; /* 0x3FB3B8C5, 0xB12E9282 */

#ifdef __cplusplus
extern "C" {
#endif

double jvm_fplib_asin(double x) {
  double t=0,w,p,q,c,r,s;
  int hx,ix;
  hx = __JHI(x);
  ix = hx&0x7fffffff;
  if(ix>= 0x3ff00000) {		/* |x|>= 1 */
    if(((ix-0x3ff00000)|__JLO(x))==0)
      /* asin(1)=+-pi/2 with inexact */
      return jvm_dadd(jvm_dmul(x, as_pio2_hi), jvm_dmul(x, as_pio2_lo));
    return jvm_ddiv(jvm_dsub(x, x), jvm_dsub(x, x));		/* asin(|x|>1) is NaN */
  } else if (ix<0x3fe00000) {	/* |x|<0.5 */
    if(ix<0x3e400000) {		/* if |x| < 2**-27 */
      if(jvm_dcmpl(jvm_dadd(as_huge, x), as_one) > 0) return x;/* return x with inexact if x!=0*/
    } else
      t = jvm_dmul(x, x);
    p = jvm_dmul(t, jvm_dadd(as_pS0, jvm_dmul(t, jvm_dadd(as_pS1, jvm_dmul(t, jvm_dadd(as_pS2, jvm_dmul(t, jvm_dadd(as_pS3, jvm_dmul(t, jvm_dadd(as_pS4, jvm_dmul(t, as_pS5)))))))))));
    q = jvm_dadd(as_one, jvm_dmul(t, jvm_dadd(as_qS1, jvm_dmul(t, jvm_dadd(as_qS2, jvm_dmul(t, jvm_dadd(as_qS3, jvm_dmul(t, as_qS4))))))));
    w = jvm_ddiv(p, q);
    return jvm_dadd(x, jvm_dmul(x, w));
  }
  /* 1> |x|>= 0.5 */
  w = jvm_dsub(as_one, jvm_fabs(x));
  t = jvm_dmul(w, 0.5);
  p = jvm_dmul(t, jvm_dadd(as_pS0, jvm_dmul(t, jvm_dadd(as_pS1, jvm_dmul(t, jvm_dadd(as_pS2, jvm_dmul(t, jvm_dadd(as_pS3, jvm_dmul(t, jvm_dadd(as_pS4, jvm_dmul(t, as_pS5)))))))))));
  q = jvm_dadd(as_one, jvm_dmul(t, jvm_dadd(as_qS1, jvm_dmul(t, jvm_dadd(as_qS2, jvm_dmul(t, jvm_dadd(as_qS3, jvm_dmul(t, as_qS4))))))));
  s = ieee754_sqrt(t);
  if(ix>=0x3FEF3333) { 	/* if |x| > 0.975 */
    w = jvm_ddiv(p, q);
    t = jvm_dsub(as_pio2_hi, jvm_dsub(jvm_dmul(2.0, jvm_dadd(s, jvm_dmul(s, w))), as_pio2_lo));
  } else {
    w  = s;
    __JLO(w) = 0;
    c  = jvm_ddiv(jvm_dsub(t, jvm_dmul(w, w)), jvm_dadd(s, w));
    r  = jvm_ddiv(p, q);
    p  = jvm_dsub(jvm_dmul(jvm_dmul(2.0, s), r), jvm_dsub(as_pio2_lo, jvm_dmul(2.0, c)));
    q  = jvm_dsub(as_pio4_hi, jvm_dmul(2.0, w));
    t  = jvm_dsub(as_pio4_hi, jvm_dsub(p, q));
  }
  if(hx>0) return t; else return jvm_dneg(t);
}

#ifdef __cplusplus
}
#endif

#endif // ENABLE_FLOAT && ENABLE_CLDC_111
