
/* @(#)e_gamma.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/* gamma(x)
 * Return the logarithm of the Gamma function of x.
 *
 * Method: call gamma_r
 */

#include "math_private.h"

extern int signgam;

#ifdef __STDC__
	//__private_extern__
	double gamma(double x)
#else
	double gamma(x)
	double x;
#endif
{
	return gamma_r(x,&signgam);
}
