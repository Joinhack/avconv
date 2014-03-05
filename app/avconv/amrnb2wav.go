package avconv

/*
#cgo LDFLAGS: -lopencore-amrnb

int amrnb2wav(const char *amrpath, const char *wavpath);
*/
import "C"

func init() {
}

func Amrnb2Wav(s, d string) int {
	return int(C.amrnb2wav(C.CString(s), C.CString(d)))
}
