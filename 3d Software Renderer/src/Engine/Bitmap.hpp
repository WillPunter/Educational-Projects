//Bitmap.hpp

#ifndef BITMAP_HPP
#define BITMAP_HPP

#include "Pixel.hpp"
#include <fstream>

class Bitmap {
	public:
		BITMAPFILEHEADER fileHeader;
		BITMAPINFOHEADER infoHeader;
		Pixel * pixels;
		
		static void loadBitmapFromBMPFile(const char * filePath, Bitmap * bitmap){
			//open file
			std::ifstream file;
			file.open(filePath);
			
			if(file.is_open()){
				//read file header
				file.read((char*) &bitmap->fileHeader, sizeof(BITMAPFILEHEADER));
				file.read((char*) &bitmap->infoHeader, sizeof(BITMAPINFOHEADER));
				
				//allocate pixel buffer
				bitmap->pixels = new Pixel[bitmap->infoHeader.biWidth * bitmap->infoHeader.biHeight];
				
				Pixel newPixel;
				
				//iterate through rows
				for(int i = 0; i < bitmap->infoHeader.biHeight; i++){
					for(int j = 0; j < bitmap->infoHeader.biWidth; j++){
						//read pixel
						int index = bitmap->infoHeader.biWidth * i + j;
						file.read((char*) &bitmap->pixels[index], sizeof(Pixel));
						//file >> bitmap->pixels[index].red;
						//file >> bitmap->pixels[index].green;
						//file >> bitmap->pixels[index].blue;
						//std::cout << "r: " << (int) bitmap->pixels[index].red << ", g: " << (int) bitmap->pixels[index].green << ", b: " << (int) bitmap->pixels[index].blue << std::endl;
					};
					
					//read buffer pad at the end of the scanline
					int leftOverBytes = (bitmap->infoHeader.biWidth * sizeof(Pixel)) % 4;
					file.ignore(leftOverBytes);
				};
			};
		};
};

#endif