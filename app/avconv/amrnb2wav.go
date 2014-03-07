package avconv

import (
	"unsafe"
)

/*
#cgo LDFLAGS: -lopencore-amrnb
#include <stdlib.h>

int amrnb2wav(const char *amrpath, const char *wavpath);
*/
import "C"

func init() {
}

func Amrnb2Wav(s, d string) int {
	amrPath := C.CString(s)
	wavPath := C.CString(d)
	defer func() {
		C.free(unsafe.Pointer(amrPath))
		C.free(unsafe.Pointer(wavPath))
	}()
	return int(C.amrnb2wav(amrPath, wavPath))
}
