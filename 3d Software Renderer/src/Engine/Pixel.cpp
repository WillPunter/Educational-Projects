//Pixel.cpp

#include "Pixel.hpp"

//constructors
Pixel::Pixel(){
	this->red = 0;
	this->green = 0;
	this->blue = 0;
};

Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue){
	this->red = red;
	this->green = green;
	this->blue = blue;
};