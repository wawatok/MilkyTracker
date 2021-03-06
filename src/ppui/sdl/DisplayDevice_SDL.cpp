/*
 *  ppui/sdl/DisplayDevice_SDL.cpp
 *
 *  Copyright 2009 Peter Barth, Christopher O'Neill, Dale Whinham
 *
 *  This file is part of Milkytracker.
 *
 *  Milkytracker is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Milkytracker is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Milkytracker.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "DisplayDevice_SDL.h"
#include "Graphics.h"

#if SDL_VERSION_ATLEAST(2, 0, 0)
SDL_Window* PPDisplayDevice::CreateWindow(pp_int32& w, pp_int32& h, pp_int32& bpp, Uint32 flags)
{
	char rendername[256] = { 0 };
	PFNGLGETSTRINGPROC glGetStringAPI = NULL;

	for (int it = 0; it < SDL_GetNumRenderDrivers(); it++)
	{
		SDL_RendererInfo info;
		SDL_GetRenderDriverInfo(it,&info);

		strncat(rendername, info.name, 9);
		strncat(rendername, " ", 1);

		if (strncmp("opengles2", info.name, 9) == 0)
		{
			drv_index = it;
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
			SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		}
	}

	// Create SDL window
	SDL_Window* theWindow = SDL_CreateWindow("MilkyTracker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | flags);

	if (theWindow == NULL)
	{
		fprintf(stderr, "SDL: SDL_CreateWindow (width: %d, height: %d) failed: %s\n", w, h, SDL_GetError());
		fprintf(stderr, "Retrying with default size...");

		w = getDefaultWidth();
		h = getDefaultHeight();
		
		theWindow = SDL_CreateWindow("MilkyTracker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | flags);
		
		if (theWindow == NULL)
		{
			fprintf(stderr, "SDL: SDL_CreateWindow (width: %d, height: %d) failed: %s\n", w, h, SDL_GetError());
			fprintf(stderr, "Giving up.\n");
			return NULL;
		}
	}

	SDL_GLContext ctx = SDL_GL_CreateContext(theWindow);
	SDL_GL_MakeCurrent(theWindow, ctx);
	
	glGetStringAPI = (PFNGLGETSTRINGPROC)SDL_GL_GetProcAddress("glGetString");

	fprintf(stdout, "Available Renderers: %s\n", rendername);
	if (glGetStringAPI)
	{
		fprintf(stdout, "Vendor     : %s\n", glGetStringAPI(GL_VENDOR));
		fprintf(stdout, "Renderer   : %s\n", glGetStringAPI(GL_RENDERER));
		fprintf(stdout, "Version    : %s\n", glGetStringAPI(GL_VERSION));
#ifdef DEBUG
		fprintf(stdout, "Extensions : %s\n", glGetStringAPI(GL_EXTENSIONS));
#endif
	}
	// Prevent window from being resized below minimum
	SDL_SetWindowMinimumSize(theWindow, w, h);
	fprintf(stderr, "SDL: Minimum window size set to %dx%d.\n", w, h);

	return theWindow;
}
#else
SDL_Surface* PPDisplayDevice::CreateScreen(pp_int32& w, pp_int32& h, pp_int32& bpp, Uint32 flags)
{
	SDL_Surface *screen;
	
	/* Set the video mode */
	screen = SDL_SetVideoMode(w, h, bpp, flags);
	if (screen == NULL) 
	{
		fprintf(stderr, "Couldn't set display mode: %s\n", SDL_GetError());
		fprintf(stderr, "Retrying with default size...");

		w = getDefaultWidth();
		h = getDefaultHeight();
		
		screen = SDL_SetVideoMode(w, h, bpp, flags);
		
		if (screen == NULL) 
		{
			fprintf(stderr, "Couldn't set display mode: %s\n", SDL_GetError());
			fprintf(stderr, "Giving up.");
			
			return NULL;
		}
	}

	return screen;
}
#endif

PPDisplayDevice::PPDisplayDevice(
#if !SDL_VERSION_ATLEAST(2, 0, 0)
								 SDL_Surface*& screen, 
#endif
								 pp_int32 width, 
								 pp_int32 height, 
								 pp_int32 scaleFactor,
								 pp_int32 bpp,
								 bool fullScreen, 
								 Orientations theOrientation/* = ORIENTATION_NORMAL*/) :
	PPDisplayDeviceBase(width, height, scaleFactor),
	realWidth(width), realHeight(height),
	orientation(theOrientation)
{
	adjust(realWidth, realHeight);

	bFullScreen = fullScreen;

#if SDL_VERSION_ATLEAST(2, 0, 0)
	drv_index = -1;
#endif
	initMousePointers();
}

