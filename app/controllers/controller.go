package controllers

import (
	"crypto/md5"
	"fmt"
	conv "github.com/joinhack/avconv/app/avconv"
	"github.com/joinhack/peony"
	"io"
	"net"
	"net/http"
	"os"
	"path"
	"time"
)

var timeout = time.Duration(2 * time.Second)

func dialTimeout(network, addr string) (net.Conn, error) {
	return net.DialTimeout(network, addr, timeout)
}

//@Mapper("/")
func Index(url string, app *peony.App) peony.Renderer {
	return peony.Render()
}

//@Mapper("/amrnb2wav")
func Amrnb2Wav(url string, app *peony.App) peony.Renderer {
	if url == "" {
		return peony.NotFound("No such file")
	}
	savePath := app.GetStringConfig("savepath", "/tmp")

	hash := md5.New()
	io.WriteString(hash, url)
	sum := hash.Sum(nil)
	wavPath := path.Join(savePath, fmt.Sprintf("%x.wav", sum))

	if info, err := os.Stat(wavPath); err == nil && !info.IsDir() {
		return peony.RenderFile(wavPath)
	}
	transport := http.Transport{
		Dial: dialTimeout,
	}
	client := http.Client{
		Transport: &transport,
	}
	workPath := app.GetStringConfig("workpath", "/tmp")
	resp, err := client.Get(url)
	if err != nil {
		return peony.NotFound("Get file[%s] error, detial: %s ", url, err.Error())
	}
	amrPath := path.Join(workPath, fmt.Sprintf("%x.amr", sum))
	amrFile, err := os.OpenFile(amrPath, os.O_WRONLY|os.O_CREATE, 0666)
	if err != nil {
		return peony.NotFound("Save file[%s] error, detial: %s ", url, err.Error())
	}
	defer resp.Body.Close()
	defer amrFile.Close()
	io.Copy(amrFile, resp.Body)
	conv.Amrnb2Wav(amrPath, wavPath)
	return peony.RenderFile(wavPath)
}
