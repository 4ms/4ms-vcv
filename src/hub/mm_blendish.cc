#include "mm_blendish.hh"
#include <math.h>
#include <memory.h>

namespace MetaModule
{

static int bnd_font = -1;

void bndSetFont(int font) {
	bnd_font = font;
}

static void bndCaretPosition(NVGcontext *ctx,
							 float x,
							 float y,
							 float desc,
							 float lineHeight,
							 const char *caret,
							 NVGtextRow *rows,
							 int nrows,
							 int *cr,
							 float *cx,
							 float *cy) {
	static NVGglyphPosition glyphs[BND_MAX_GLYPHS];
	int r, nglyphs;
	for (r = 0; r < nrows - 1 && rows[r].end < caret; ++r)
		;
	*cr = r;
	*cx = x;
	*cy = y - lineHeight - desc + r * lineHeight;
	if (nrows == 0)
		return;
	*cx = rows[r].minx;
	nglyphs = nvgTextGlyphPositions(ctx, x, y, rows[r].start, rows[r].end + 1, glyphs, BND_MAX_GLYPHS);
	for (int i = 0; i < nglyphs; ++i) {
		*cx = glyphs[i].x;
		if (glyphs[i].str == caret)
			break;
	}
}

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
					   int cend) {
	float pleft = BND_TEXT_RADIUS;
	if (!label)
		return;
	if (iconid >= 0) {
		bndIcon(ctx, x + 4, y + 2, iconid);
		pleft += BND_ICON_SHEET_RES;
	}

	if (bnd_font < 0)
		return;

	x += pleft;
	y += BND_WIDGET_HEIGHT - BND_TEXT_PAD_DOWN;

	nvgFontFaceId(ctx, bnd_font);
	nvgFontSize(ctx, fontsize);
	nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_BASELINE);

	w -= BND_TEXT_RADIUS + pleft;

	if (cend >= cbegin) {
		int c0r, c1r;
		float c0x, c0y, c1x, c1y;
		float desc, lh;
		static NVGtextRow rows[BND_MAX_ROWS];
		int nrows = nvgTextBreakLines(ctx, label, label + cend + 1, w, rows, BND_MAX_ROWS);
		nvgTextMetrics(ctx, NULL, &desc, &lh);

		bndCaretPosition(ctx, x, y, desc, lh, label + cbegin, rows, nrows, &c0r, &c0x, &c0y);
		bndCaretPosition(ctx, x, y, desc, lh, label + cend, rows, nrows, &c1r, &c1x, &c1y);

		nvgBeginPath(ctx);
		if (cbegin == cend) {
			// nvgFillColor(ctx, nvgRGBf(0.337, 0.502, 0.761));
			nvgFillColor(ctx, nvgRGBf(0.9, 0.9, 0.9));
			nvgRect(ctx, c0x - 1, c0y, 2, lh + 1);
		} else {
			nvgFillColor(ctx, caretcolor);
			if (c0r == c1r) {
				nvgRect(ctx, c0x - 1, c0y, c1x - c0x + 1, lh + 1);
			} else {
				int blk = c1r - c0r - 1;
				nvgRect(ctx, c0x - 1, c0y, x + w - c0x + 1, lh + 1);
				nvgRect(ctx, x, c1y, c1x - x + 1, lh + 1);

				if (blk)
					nvgRect(ctx, x, c0y + lh, w, blk * lh + 1);
			}
		}
		nvgFill(ctx);
	}

	nvgBeginPath(ctx);
	nvgFillColor(ctx, color);
	nvgTextBox(ctx, x, y, w, label, NULL);
}

} // namespace MetaModule
