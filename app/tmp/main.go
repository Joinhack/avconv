
package main
import (
	"reflect"
	"time"
	"fmt"
	"flag"
	controllers0 "github.com/joinhack/avconv/app/controllers"
	peony "github.com/joinhack/peony"
)

var (
	_ = reflect.Ptr
	runMode    *string = flag.String("runMode", "", "Run mode.")
	bindAddr   *string = flag.String("bindAddr", ":8080", "By default, read from app.conf")
	importPath *string = flag.String("importPath", "", "Go ImportPath for the app.")
	srcPath    *string = flag.String("srcPath", "", "Path to the source root.")
	devMode    *bool    = flag.Bool("devMode", false, "Run mode")
)

func main() {
	flag.Parse()
	app := peony.NewApp(*srcPath, *importPath)
	app.BindAddr = *bindAddr
	if devMode != nil {
		app.DevMode = *devMode
	}
	svr := app.NewServer()
	svr.Init()

	svr.FuncMapper(`/`, []string{"GET", "POST", "PUT", "DELETE"}, 
		controllers0.Index, &peony.Action{
			Name: "Index",
			
			Args: []*peony.ArgType{ 
				
				&peony.ArgType{
					Name: "url", 
					Type: reflect.TypeOf((*string)(nil)).Elem(),
				},
			
				&peony.ArgType{
					Name: "app", 
					Type: reflect.TypeOf((*peony.App)(nil)),
				},
			}},
	)

	svr.FuncMapper(`/amrnb2wav`, []string{"GET", "POST", "PUT", "DELETE"}, 
		controllers0.Amrnb2Wav, &peony.Action{
			Name: "Amrnb2Wav",
			
			Args: []*peony.ArgType{ 
				
				&peony.ArgType{
					Name: "url", 
					Type: reflect.TypeOf((*string)(nil)).Elem(),
				},
			
				&peony.ArgType{
					Name: "app", 
					Type: reflect.TypeOf((*peony.App)(nil)),
				},
			}},
	)

	svr.FuncMapper(`/amrnb2mp3`, []string{"GET", "POST", "PUT", "DELETE"}, 
		controllers0.Amrnb2Mp3, &peony.Action{
			Name: "Amrnb2Mp3",
			
			Args: []*peony.ArgType{ 
				
				&peony.ArgType{
					Name: "url", 
					Type: reflect.TypeOf((*string)(nil)).Elem(),
				},
			
				&peony.ArgType{
					Name: "app", 
					Type: reflect.TypeOf((*peony.App)(nil)),
				},
			}},
	)


	svr.Router.Refresh()

	go func(){
		time.Sleep(1*time.Second)
		fmt.Println("Server is running, listening on", app.BindAddr)
	}()
	if err := <- svr.Run(); err != nil {
		panic(err)
	}
}
