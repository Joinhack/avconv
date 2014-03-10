#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "decode_amrnb.h"
#include "wavwriter.h"


static void wave_encode_ctx_close(encode_ctx *ctx) {
	if(ctx->priv != NULL) {
		wav_write_close(ctx->priv);
		ctx->priv = NULL;
	}
}

static void wave_encode_ctx_flush(encode_ctx *ctx) {
	if(ctx->priv != NULL) {
		wav_write_flush(ctx->priv);
	}
}

static void wave_encode_ctx_write(encode_ctx *ctx, const unsigned char* data, int length) {
	void* wav = ctx->priv;
	wav_write_data(wav, data, 320);
}

static int wave_encode_ctx_init(encode_ctx *ctx) {
	void *wav = wav_write_open(ctx->output, 8000, 16, 1);
	if (wav == NULL) {
		return -1;
	}
	ctx->priv = wav;
	return 0;
}

static void init_wave_encode_ctx(encode_ctx *ctx, const char *wavpath) {
	memset(ctx, 0, sizeof(*ctx));
	ctx->output = wavpath;
	ctx->init = wave_encode_ctx_init;
	ctx->close = wave_encode_ctx_close;
	ctx->write = wave_encode_ctx_write;
	ctx->flush = wave_encode_ctx_flush;
}

int amrnb2wav(const char *amrpath, const char *wavpath) {
	encode_ctx ctx;
	init_wave_encode_ctx(&ctx, wavpath);
	decode_amrnb(amrpath, &ctx);
	ctx.close(&ctx);
	return 0;
}

