package imgconv

import (
	"unsafe"
	"errors"
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

func Resize(fpath, tpath string, typ, cols, rows int) error {
	var mw *C.MagickWand = C.NewMagickWand()
	var cfpath = C.CString(fpath)
	var ctpath = C.CString(tpath)
	var etype C.ExceptionType
	defer func() {
		C.free(unsafe.Pointer(cfpath))
		C.free(unsafe.Pointer(ctpath))
	}()
	defer func() {
		C.ClearMagickWand(mw)
		C.DestroyMagickWand(mw)
	}()
	if C.MagickReadImage(mw, cfpath) == C.MagickFalse {
		goto ERR	
	}
	
	if C.scale(mw, C.int(typ), C.int(cols), C.int(rows)) == C.MagickFalse {
		goto ERR
	}
	if C.MagickWriteImages(mw, ctpath, C.MagickTrue) == C.MagickFalse {
		goto ERR
	}
	return nil
ERR:
	etype = C.MagickGetExceptionType(mw)
	return errors.New(C.GoString(C.MagickGetException(mw, &etype)))
}
