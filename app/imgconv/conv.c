#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <wand/magick_wand.h>

int resize(MagickWand *mw, int type, int cols, int rows) {
	int rs = -1;
	unsigned long mw_cols = MagickGetImageWidth(mw);
	unsigned long mw_rows = MagickGetImageHeight(mw);

	if (type == 1) {
		if (cols == 0 || rows == 0) {
			if (cols > 0) {
				rows = (uint32_t)round(((double)cols / mw_cols) * mw_rows);
			} else {
				cols = (uint32_t)round(((double)rows / mw_rows) * mw_cols);
			}
			rs = MagickResizeImage(mw, cols, rows, UndefinedFilter, 1.0);
		} else	{
			uint32_t x = 0, y = 0, s_cols, s_rows;
			double cols_rate = (double)cols / mw_cols;
			double rows_rate = (double)rows / mw_rows;

			if (cols_rate > rows_rate) {
				s_cols = cols;
				s_rows = (uint32_t)round(cols_rate * mw_rows);
				y = (uint32_t)floor((s_rows - rows) / 2.0);
			} else {
				s_cols = (uint32_t)round(rows_rate * mw_cols);
				s_rows = rows;
				x = (uint32_t)floor((s_cols - cols) / 2.0);
			}
			rs = MagickResizeImage(mw, s_cols, s_rows, UndefinedFilter, 1.0);

			rs = MagickCropImage(mw, cols, rows, x, y);
		}
	} else if (type == 2) {
		uint32_t x, y;
		x = (uint32_t)floor((mw_cols - cols) / 2.0);
		y = (uint32_t)floor((mw_rows - rows) / 2.0);
		rs = MagickCropImage(mw, cols, rows, x, y);
	} else if (type == 3) {
		if (cols == 0 || rows == 0) {
			int rate = cols > 0 ? cols : rows;
			rows = (uint32_t)round(mw_rows * (double)rate / 100);
			cols = (uint32_t)round(mw_cols * (double)rate / 100);
			rs = MagickResizeImage(mw, cols, rows, UndefinedFilter, 1.0);
		} else {
			rows = (uint32_t)round(mw_rows * (double)rows / 100);
			cols = (uint32_t)round(mw_cols * (double)cols / 100);
			rs = MagickResizeImage(mw, cols, rows, UndefinedFilter, 1.0);
		}
	} else if (type == 0) {
		rs = MagickResizeImage(mw, cols, rows, UndefinedFilter, 1.0);
	}	else if (type == 4) {
		double rate = 1.0;
		if (cols == 0 || rows == 0) {
			rate = cols > 0 ? (double)cols / mw_cols : (double)rows / mw_rows;
		} else {
			double rate_col = (double)cols / mw_cols;
			double rate_row = (double)rows / mw_rows;
			rate = rate_col < rate_row ? rate_col : rate_row;
		}
		cols = (uint32_t)round(mw_cols * rate);
		rows = (uint32_t)round(mw_rows * rate);
		rs = MagickResizeImage(mw, cols, rows, UndefinedFilter, 1.0);
	}

	return rs;
}

void init() {
	MagickWandGenesis();
}


