
#ifndef WAVWRITER_H
#define WAVWRITER_H

#ifdef __cplusplus
extern "C" {
#endif

void* wav_write_open(const char *filename, int sample_rate, int bits_per_sample, int channels);
void wav_write_close(void* obj);

void wav_write_data(void* obj, const unsigned char* data, int length);

void wav_write_flush(void* obj);

#ifdef __cplusplus
}
#endif

#endif

