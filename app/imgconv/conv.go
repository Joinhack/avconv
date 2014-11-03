package imgconv

import (
	"unsafe"
)

/*
#cgo pkg-config: MagickWand
#include <stdlib.h>
#include <wand/magick_wand.h>
int scale(MagickWand *mw, int type, int cols, int rows);
void init();
*/
import "C"

func init() {
	C.init()
}

func Scale(fpath, tpath string, typ, cols, rows int) error {
	var mw *C.MagickWand = C.NewMagickWand()
	var cfpath = C.CString(fpath)
	var ctpath = C.CString(tpath)
	defer func() {
		C.free(unsafe.Pointer(cfpath))
		C.free(unsafe.Pointer(ctpath))
	}()
	defer func() {
		C.ClearMagickWand(mw)
		C.DestroyMagickWand(mw)
	}()
	var status C.MagickBooleanType
	status = C.MagickReadImage(mw, cfpath)
	if status == C.MagickFalse {return}
	C.scale(mw, C.int(typ), C.int(cols), C.int(rows))
	C.MagickWriteImages(mw, ctpath, C.MagickTrue)
	return nil
}
