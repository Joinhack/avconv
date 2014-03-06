package avconv

/*
#cgo LDFLAGS: -lopencore-amrnb -lmp3lame

int amrnb2mp3(const char *amrpath, const char *wavpath);
*/
import "C"

func init() {
}

func Amrnb2Mp3(s, d string) int {
	return int(C.amrnb2mp3(C.CString(s), C.CString(d)))
}
