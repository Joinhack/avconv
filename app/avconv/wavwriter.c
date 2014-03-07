#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "wavwriter.h"
#include "write_cache.h"

struct wav_writer {
	FILE *wav;
	int data_length;
	int sample_rate;
	int bits_per_sample;
	int channels;
	write_cache cache;
};

static void write_string(struct wav_writer* ww, const char *str) {
	fputc(str[0], ww->wav);
	fputc(str[1], ww->wav);
	fputc(str[2], ww->wav);
	fputc(str[3], ww->wav);
}

static void write_int32(struct wav_writer* ww, int value) {
	fputc((value >>  0) & 0xff, ww->wav);
	fputc((value >>  8) & 0xff, ww->wav);
	fputc((value >> 16) & 0xff, ww->wav);
	fputc((value >> 24) & 0xff, ww->wav);
}

static void write_int16(struct wav_writer* ww, int value) {
	fputc((value >> 0) & 0xff, ww->wav);
	fputc((value >> 8) & 0xff, ww->wav);
}

static void write_header(struct wav_writer* ww, int length) {
	int bytes_per_frame, bytes_per_sec;
	write_string(ww, "RIFF");
	write_int32(ww, 4 + 8 + 16 + 8 + length);
	write_string(ww, "WAVE");

	write_string(ww, "fmt ");
	write_int32(ww, 16);

	bytes_per_frame = ww->bits_per_sample/8*ww->channels;
	bytes_per_sec   = bytes_per_frame*ww->sample_rate;
	write_int16(ww, 1);                   // Format
	write_int16(ww, ww->channels);        // Channels
	write_int32(ww, ww->sample_rate);     // Samplerate
	write_int32(ww, bytes_per_sec);       // Bytes per sec
	write_int16(ww, bytes_per_frame);     // Bytes per frame
	write_int16(ww, ww->bits_per_sample); // Bits per sample

	write_string(ww, "data");
	write_int32(ww, length);
}

void* wav_write_open(const char *filename, int sample_rate, int bits_per_sample, int channels) {
	struct wav_writer* ww = (struct wav_writer*) malloc(sizeof(*ww));
	memset(ww, 0, sizeof(*ww));
	ww->wav = fopen(filename, "wb");
	if (ww->wav == NULL) {
		free(ww);
		return NULL;
	}
	ww->data_length = 0;
	ww->sample_rate = sample_rate;
	ww->bits_per_sample = bits_per_sample;
	ww->channels = channels;
	write_cache_init(&ww->cache, ww->wav, 1024*12);
	return ww;
}

void wav_write_close(void* obj) {
	struct wav_writer* ww = (struct wav_writer*) obj;
	write_cache_close(&ww->cache);
	if (ww->wav == NULL) {
		free(ww);
		return;
	}
	fclose(ww->wav);
	free(ww);
}

void wav_write_flush(void* obj) {
	struct wav_writer* ww = (struct wav_writer*) obj;
	if (ww->wav == NULL)
		return;
	write_cache_flush(&ww->cache);
	fseek(ww->wav, 0, SEEK_SET);
	write_header(ww, ww->data_length);
}

void wav_write_data(void* obj, const unsigned char* data, int length) {
	int left;
	struct wav_writer* ww = (struct wav_writer*) obj;
	if (ww->wav == NULL)
		return;
	write_cache_write(&ww->cache, data, length);
	ww->data_length += length;
}

