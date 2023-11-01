package main

/*
#cgo linux pkg-config: Qt5Widgets Qt5Core Qt5WebEngineWidgets
#cgo CXXFLAGS: -std=c++17
#cgo LDFLAGS: -L/usr/local/lib -lstdc++

#include "lib.hpp"

*/
import "C"

import (
	"os"
	"time"
)

var exitCh = make(chan int)

//export appExited
func appExited(returnCode C.int) {
	exitCh <- int(returnCode)
}

func main() {
	app := C.Application_new(0, nil)
	win := C.Window_new(app.qt_application)
	C.Window_set_title(win, C.CString("Test Application"))
	C.Window_resize(win, C.int(800), C.int(600))
	layout := C.WindowLayout_new(win)
	webEngine := C.WebEngineView_new(win, layout)

	time.Sleep(3 * time.Second)
	C.WebEngineView_load_url(webEngine, C.CString("https://wails.io/"))

	os.Exit(<-exitCh)
}
