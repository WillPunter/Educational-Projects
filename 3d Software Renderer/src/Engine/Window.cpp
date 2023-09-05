//Window.cpp

//include headers
#include "Window.hpp"
#include <winuser.h>
#include <cstring>

//window procedure
LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam){
	//check message type
	switch(msg){
		//destroy
		case WM_DESTROY:{
			//add quit message to message queue
			PostQuitMessage(0); //argument is WPARAM of WM_QUIT message
			break;
		};
		
		//override size message to prevent window resizing
		case WM_SIZE:{
			return 1;
		};
		
		//note that the WM_QUIT message is not associated with a window and therefore never runs through a window's window procedure
	};
	
	//handle default window procedure
	return DefWindowProc(windowHandle, msg, wParam, lParam);
};

//constructor
Window::Window(const char * title, unsigned int width, unsigned int height){
	//set dpi process aware - this tells Windows not to scale out program automatically (according to the zoom in the display settings)
	//i.e. it tells Windows to use device coordinates (actual monitor pixels) for our window sizes (scaled up monitor pixels), not virtual coordinates
	SetProcessDPIAware();
	
	//set width and height
	this->width = width;
	this->height = height;
	
	//create window class and zero memory
	WNDCLASSEX windowClass = {0};
	
	//set necessary attributes of window class
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpszClassName = "MAIN_GAME_WINDOW_CLASS";
	windowClass.lpfnWndProc = windowProcedure;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(0, 0, 0));
	
	//register window class - this sends the window class to the OS, which stores a table of window classes
	RegisterClassEx(&windowClass);
	
	//set window style
	this->windowStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE; //overlappedwindow without the resize box around the frame
	
	//set client dimensiosn and scale
	this->clientWidth = this->width;
	this->clientHeight = this->height;
	this->scale = 1;
	
	//set fullscreen to false
	this->fullscreen = false;
	
	//calculate window frame size from client size
	RECT windowSize = {0, 0, (int) width, (int) height};
	AdjustWindowRectEx(&windowSize, this->windowStyle, false, 0);
	
	//create window
	this->windowHandle = CreateWindowEx(
		0, //extended window style (using WS_EX_OVERLAPPEDWINDOW will interfere with the fullscreen mode)
		windowClass.lpszClassName, //window class name - used by OS to identify the window class being referenced
		title, //window title
		this->windowStyle, //default window style / behaviour
		CW_USEDEFAULT, CW_USEDEFAULT, //use default x and y coordinates
		windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, //width and height
		NULL, //no parent window
		NULL, //no menu bar
		NULL, //use default application handle (hInstance)
		NULL //no LPARAM (no extra data to be added)
	);
	
	//check if window created
	if(this->windowHandle){
		//set running
		this->running = true;
		
		//show window
		ShowWindow(this->windowHandle, SW_SHOW);
		
		//get device context
		this->deviceContext = GetDC(this->windowHandle);
		
		//fill out metadata structure for render buffer (the render buffer is effectively a bitmap image)
		this->bitmapInfo = {0};
		this->bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		this->bitmapInfo.bmiHeader.biWidth = this->width;
		this->bitmapInfo.bmiHeader.biHeight = this->height;
		this->bitmapInfo.bmiHeader.biPlanes = 1;
		this->bitmapInfo.bmiHeader.biBitCount = sizeof(Pixel) * 8;
		this->bitmapInfo.bmiHeader.biCompression = BI_RGB;
		this->bitmapInfo.bmiHeader.biSizeImage = this->width * this->height * sizeof(Pixel);
		
		//create render buffer
		this->renderBuffer = new Pixel[this->width * this->height];
		
		//create depth buffer
		this->depthBuffer = new double[this->width * this->height];
	} else {
		//output error message box
		MessageBox(NULL, "Error - could not create window!", "Window Error", MB_OK | MB_ICONERROR);
	};
};

//destructor
Window::~Window(){
	//delete render buffer
	delete[] this->renderBuffer;
};

