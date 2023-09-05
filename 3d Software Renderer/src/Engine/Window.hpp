//Window.hpp

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <windows.h>
#include <iostream>

#include "Pixel.hpp"

//declare class
class Window {
	//public access specifier
	public:
		//constructor
		Window(const char * title, unsigned int width, unsigned int height);
	
		//destructor
		~Window();
		
		//handle events
		void handleEvents();
	
		//clear screen
		void clearScreen();
		void clearScreen(uint8_t red, uint8_t green, uint8_t blue);
		
		//draw pixel
		void drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
		void drawPixel(int x, int y, double depth, uint8_t red, uint8_t green, uint8_t blue);
		
		//swap buffers
		void swapBuffers();
	
		//change resolution
		void changeResolution(int width, int height);
	
		//set fullscreen
		void setFullscreen(bool fullscreen);
	
		//end program
		void endProgram();
		
		//updatate delta time
		void updateDeltaTime();
		
		//getters
		unsigned int getWidth();
		unsigned int getHeight();
		double getAspectRatio();
		bool getRunning();
		double getDeltaTime();
		
	//private access specifier
	private:
		//data members
		unsigned int width;
		unsigned int height;
		unsigned int clientWidth;
		unsigned int clientHeight;
		int scale;
		HWND windowHandle;
		HDC deviceContext;
		BITMAPINFO bitmapInfo;
		Pixel * renderBuffer = nullptr;
		double * depthBuffer = nullptr;
		bool running;
		bool fullscreen;
		DWORD windowStyle;
		LARGE_INTEGER startTime;
		LARGE_INTEGER endTime;
		double deltaTime;
		
		//private methods
		void computeMaximumScale();
};

#endif