/**
 * Copyright (C) 2013, 2014 Johannes Taelman
 *
 * This file is part of Axoloti.
 *
 * Axoloti is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Axoloti is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Axoloti. If not, see <http://www.gnu.org/licenses/>.
 */
#include "axoloti_math.h"

int16_t sinet[SINETSIZE + 1];
int32_t sine2t[SINE2TSIZE + 1];
int16_t windowt[WINDOWSIZE + 1];
uint32_t pitcht[PITCHTSIZE];
uint16_t expt[EXPTSIZE];
uint16_t logt[LOGTSIZE];

void axoloti_math_init(void)
{
	static bool already_init = false;

	if (!already_init) {
		already_init = true;
		short *p;
		uint32_t i;
		p = (short *)sinet;
		for (i = 0; i < SINETSIZE + 1; i++) {
			float f = i * 2 * PI_F / (float)SINETSIZE; //0..2pi
			float sin_f = sinf(f);					   //0..1..0..-1
			int32_t isinf = (32767.0f * sin_f);		   // -32767 .. 32767  S | dec:15 | frac:16
			*p++ = isinf;

			// volatile int q = arm_sin_q31(i << 21); //returns -1 to +1 in Q31: S | dec:0 | frac:31
			// *p++ = (int16_t)(q >> 16); //Q31 => Q15 -1 to +1 in Q15 which is S | dec 15 | frac 15
		}

		for (i = 0; i < SINE2TSIZE + 1; i++) {
			float f = i * 2 * PI_F / (float)SINE2TSIZE;
			sine2t[i] = (int32_t)((float)INT32_MAX * sinf(f));
		}
		//Fix: sine2t[1024] should be 0x7FFFFFFF but is 0x80000000
		//The reason is that the float value of INT32_MAX cannot be precisely represented by a float
		sine2t[1024] = 0x7FFFFFFF;

		p = (short *)windowt;
		for (i = 0; i < WINDOWSIZE + 1; i++) {
			float f = i * 2 * PI_F / (float)WINDOWSIZE;
			*p++ = (int16_t)(32767.0f * (0.5f - 0.5f * cosf(f)));
		}

		uint32_t *q;
		q = (uint32_t *)pitcht;
		for (i = 0; i < PITCHTSIZE; i++) {
			//Fix: powf returns a double, so we use pow() to avoid the a hidden float->double promotion:
			// double f = 440.0 * powf(2.0, (i - 69.0 - 64.0) / 12.0);
			double f = 440.0 * pow(2.0, (i - 69.0 - 64.0) / 12.0);
			double phi = 4.0 * (double)(1 << 30) * f / (SAMPLERATE * 1.0);
			if (phi > ((unsigned int)1 << 31))
				phi = 0x7FFFFFFF;
			*q++ = (uint32_t)phi;
		}

		uint16_t *q16;
		q16 = expt;
		for (i = 0; i < EXPTSIZE; i++) {
			double e = pow(2.0, ((float)i) / (float)EXPTSIZE);
			*q16++ = (uint32_t)(e * (1 + INT16_MAX));
		}

		q16 = logt;
		for (i = 0; i < LOGTSIZE; i++) {
			double e = 0.5 * log(1.0 + ((double)i / (double)LOGTSIZE)) / log(2.0);
			*q16++ = (uint32_t)(e * (1 + INT16_MAX));
		}
	}
}

uint32_t FastLog(uint32_t i)
{
	Float_t f;
	f.f = i;
	uint32_t r = f.parts.exponent << 23;
	r += f.parts.mantissa >> 10;
	return r;
}