//handle events
void Window::handleEvents(){
	//check message queue for events
	MSG message;
	
	//peek message retrieves a message (if present) from the front of the message queue
	//pointer to a message struct, the window handle to retrieve messages from (NULL indicates all messages), the minimum message code (0 indicates no minimum), the maximum message code (0 indicates no maximum - note that this parameter is used for message filtering), what to do with the messages (PM_REMOVE means that messages are removed after processing)
	while(PeekMessage(&message, NULL, 0, 0, PM_REMOVE)){ 
		//translate message - convert key codes to characters for text events
		TranslateMessage(&message);
		
		//dispatch message - send message to window procedure (i.e. run window procedure for given message)
		DispatchMessage(&message);
		
		if(message.message == WM_QUIT){
			//set running to false
			this->running = false;
		};
	};
};

//clear screen
void Window::clearScreen(){
	//set all pixels to black
	std::memset(this->renderBuffer, 0x00, this->width * this->height * sizeof(Pixel));

	//clear depth buffer
	for(int i = 0; i < this->width * this->height; i++){
		this->depthBuffer[i] = 9999999;
	};
};

void Window::clearScreen(uint8_t red, uint8_t green, uint8_t blue){
	//set all pixels to colour
	for(int i = 0; i < this->width * this->height; i++){
		this->renderBuffer[i].red = red;
		this->renderBuffer[i].green = green;
		this->renderBuffer[i].blue = blue;
	};
	
	//clear depth buffer
	for(int i = 0; i < this->width * this->height; i++){
		this->depthBuffer[i] = 9999999;
	};
};

//draw pixel without taking depth into account
void Window::drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue){
	//top left is (0, 0) as is traditional in graphics
	if(x >= 0 && x < this->width && y >= 0 && y < this->height){
		//calculate pixel index
		int pixelIndex = y * this->width + x;//(this->height - 1 - y) * this->width + x;
	
		//set pixel colour
		this->renderBuffer[pixelIndex].red = red;
		this->renderBuffer[pixelIndex].green = green;
		this->renderBuffer[pixelIndex].blue = blue;
	};
};

//draw pixel with depth
void Window::drawPixel(int x, int y, double depth, uint8_t red, uint8_t green, uint8_t blue){
	//calculate pixel index
	int pixelIndex = y * this->width + x;//(this->height - 1 - y) * this->width + x;
	
	//top left is (0, 0) as is traditional in graphics
	if(x >= 0 && x < this->width && y >= 0 && y < this->height && depth < this->depthBuffer[pixelIndex]){
		//set pixel colour
		this->renderBuffer[pixelIndex].red = red;
		this->renderBuffer[pixelIndex].green = green;
		this->renderBuffer[pixelIndex].blue = blue;
		
		//update depth buffer
		this->depthBuffer[pixelIndex] = depth;
	};
};
	
