#pragma once
#include "plugin.hh"
#include <blendish.h>

namespace MetaModule
{

void bndSetFont(int font);

void bndIconLabelCaret(NVGcontext *ctx,
					   float x,
					   float y,
					   float w,
					   float h,
					   int iconid,
					   NVGcolor color,
					   float fontsize,
					   const char *label,
					   NVGcolor caretcolor,
					   int cbegin,
					   int cend);

} // namespace MetaModule
