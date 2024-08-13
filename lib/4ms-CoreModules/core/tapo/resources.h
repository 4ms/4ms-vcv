// Copyright 2016 Matthias Puech.
//
// Author: Matthias Puech (matthias.puech@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// See http://creativecommons.org/licenses/MIT/ for more information.
//
// -----------------------------------------------------------------------------
//
// Resources definitions.
//
// Automatically generated with:
// make resources


#pragma once

#include "stmlib/stmlib.h"

namespace TapoDelay {


typedef uint8_t ResourceId;

extern const int16_t* lookup_table_int16_table[];

extern const float* lookup_table_table[];

extern const int16_t lut_preset_types[];
extern const int16_t lut_preset_sizes[];
extern const float lut_xfade_in[];
extern const float lut_xfade_out[];
extern const float lut_preset_times[];
extern const float lut_preset_velos[];
extern const float lut_preset_pans[];
#define LUT_PRESET_TYPES 0
#define LUT_PRESET_TYPES_SIZE 768
#define LUT_PRESET_SIZES 1
#define LUT_PRESET_SIZES_SIZE 24
#define LUT_XFADE_IN 0
#define LUT_XFADE_IN_SIZE 17
#define LUT_XFADE_OUT 1
#define LUT_XFADE_OUT_SIZE 17
#define LUT_PRESET_TIMES 2
#define LUT_PRESET_TIMES_SIZE 768
#define LUT_PRESET_VELOS 3
#define LUT_PRESET_VELOS_SIZE 768
#define LUT_PRESET_PANS 4
#define LUT_PRESET_PANS_SIZE 768

}

