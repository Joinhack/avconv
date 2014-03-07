package avconv

import (
	"unsafe"
)

/*
#cgo LDFLAGS: -lopencore-amrnb -lmp3lame
#include <stdlib.h>

int amrnb2mp3(const char *amrpath, const char *wavpath);
*/
import "C"

func init() {
}

func Amrnb2Mp3(s, d string) int {
	amrPath := C.CString(s)
	mp3Path := C.CString(d)
	defer func() {
		C.free(unsafe.Pointer(amrPath))
		C.free(unsafe.Pointer(mp3Path))
	}()
	return int(C.amrnb2mp3(amrPath, mp3Path))
}
