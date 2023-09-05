//Pixel.hpp

#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <stdint.h>

//declare pixel class
class Pixel {
	//public
	public:
		//data members
		uint8_t blue;
		uint8_t green;
		uint8_t red;
	
		//remember that only data members are stored in individual objects
		//methods are stored inside the virtual table for the class, not in individual objects
		//constructors
		Pixel();
		Pixel(uint8_t red, uint8_t green, uint8_t blue);
};

#endif