PPDisplayDevice::~PPDisplayDevice()
{	
	delete currentGraphics;
}

void PPDisplayDevice::adjust(pp_int32& x, pp_int32& y)
{
	switch (orientation)
	{
		case ORIENTATION_NORMAL:
			break;
			
		case ORIENTATION_ROTATE90CCW:
		case ORIENTATION_ROTATE90CW:
		{
			pp_int32 h = x;
			x = y;
			y = h;
			break;
		}
	}	
	
	x *= scaleFactor;
	y *= scaleFactor;
}

void PPDisplayDevice::transform(pp_int32& x, pp_int32& y)
{
	pp_int32 h;

	switch (orientation)
	{
		case ORIENTATION_NORMAL:
			break;
			
		case ORIENTATION_ROTATE90CW:
			h = x;
			x = y;
			y = realWidth - 1 - h;
			break;
			
		case ORIENTATION_ROTATE90CCW:
			h = x;
			x = realHeight - 1 - y;
			y = h;
			break;
	}
}

void PPDisplayDevice::transformInverse(pp_int32& x, pp_int32& y)
{
	pp_int32 h;

	switch (orientation)
	{
		case ORIENTATION_NORMAL:
			break;
			
		case ORIENTATION_ROTATE90CW:
			h = x;
			x = realWidth - y;
			y = h;
			break;
			
		case ORIENTATION_ROTATE90CCW:
			h = x;
			x = y;
			y = realHeight - h;
			break;
	}
}

void PPDisplayDevice::transformInverse(PPRect& r)
{
	transformInverse((pp_int32&)r.x1, (pp_int32&)r.y1);	
	transformInverse((pp_int32&)r.x2, (pp_int32&)r.y2);	

	pp_int32 h;
	if (r.x2 < r.x1)
	{
		h = r.x1; r.x1 = r.x2; r.x2 = h;
	}	
	if (r.y2 < r.y1)
	{
		h = r.y1; r.y1 = r.y2; r.y2 = h;
	}	
}

void PPDisplayDevice::setTitle(const PPSystemString& title)
{
#if SDL_VERSION_ATLEAST(2, 0, 0)
	SDL_SetWindowTitle(theWindow, title);
#else
	SDL_WM_SetCaption(title, "MilkyTracker");
#endif
}

#if !SDL_VERSION_ATLEAST(2, 0, 0)
void PPDisplayDevice::setSize(const PPSize& size)	
{	
	theSurface = SDL_SetVideoMode(size.width, size.height, theSurface->format->BitsPerPixel, theSurface->flags);	
}
#endif

