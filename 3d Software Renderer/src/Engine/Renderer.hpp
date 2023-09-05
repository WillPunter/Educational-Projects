//Renderer.hpp

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "Window.hpp"
#include "Model.hpp"
#include "Camera.hpp"
#include "Bitmap.hpp"
#include <math.h> 

//light types enumeration
enum LIGHT_TYPES {
	AMBIENT_LIGHT=0,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT
};

//light structure
struct Light {
	int type;
	Vec4f position;
	Vec4f direction;
	double intensity;
	
	Light(int type, Vec4f position, Vec4f direction, double intensity) : type(type), position(position), direction(direction), intensity(intensity) {};
};

//declare class
class Renderer {
	public:
		//constructor
		Renderer(Window * window);
		
		//draw pixel
		void drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
		
		//draw line
		void drawLine(int startX, int startY, int endX, int endY, Pixel endColour);
		void drawLine(int startX, int startY, int endX, int endY, Pixel startColour, Pixel endColour);
		
		//draw triangle
		void drawWireframeTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Pixel c1, Pixel c2, Pixel c3);
		void drawHorizontalLine(int x1, int x2, int y, double i1, double i2, double invD1, double invD2, double tx1, double tx2, double ty1, double ty2, Bitmap * bmp, Pixel c1, Pixel c2);

		//void drawHorizontalLine(int x1, int x2, int y, double i1, double i2, double invD1, double invD2, Pixel c1, Pixel c2);
		//void drawShadedTriangle(int x1, int y1, int x2, int y2, int x3, int y3, double i1, double i2, double i3, double d1, double d2, double d3, Pixel c1, Pixel c2, Pixel c3);
		void drawShadedTriangle(int x1, int y1, int x2, int y2, int x3, int y3, double i1, double i2, double i3, double d1, double d2, double d3, double tx1, double ty1, double tx2, double ty2, double tx3, double ty3, Bitmap * bmp, Pixel c1, Pixel c2, Pixel c3);

		//draw rectangle
		void drawRectangle(int left, int top, int right, int bottom, uint8_t red, uint8_t green, uint8_t blue);
		
		//draw bitmap
		void drawBitmap(Bitmap * bitmap, int x, int y);
		
		//3d model functions
		Triangle transformTriangle(Triangle t, Mat4x4f transform);
		bool cullBackFace(Triangle triangle);
		Triangle convertTriangleToPixelSpace(Triangle triangle);
		Vertex Renderer::shadeVertex(Vertex vertex, Vec4f normal, std::vector<Light> lights);
		Triangle Renderer::applyLighting(Triangle triangle, std::vector<Light> lights);
		int clipTriangleAgainstPlane(Triangle triangle, Vec4f planePos, Vec4f planeNormal, Triangle * clippedTriangle1, Triangle * clippedTriangle2);
		void clipAgainstScreenPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles);
		void clipAgainstTopPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles);
		void clipAgainstBottomPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles);
		void clipAgainstLeftPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles);
		void clipAgainstRightPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles);
		Triangle projectTriangle(Triangle triangle);
		void draw3dTriangle(Triangle t, Mat4x4f transform, Mat4x4f viewTransform, std::vector<Light> lights);
		void draw3dModel(Model * model, Camera * camera, std::vector<Light> lights);
	
		//getters
		double getFov();
		double getProjectionPlaneDistance();
		
		//setters
		void setFov(double fov);
	
	private:
		//data members
		Window * window;
		double fov;
		double tanHalfFov;
};

#endif