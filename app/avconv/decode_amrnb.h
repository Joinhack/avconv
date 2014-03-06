
#ifndef DECODE_AMRNB_H
#define DECODE_AMRNB_H

#ifdef __cplusplus
extern "C" {
#endif


typedef struct encode_handle_ctx {
	void (*flush)(struct encode_handle_ctx *ctx);
	void (*close)(struct encode_handle_ctx *ctx);
	void (*write)(struct encode_handle_ctx *ctx, const unsigned char* data, int length);
	void *priv;
} encode_ctx;

int decode_amrnb(const char *amrpath, encode_ctx *ctx);

#ifdef __cplusplus
}
#endif

#endif

