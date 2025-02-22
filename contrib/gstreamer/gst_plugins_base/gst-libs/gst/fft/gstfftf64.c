/* GStreamer
 * Copyright (C) <2007> Sebastian Dröge <slomo@circular-chaos.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <glib.h>
#include <math.h>

#include "kiss_fftr_f64.h"
#include "gstfft.h"
#include "gstfftf64.h"

/**
 * SECTION:gstfftf64
 * @short_description: FFT functions for 64 bit float samples
 *
 * #GstFFTF64 provides a FFT implementation and related functions for
 * 64 bit float samples. To use this call gst_fft_f64_new() for
 * allocating a #GstFFTF64 instance with the appropiate parameters and
 * then call gst_fft_f64_fft() or gst_fft_f64_inverse_fft() to perform the
 * FFT or inverse FFT on a buffer of samples.
 *
 * After use free the #GstFFTF64 instance with gst_fft_f64_free().
 *
 * For the best performance use gst_fft_next_fast_length() to get a
 * number that is entirely a product of 2, 3 and 5 and use this as the
 * @len parameter for gst_fft_f64_new().
 *
 * The @len parameter specifies the number of samples in the time domain that
 * will be processed or generated. The number of samples in the frequency domain
 * is @len/2 + 1. To get n samples in the frequency domain use 2*n - 2 as @len.
 *
 * Before performing the FFT on time domain data it usually makes sense
 * to apply a window function to it. For this gst_fft_f64_window() can comfortably
 * be used.
 *
 * Be aware, that you can't simply run gst_fft_f32_inverse_fft() on the
 * resulting frequency data of gst_fft_f32_fft() to get the original data back.
 * The relation between them is iFFT (FFT (x)) = x * nfft where nfft is the
 * length of the FFT. This also has to be taken into account when calculation
 * the magnitude of the frequency data.
 *
 */

/**
 * gst_fft_f64_new:
 * @len: Length of the FFT in the time domain
 * @inverse: %TRUE if the #GstFFTF64 instance should be used for the inverse FFT
 *
 * This returns a new #GstFFTF64 instance with the given parameters. It makes
 * sense to keep one instance for several calls for speed reasons.
 *
 * @len must be even and to get the best performance a product of
 * 2, 3 and 5. To get the next number with this characteristics use
 * gst_fft_next_fast_length().
 *
 * Returns: a new #GstFFTF64 instance.
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

GstFFTF64 *
gst_fft_f64_new (gint len, gboolean inverse)
{
  GstFFTF64 *self;

  g_return_val_if_fail (len > 0, NULL);
  g_return_val_if_fail (len % 2 == 0, NULL);

  self = g_new (GstFFTF64, 1);

  self->cfg = kiss_fftr_f64_alloc (len, (inverse) ? 1 : 0, NULL, NULL);
  g_assert (self->cfg);

  self->inverse = inverse;
  self->len = len;

  return self;
}

/**
 * gst_fft_f64_fft:
 * @self: #GstFFTF64 instance for this call
 * @timedata: Buffer of the samples in the time domain
 * @freqdata: Target buffer for the samples in the frequency domain
 *
 * This performs the FFT on @timedata and puts the result in @freqdata.
 *
 * @timedata must have as many samples as specified with the @len parameter while
 * allocating the #GstFFTF64 instance with gst_fft_f64_new().
 *
 * @freqdata must be large enough to hold @len/2 + 1 #GstFFTF64Complex frequency
 * domain samples.
 *
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_fft_f64_fft (GstFFTF64 * self, const gdouble * timedata,
    GstFFTF64Complex * freqdata)
{
  g_return_if_fail (self);
  g_return_if_fail (!self->inverse);
  g_return_if_fail (timedata);
  g_return_if_fail (freqdata);

  kiss_fftr_f64 (self->cfg, timedata, (kiss_fft_f64_cpx *) freqdata);
}

/**
 * gst_fft_f64_inverse_fft:
 * @self: #GstFFTF64 instance for this call
 * @freqdata: Buffer of the samples in the frequency domain
 * @timedata: Target buffer for the samples in the time domain
 *
 * This performs the inverse FFT on @freqdata and puts the result in @timedata.
 *
 * @freqdata must have @len/2 + 1 samples, where @len is the parameter specified
 * while allocating the #GstFFTF64 instance with gst_fft_f64_new().
 *
 * @timedata must be large enough to hold @len time domain samples.
 *
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_fft_f64_inverse_fft (GstFFTF64 * self, const GstFFTF64Complex * freqdata,
    gdouble * timedata)
{
  g_return_if_fail (self);
  g_return_if_fail (self->inverse);
  g_return_if_fail (timedata);
  g_return_if_fail (freqdata);

  kiss_fftri_f64 (self->cfg, (kiss_fft_f64_cpx *) freqdata, timedata);
}

/**
 * gst_fft_f64_free:
 * @self: #GstFFTF64 instance for this call
 *
 * This frees the memory allocated for @self.
 *
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_fft_f64_free (GstFFTF64 * self)
{
  kiss_fftr_f64_free (self->cfg);
  g_free (self);
}

/**
 * gst_fft_f64_window:
 * @self: #GstFFTF64 instance for this call
 * @timedata: Time domain samples
 * @window: Window function to apply
 *
 * This calls the window function @window on the @timedata sample buffer.
 *
 */
#ifdef __SYMBIAN32__
EXPORT_C
#endif

void
gst_fft_f64_window (GstFFTF64 * self, gdouble * timedata, GstFFTWindow window)
{
  gint i, len;

  g_return_if_fail (self);
  g_return_if_fail (timedata);

  len = self->len;

  switch (window) {
    case GST_FFT_WINDOW_RECTANGULAR:
      /* do nothing */
      break;
    case GST_FFT_WINDOW_HAMMING:
      for (i = 0; i < len; i++)
        timedata[i] *= (0.53836 - 0.46164 * cos (2.0 * M_PI * i / len));
      break;
    case GST_FFT_WINDOW_HANN:
      for (i = 0; i < len; i++)
        timedata[i] *= (0.5 - 0.5 * cos (2.0 * M_PI * i / len));
      break;
    case GST_FFT_WINDOW_BARTLETT:
      for (i = 0; i < len; i++)
        timedata[i] *= (1.0 - fabs ((2.0 * i - len) / len));
      break;
    case GST_FFT_WINDOW_BLACKMAN:
      for (i = 0; i < len; i++)
        timedata[i] *= (0.42 - 0.5 * cos ((2.0 * i) / len) +
            0.08 * cos ((4.0 * i) / len));
      break;
    default:
      g_assert_not_reached ();
      break;
  }
}
