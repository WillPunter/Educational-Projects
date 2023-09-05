//Mesh.hpp

#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include "Mathematics.hpp"
#include "Pixel.hpp"
#include "Bitmap.hpp"

//vertex
struct Vertex {
	Vec4f position;
	Vec2f textureCoord;
	Vec4f normal;
	Pixel colour;
	double lightIntensity;
};

//triangle
struct Triangle {
	Vertex vertices[3]; //error may be caused by this being a pointer, so cannot be copied in a copy constructor?
	//Pixel colour; //this causes an error? Why? - because the copy constructor was overriden
	Bitmap * texture;
};

//maybe make notes on copy constructors?

/*
	Notes about the .obj file format:
	# defines a comment
	v defines a vertex with (x, y, z) coordinates
	vn defines a vertex normal (a vector perpendicular to a face) with (x, y, z) coordinates
	vt defines texture coordinates, also called UV coordinates, with (x, y) coordinates 
	f defines a face using 3 vertices each in the format: "vertex coordinate/texture coordinate/normal vector" and each separated by a space.
		-Note that if no texture coordinates are present, vertices will be expressed in the form "vertex coordinate//normal vector"
		
*/

//declare class 
class Mesh {
	//public
	public:
		//store triangles
		std::vector<Vec4f> vertices;
		std::vector<Vec2f> textureCoords;
		std::vector<Vec4f> normals;
		std::vector<Triangle> triangles;
		
		//static
		static bool loadMeshFromObjFile(std::string filePath, Mesh * mesh, std::string bitmapFilePath, Bitmap * bmp){
			//load bitmap
			if(bmp != nullptr){
				Bitmap::loadBitmapFromBMPFile(bitmapFilePath.c_str(), bmp);
			};
			
			//open file
			std::ifstream file;
			file.open(filePath);
			
			//check if file opened correctly
			if(file.is_open()){
				//clear mesh vertices
				mesh->vertices.clear();
				mesh->normals.clear();
				mesh->triangles.clear();
				
				//reserve 
				mesh->vertices.reserve(100);
				mesh->normals.reserve(100);
				mesh->triangles.reserve(100);
				
				//read through .obj file once to count the number of vertices, normals and faces
				std::string line;
				while(std::getline(file, line)){
					//create string stream from line
					std::stringstream lineStream;
					lineStream << line;
					
					//get first word
					std::string token;
					lineStream >> token;
					//std::cout << token << std::endl;
					
					//check token and increment corresponding counter
					if(token == "v"){
						//add vertex to vertices
						Vec4f v;
						lineStream >> v.x;
						lineStream >> v.y;
						lineStream >> v.z;
						v.w = 1;
						mesh->vertices.push_back(v);
					} else if(token == "vn"){
						//add normal to normals
						Vec4f n;
						lineStream >> n.x;
						lineStream >> n.y;
						lineStream >> n.z;
						n.w = 0;
						mesh->normals.push_back(n);
					} else if(token == "vt"){
						//add texture coord to normals
						Vec2f tex;
						lineStream >> tex.x;
						lineStream >> tex.y;
						mesh->textureCoords.push_back(tex);
					} else if(token == "f"){
						//add face to triangles
						Triangle t;
						
						//iterate through face vertices
						for(int i = 0; i < 3; i++){
							//get token
							lineStream >> token;
							
							//get end index
							int startIndex = 0;
							int endIndex = 0;
							
							//index data
							int indices[3];
							int indexCount = 0;
							bool numEndReached = false;
							
							//num string
							std::string numStr;
							
							for(int j = 0; j < token.length(); j++){
								//reset num end
								numEndReached = false;
								
								//check for /
								if(token[j] == '/'){
									//set end index
									endIndex = j;
									numStr = token.substr(startIndex, endIndex - startIndex);
									
									//update start index 
									startIndex = j + 1;
									
									//set num end reached (to indicate that an index must be added)
									numEndReached = true;
								} else if(j == token.length() - 1){
									//update end index
									endIndex = j;
									numStr = token.substr(startIndex, endIndex + 1 - startIndex);
									
									//set num end reached (to indicate that an index must be added)
									numEndReached = true;
								};
								
								//add index if end of number reached
								if(numEndReached){
									//std::cout << "TOKEN (" << i << ")" << token << " NUMSTR: " << numStr << std::endl;
									
									//add index to list
									if(numStr.length() > 0){
										indices[indexCount] = std::stoi(numStr);
									} else {
										indices[indexCount] = -1;
									};
									//std::cout << indices[indexCount] << std::endl;
								
									//increment index
									indexCount++;
								};
							};
							
							//add triangle data for given vertex
							if(indices[0] >= 0){
								t.vertices[i].position = mesh->vertices[indices[0] - 1];
							};
							
							if(indices[1] >= 0){
								t.vertices[i].textureCoord = mesh->textureCoords[indices[1] - 1];
							};
							
							if(indices[2] >= 0){
								t.vertices[i].normal = mesh->normals[indices[2] - 1];
							};
						};
						
						//add triangle
						t.texture = bmp;
						mesh->triangles.push_back(t);
					};
				};
				
				//close file
				file.close();
				
				return true;
			};
			return false;
		};
};

#endif