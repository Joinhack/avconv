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
}

static void wave_encode_ctx_write(encode_ctx *ctx, const unsigned char* data, int length) {
	void* wav = ctx->priv;
	wav_write_data(wav, data, 320);
}


static int wave_encode_ctx_init(encode_ctx *ctx, const char *wavpath) {
	memset(ctx, 0, sizeof(*ctx));
	void *wav = wav_write_open(wavpath, 8000, 16, 1);
	if (!wav) {
		return -1;
	}
	ctx->priv = wav;
	ctx->close = wave_encode_ctx_close;
	ctx->write = wave_encode_ctx_write;
	ctx->flush = wave_encode_ctx_flush;
	return 0;
}

int amrnb2wav(const char *amrpath, const char *wavpath) {
	encode_ctx ctx;
	if (wave_encode_ctx_init(&ctx, wavpath) < 0) {
		fprintf(stderr, "Unable to open %s\n", wavpath);
		return -1;
	}
	decode_amrnb(amrpath, &ctx);
	return 0;
}