bool PPDisplayDevice::goFullScreen(bool b)
{
	// In X11, this will make MilkyTracker go fullscreen at the selected
	// resolution.
#if SDL_VERSION_ATLEAST(2, 0, 0)
	if (SDL_SetWindowFullscreen(theWindow, (!b)?SDL_FALSE:SDL_WINDOW_FULLSCREEN_DESKTOP) == 0)
	{
		bFullScreen = b;
		return true;
	}
#else
	SDL_Surface* screen = SDL_GetVideoSurface();
	if (SDL_WM_ToggleFullScreen(screen)) 
	{
		bFullScreen = !bFullScreen;
		return true;
	}
#endif
	
	return false;
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
SDL_Window* PPDisplayDevice::getWindow() {
	return theWindow;
}
#endif

// Defined in main.cpp
void exitSDLEventLoop(bool serializedEventInvoked = true);

#if SDL_VERSION_ATLEAST(2, 0, 0)
PPSize PPDisplayDevice::getDisplayResolution() const {
	// Find the monitor MilkyTracker is being displayed on
	int currentDisplay = SDL_GetWindowDisplayIndex(theWindow);

	// Structure to hold the display resolution
	SDL_DisplayMode displayMode;

	// If this fails, return -1 dimensions (makes MilkyTracker display an error)
	if (SDL_GetDesktopDisplayMode(currentDisplay, &displayMode) != 0)
		return PPSize(-1, -1);

	// Return the desktop size
	return PPSize(displayMode.w, displayMode.h);
}
#endif

void PPDisplayDevice::shutDown()
{
	exitSDLEventLoop();
}

void PPDisplayDevice::setMouseCursor(MouseCursorTypes type)
{
	currentCursorType = type;
	
	switch (type)
	{
		case MouseCursorTypeStandard:
			SDL_SetCursor(cursorStandard);
			break;
#if SDL_VERSION_ATLEAST(2, 0, 0)			
		case MouseCursorTypeResizeLeft:
		case MouseCursorTypeResizeRight:
			SDL_SetCursor(cursorResizeHoriz);
			break;
#else
		case MouseCursorTypeResizeLeft:
			SDL_SetCursor(cursorResizeLeft);
			break;

		case MouseCursorTypeResizeRight:
			SDL_SetCursor(cursorResizeRight);
			break;
#endif	
		case MouseCursorTypeHand:
			SDL_SetCursor(cursorHand);
			break;

		case MouseCursorTypeWait:
			SDL_SetCursor(cursorEggtimer);
			break;
	}
}

void PPDisplayDevice::signalWaitState(bool b, const PPColor& color)
{
	setMouseCursor(b ? MouseCursorTypeWait : MouseCursorTypeStandard);
}

#if SDL_VERSION_ATLEAST(2, 0, 0)
void PPDisplayDevice::initMousePointers()
{
	cursorStandard = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	cursorResizeHoriz = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	cursorEggtimer = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	cursorHand = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
}
#else
// Mouse pointer data
Uint8 PPDisplayDevice::resizeLeft_data[] = { 0, 0, 96, 0, 97, 128, 99, 0, 102, 0, 108, 0, 120, 0, 127, 254, 120, 0, 124, 0, 102, 0, 99, 0, 97, 128, 96, 0, 0, 0, 0, 0 };
Uint8 PPDisplayDevice::resizeLeft_mask[] = { 240, 0, 241, 128, 243, 192, 247, 128, 255, 0, 254, 0, 255, 255, 255, 255, 255, 255, 254, 0, 255, 0, 247, 128, 243, 192, 241, 128, 240, 0, 0, 0 };
Uint8 PPDisplayDevice::resizeRight_data[] = { 0, 0, 0, 6, 1, 134, 0, 198, 0, 102, 0, 54, 0, 30, 127, 254, 0, 30, 0, 62, 0, 102, 0, 198, 1, 134, 0, 6, 0, 0, 0, 0 };
Uint8 PPDisplayDevice::resizeRight_mask[] = { 0, 15, 1, 143, 3, 207, 1, 239, 0, 255, 0, 127, 255, 255, 255, 255, 255, 255, 0, 127, 0, 255, 1, 239, 3, 207, 1, 143, 0, 15, 0, 0, };
Uint8 PPDisplayDevice::eggtimer_data[] = { 0, 0, 127, 192, 32, 128, 32, 128, 17, 0, 17, 0, 10, 0, 4, 0, 4, 0, 10, 0, 17, 0, 17, 0, 32, 128, 32, 128, 127, 192, 0, 0 };
Uint8 PPDisplayDevice::eggtimer_mask[] = { 255, 224, 255, 224, 127, 192, 127, 192, 63, 128, 63, 128, 31, 0, 14, 0, 14, 0, 31, 0, 63, 128, 63, 128, 127, 192, 127, 192, 255, 224, 255, 224 };
Uint8 PPDisplayDevice::hand_data[] = {54, 192, 91, 64, 146, 64, 146, 112, 146, 104, 146, 104, 128, 40, 128, 40, 128, 8, 128, 8, 128, 16, 64, 16, 64, 32, 32, 32, 31, 192, 0, 0, };
Uint8 PPDisplayDevice::hand_mask[] = {54, 192, 127, 192, 255, 192, 255, 240, 255, 248, 255, 248, 255, 248, 255, 248, 255, 248, 255, 248, 255, 240, 127, 240, 127, 224, 63, 224, 31, 192, 0, 0, };

void PPDisplayDevice::initMousePointers()
{
	cursorResizeLeft = SDL_CreateCursor(resizeLeft_data, resizeLeft_mask, 16, 16, 2, 7);
	cursorResizeRight = SDL_CreateCursor(resizeRight_data, resizeRight_mask, 16, 16, 13, 7);
	cursorEggtimer = SDL_CreateCursor(eggtimer_data, eggtimer_mask, 16, 16, 5, 7);
	cursorHand = SDL_CreateCursor(hand_data, hand_mask, 16, 16, 5, 5);

	// The current cursor is used as the standard cursor;
	// This might cause problems if the system if displaying some other cursor at
	// the time, or it might not. It depends.
	cursorStandard = SDL_GetCursor();
}
#endif
