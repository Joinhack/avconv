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
	return convInner(url, app, "wav")
}

var convter = map[string]func(string, string) int{
	"mp3": conv.Amrnb2Mp3,
	"wav": conv.Amrnb2Wav,
}

func convInner(url string, app *peony.App, f string) peony.Renderer {
	if url == "" {
		return peony.NotFound("No such file")
	}
	savePath := app.GetStringConfig("savepath", "/tmp")

	hash := md5.New()
	io.WriteString(hash, url)
	sum := hash.Sum(nil)
	destPath := path.Join(savePath, fmt.Sprintf("%x.%s", sum, f))

	if info, err := os.Stat(destPath); err == nil && !info.IsDir() {
		return peony.RenderFile(destPath)
	}
	transport := http.Transport{
		Dial: dialTimeout,
	}
	client := http.Client{
		Transport: &transport,
	}
	workPath := app.GetStringConfig("workpath", "/tmp")
	resp, err := client.Get(url)
	if err != nil || resp.StatusCode != http.StatusOK {
		return peony.NotFound("Get file from [%s] error.", url)
	}
	amrPath := path.Join(workPath, fmt.Sprintf("%x.amr", sum))
	amrFile, err := os.OpenFile(amrPath, os.O_WRONLY|os.O_CREATE, 0666)
	if err != nil {
		return peony.NotFound("Save file[%s] error, detial: %s ", url, err.Error())
	}
	defer resp.Body.Close()
	defer func() {
		amrFile.Close()
		os.Remove(amrPath)
	}()
	io.Copy(amrFile, resp.Body)
	convter[f](amrPath, destPath)
	return peony.RenderFile(destPath)
}

//@Mapper("/amrnb2mp3")
func Amrnb2Mp3(url string, app *peony.App) peony.Renderer {
	return convInner(url, app, "mp3")
}
