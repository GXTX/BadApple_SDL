package main

import (
	"fmt"
	"github.com/veandco/go-sdl2/sdl"
	"github.com/veandco/go-sdl2/ttf"
	"io/ioutil"
	"os"
)

const (
	Width           = 640
	Height          = 480
	FramesPerSecond = 30
	LineEnd         = 1
	FrameLines      = 60
	FrameLineSize   = 161
)

type frontEnd struct {
	window        *sdl.Window
	windowSurface *sdl.Surface
	fpsSurface    *sdl.Surface
	event         sdl.Event
}

type glyph struct {
	str     string
	surface *sdl.Surface
	metrics *ttf.GlyphMetrics
}

type timer struct {
	time     uint32
	lastTime uint32
}

func main() {
	if err := sdl.Init(sdl.INIT_VIDEO); err != nil {
		panic(err)
	}

	if err := ttf.Init(); err != nil {
		panic(err)
	}

	front := frontEnd{}
	front.window, _ = sdl.CreateWindow("Bad Apple!!", sdl.WINDOWPOS_UNDEFINED,
		sdl.WINDOWPOS_UNDEFINED, Width, Height, sdl.WINDOW_SHOWN)
	front.windowSurface, _ = front.window.GetSurface()

	font, _ := ttf.OpenFont("resource/consola.ttf", 7)
	color := sdl.Color{0x80, 0x80, 0x80, 0xFF}
	glyphs := [128 - 32]glyph{}
	// Pre-render some text glyphs, saving a little memory by not reading in control characters.
	// TODO: Why am I getting off-by-one?
	for i := 0; i < (128 - 32); i++ {
		glyphs[i].str = string(i + 31)
		glyphs[i].surface, _ = font.RenderUTF8Solid(string(rune(i+31)), color)
		glyphs[i].metrics, _ = font.GlyphMetrics(rune(i + 31))
	}

	// Read in our 'video'.
	file, _ := os.Open("resource/AscPic.txt")
	videoSize, _ := file.Stat()
	video, _ := ioutil.ReadFile("resource/AscPic.txt")
	ptrLocation := int32(0)

	currentFrame := int64(0)
	totalFrames := videoSize.Size() / (FrameLineSize * FrameLines)

	//
	frame := 0
	updatefps := timer{}
	fps := timer{}

	for {
		front.windowSurface.FillRect(nil, 0)

		if currentFrame < totalFrames {
			memoryToSurface(front.windowSurface, video, &ptrLocation, glyphs)
			displayFrames(&front, font, &updatefps, &frame)

			if (sdl.GetTicks() - fps.lastTime) < (1000 / FramesPerSecond) {
				sdl.Delay((1000 / FramesPerSecond) - (sdl.GetTicks() - fps.lastTime))
			}

			fps.lastTime = sdl.GetTicks()
			front.window.UpdateSurface()
			frame++
			currentFrame++
		} else {
			return
		}

		// TODO: Why does sdl.PollEvent() get called twice in the example?
		for front.event = sdl.PollEvent(); front.event != nil; front.event = sdl.PollEvent() {
			switch front.event.(type) {
			case *sdl.QuitEvent:
				fmt.Print("Quitting.\n")
				return
			}
		}
	}
}

func memoryToSurface(surface *sdl.Surface, video []byte, videoLocation *int32, glyphs [128 - 32]glyph) {
	for y := 0; y < FrameLines; y++ {
		for x := 0; x < FrameLineSize-LineEnd; x++ {
			location := sdl.Rect{int32(x) * int32(glyphs[video[int32(x)+*videoLocation]-32].metrics.Advance),
				int32(y) * glyphs[video[int32(x)+*videoLocation]-32].surface.H, 0, 0}
			glyphs[video[int32(x)+*videoLocation]-31].surface.Blit(nil, surface, &location)
		}
		*videoLocation += FrameLineSize
	}
	*videoLocation += LineEnd
}

func displayFrames(front *frontEnd, font *ttf.Font, updatetime *timer, Frame *int) {
	color := sdl.Color{0xFF, 0x00, 0x00, 0xFF}

	if updatetime.lastTime == 0 {
		updatetime.lastTime = sdl.GetTicks()
		return
	}

	temptime := sdl.GetTicks()
	updatetime.time += temptime - updatetime.lastTime
	updatetime.lastTime = temptime

	if updatetime.time >= 1000 {
		updatetime.time = 0
		fpsch := fmt.Sprintf("FPS: %d", *Frame)
		front.fpsSurface, _ = font.RenderUTF8Solid(fpsch, color)
		*Frame = 0
	}

	if front.fpsSurface == nil {
		front.fpsSurface, _ = font.RenderUTF8Solid("FPS: 0", color)
	}

	location := sdl.Rect{Width - front.fpsSurface.W, 0, 0, 0}
	front.fpsSurface.Blit(nil, front.windowSurface, &location)
}
