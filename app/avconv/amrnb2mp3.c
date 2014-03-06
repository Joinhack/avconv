#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <lame/lame.h>
#include "decode_amrnb.h"


typedef struct mp3_writer {
	FILE *mp3;
	lame_t lame;
} mp3_writer;

static void mp3_encode_ctx_close(encode_ctx *ctx) {
	if(ctx->priv != NULL) {
		mp3_writer *mw = (mp3_writer*)ctx->priv;
		if(mw->mp3)
			fclose(mw->mp3);
		if(mw->lame)
			lame_close(mw->lame);
		free(mw);
	}
}

static void mp3_encode_ctx_flush(encode_ctx *ctx) {
	unsigned char mp3_buffer[8192];
	mp3_writer *mw = (mp3_writer*)ctx->priv;
	lame_t lame = mw->lame;
	int n = lame_encode_flush(lame, mp3_buffer, 8192);
	if(n > 0)
		fwrite(mp3_buffer, n, 1, mw->mp3);
}

static void mp3_encode_ctx_write(encode_ctx *ctx, const unsigned char* data, int length) {
	unsigned char mp3_buffer[8192];
	mp3_writer *mw = (mp3_writer*)ctx->priv;
	lame_t lame = mw->lame;
	int n = lame_encode_buffer(lame, (short *)((char *)data), (short *)((char *)data), length/2, mp3_buffer, 8192);
	if(n > 0)
		fwrite(mp3_buffer, n, 1, mw->mp3);
}


static int mp3_encode_ctx_init(encode_ctx *ctx, const char *filename) {
	memset(ctx, 0, sizeof(*ctx));
	lame_t lame = lame_init();
	if (!lame) {
		return -1;
	}
	lame_set_num_channels(lame,1);
	lame_set_in_samplerate(lame,8000);
	lame_set_brate(lame,8);
	lame_set_mode(lame,3);
	lame_set_quality(lame,9);
	lame_init_params(lame);

	mp3_writer *mw = malloc(sizeof(*mw));
	mw->mp3 = fopen(filename, "wb");
	if (mw->mp3 == NULL) {
		free(mw);
		lame_close(lame);
		return -1;
	}
	mw->lame = lame;
	ctx->priv = mw;
	ctx->close = mp3_encode_ctx_close;
	ctx->write = mp3_encode_ctx_write;
	ctx->flush = mp3_encode_ctx_flush;
	return 0;
}

int amrnb2mp3(const char *amrpath, const char *mp3path) {
	void *wav, *amr;
	encode_ctx ctx;
	if (mp3_encode_ctx_init(&ctx, mp3path) < 0) {
		fprintf(stderr, "Unable to open %s\n", mp3path);
		return -1;
	}
	decode_amrnb(amrpath, &ctx);
	return 0;
}