//swap buffers
void Window::swapBuffers(){
	//send pixel buffer to device context with StretchDIBits
	StretchDIBits(this->deviceContext, this->clientWidth / 2 - this->width * this->scale / 2, this->clientHeight / 2 - this->height * this->scale / 2, this->width * this->scale, this->height * this->scale, 0, 0, this->width, this->height, this->renderBuffer, &this->bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
};

//change resolution
void Window::changeResolution(int width, int height){
	//change width and height
	this->width = width;
	this->height = height;
	
	//change bitmap info structure
	this->bitmapInfo.bmiHeader.biWidth = width;
	this->bitmapInfo.bmiHeader.biHeight = height;
	this->bitmapInfo.bmiHeader.biSizeImage = width * height * sizeof(Pixel);
	
	//delete screen buffer
	if(this->renderBuffer){
		delete[] this->renderBuffer;
	};
	
	//delete depth buffer
	if(this->depthBuffer){
		delete[] this->depthBuffer;
	};
	
	//allocate new render buffer
	this->renderBuffer = new Pixel[width * height];
	
	//allocate depth buffer
	this->depthBuffer = new double[width * height];
	
	//check if in fullscreen
	if(!this->fullscreen){
		//set window size
		RECT windowSize = {0, 0, width, height};
		AdjustWindowRectEx(&windowSize, this->windowStyle, false, WS_EX_OVERLAPPEDWINDOW);
	
		//set window size
		//window handle, z order (layer - set to top), x (no move flag prevents change to this), y (no move flag prevents change to this), width, height, flags (SWP_NOMOVE keeps the position of the old window) 
		SetWindowPos(this->windowHandle, HWND_TOP, 0, 0, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, SWP_NOMOVE);
	
		//set this client size
		this->clientWidth = this->width;
		this->clientHeight = this->height;
	};
	
	//calculate scale
	this->computeMaximumScale();
};

//go fullscreen
void Window::setFullscreen(bool fullscreen){
	//set fullscreen
	this->fullscreen = fullscreen;
	
	//set monitor info metadata structure
	MONITORINFO monitorInfo = {sizeof(MONITORINFO)};
	
	//get monitor metadata
	//this is returning an unusual result, perhaps it has something to do with the virtual screen? https://learn.microsoft.com/en-us/windows/win32/gdi/the-virtual-screen
	GetMonitorInfo(MonitorFromWindow(this->windowHandle, MONITOR_DEFAULTTOPRIMARY), &monitorInfo);
	
	int monitorWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
	int monitorHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
	
	//check if fullscreen is to be enabled or disabled
	if(fullscreen){
		//set client width and height
		this->clientWidth = monitorWidth;
		this->clientHeight = monitorHeight;
		
		//set window style to WM_VISIBLE (remove WS_OVERLAPPEDWINDOW)
		SetWindowLongPtr(this->windowHandle, GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW);
		
		//set window to occupy the entire screen
		SetWindowPos(this->windowHandle, HWND_TOPMOST, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, this->clientWidth, this->clientHeight, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	} else {
		//switch out of fullscreen
		SetWindowLongPtr(this->windowHandle, GWL_STYLE, this->windowStyle);
		
		//set client size
		this->clientWidth = this->width;
		this->clientHeight = this->height;
		
		//adjust window rect
		RECT windowSize = {0, 0, (int) this->width, (int) this->height};
		AdjustWindowRectEx(&windowSize, this->windowStyle, false, 0);
		
		//set window to occupy an area equal to its render buffer
		//use HWND_BOTTOM for the layer to prevent the window from being stuck at the front
		SetWindowPos(this->windowHandle, HWND_BOTTOM, monitorInfo.rcMonitor.left + monitorWidth / 2 - (windowSize.right - windowSize.left) / 2, monitorInfo.rcMonitor.top + monitorHeight / 2 - (windowSize.bottom - windowSize.top) / 2, windowSize.right - windowSize.left, windowSize.bottom - windowSize.top, SWP_FRAMECHANGED); //removing the SWP_NOOWNERZORDER might fix the bug of always being the top layer?
		BringWindowToTop(this->windowHandle);
	};
	
	//calculate maximum scale
	this->computeMaximumScale();
};

//end program
void Window::endProgram(){
	this->running = false;
};

//compute maximum scale
void Window::computeMaximumScale(){
	//calculate maximum buffer scale
	this->scale = 0;
	while(this->width * this->scale <= this->clientWidth && this->height * this->scale <= this->clientHeight){
		this->scale += 1;
	};
	this->scale -= 1;
};

//calculate delta time
void Window::updateDeltaTime(){
	QueryPerformanceCounter(&this->endTime);
	
	LARGE_INTEGER clockFrequency;
	QueryPerformanceFrequency(&clockFrequency);
	
	this->deltaTime = (double) (this->endTime.QuadPart - this->startTime.QuadPart) / clockFrequency.QuadPart;
	
	this->startTime = this->endTime;
};

//getters
unsigned int Window::getWidth(){
	return this->width;
};

unsigned int Window::getHeight(){
	return this->height;
};

double Window::getAspectRatio(){
	return (double) this->width / this->height;
};

bool Window::getRunning(){
	return this->running;
};

double Window::getDeltaTime(){
	return this->deltaTime;
};