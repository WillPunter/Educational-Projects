//Renderer.cpp

//include headers
#include "Renderer.hpp"

//constructor
Renderer::Renderer(Window * window){
	//set window
	this->window = window;
};

//destructor?

//draw pixel
void Renderer::drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue){
	//this is simply just a wrapper, so that the window class does not have to be called
	//render buffer implementation may be different on diffent systems, hence the need to use the window function
	this->window->drawPixel(x, y, red, green, blue);
};
	
//draw line
void Renderer::drawLine(int startX, int startY, int endX, int endY, Pixel colour){
	this->drawLine(startX, startY, endX, endY, colour, colour);
};

//draw line and interpolate colour
void Renderer::drawLine(int startX, int startY, int endX, int endY, Pixel startColour, Pixel endColour){
	//draw line using digital differential analysis
	//note that each unsigned integer must be converted to a signed type so that dx and dy can be calculated for different values
	double dx = (double) endX -  (double) startX;
	double dy = (double) endY - (double) startY;
	
	//number of steps
	int numberOfSteps;
	
	//compare dx and dy
	if(abs(dx) < abs(dy)){
		//move along one unit of the y axis, as this is the longer distance, so there will be more points to interpolate
		numberOfSteps = round(abs(dy));
	} else {
		//move along one unit of the x axis, as this is the longer distance, so there will be more points to interpolate
		numberOfSteps = round(abs(dx));
	};
	
	//calculate the step in either axis
	double xStep = dx / numberOfSteps;
	double yStep = dy / numberOfSteps;
	
	//calculate change in each colour
	double dRed = (double) (endColour.red - startColour.red) / numberOfSteps;
	double dGreen = (double) (endColour.green - startColour.green) / numberOfSteps;
	double dBlue = (double) (endColour.blue - startColour.blue) / numberOfSteps;
	
	//calculate start colour
	double red = startColour.red;
	double green = startColour.green;
	double blue = startColour.blue;
	
	//move through steps
	double x = startX;
	double y = startY;
	
	for(int i = 0; i < numberOfSteps; i++){
		//draw pixel
		this->window->drawPixel(x, y, red, green, blue);
		
		//increment position
		x += xStep;
		y += yStep;
		
		//increment colour
		red += dRed;
		green += dGreen;
		blue += dBlue;
	};
};

//draw wireframe triangle
void Renderer::drawWireframeTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Pixel c1, Pixel c2, Pixel c3){
	//draw lines
	this->drawLine(x1, y1, x2, y2, c1, c2);
	this->drawLine(x1, y1, x3, y3, c1, c3);
	this->drawLine(x2, y2, x3, y3, c2, c3);
};

//draw horizontal line
void Renderer::drawHorizontalLine(int x1, int x2, int y, double i1, double i2, double invD1, double invD2, double tx1, double tx2, double ty1, double ty2, Bitmap * bmp, Pixel c1, Pixel c2){
	//swap x1 and x2, so that x1 <= x2
	if(x2 < x1){
		int temp = x1;
		x1 = x2;
		x2 = temp;
		
		double tempD = i1;
		i1 = i2;
		i2 = tempD;
		
		tempD = invD1;
		invD1 = invD2;
		invD2 = tempD;
		
		tempD = tx1;
		tx1 = tx2;
		tx2 = tempD;
		
		tempD = ty1;
		ty1 = ty2;
		ty2 = tempD;
		
		Pixel tempP = c1;
		c1 = c2;
		c2 = tempP;
	};
	
	double rStep = (double) (c2.red - c1.red) / (x2 - x1);
	double gStep = (double) (c2.green - c1.green) / (x2 - x1);
	double bStep = (double) (c2.blue - c1.blue) / (x2 - x1);
	
	double iStep = (i2 - i1) / (x2 - x1);
	
	double dStep = (invD2 - invD1) / (x2 - x1);
	
	double txStep = (tx2 - tx1) / (x2 - x1);
	double tyStep = (ty2 - ty1) / (x2 - x1);
	
	double r = c1.red;
	double g = c1.green;
	double b = c1.blue;
	
	double intensity = i1;
	
	double inverseDepth = invD1;
	
	double tx = tx1;
	double ty = ty1;
	
	for(int i = x1; i <= x2; i++){
		//sample bitmap to get pixel colour at (tx, ty) (since tx and ty are normalised, they must be multiplied by the width and height of the bitmap in pixels)
		Pixel colour = bmp->pixels[(int) (floor(ty * bmp->infoHeader.biHeight) * bmp->infoHeader.biWidth + floor(tx * bmp->infoHeader.biWidth))];
		
		//draw pixel 
		this->window->drawPixel(i, y, 1.f / inverseDepth, (uint8_t) ((double) colour.red * intensity), (uint8_t) ((double) colour.green * intensity), (uint8_t) ((double) colour.blue * intensity));
		
		//step forward
		r += rStep;
		g += gStep;
		b += bStep;
		intensity += iStep;
		inverseDepth += dStep;
		tx += txStep;
		ty += tyStep;
	};
};

//draw shaded triangle
void Renderer::drawShadedTriangle(int x1, int y1, int x2, int y2, int x3, int y3, double i1, double i2, double i3, double d1, double d2, double d3, double tx1, double ty1, double tx2, double ty2, double tx3, double ty3, Bitmap * bmp, Pixel c1, Pixel c2, Pixel c3){
	//draw shaded triangle
	
	//sort points by y-coordinate
	int temp;
	double tempD;
	Pixel tempPixel;
	
	if(y2 < y1){
		//swap v1 and v2
		temp = x1;
		x1 = x2;
		x2 = temp;
		
		temp = y1;
		y1 = y2;
		y2 = temp;
		
		tempD = i1;
		i1 = i2;
		i2 = tempD;
		
		tempD = d1;
		d1 = d2;
		d2 = tempD;
		
		tempD = tx1;
		tx1 = tx2;
		tx2 = tempD;
		
		tempD = ty1;
		ty1 = ty2;
		ty2 = tempD;
		
		tempPixel = c1;
		c1 = c2;
		c2 = tempPixel;
	};
	
	if(y3 < y1){
		//swap v3 and v1
		temp = x1;
		x1 = x3;
		x3 = temp;
		
		temp = y1;
		y1 = y3;
		y3 = temp;
		
		tempD = i1;
		i1 = i3;
		i3 = tempD;
		
		tempD = d1;
		d1 = d3;
		d3 = tempD;
		
		tempD = tx1;
		tx1 = tx3;
		tx3 = tempD;
		
		tempD = ty1;
		ty1 = ty3;
		ty3 = tempD;
		
		tempPixel = c1;
		c1 = c3;
		c3 = tempPixel;
	};
	
	if(y3 < y2){
		//swap v3 and v2
		temp = x3;
		x3 = x2;
		x2 = temp;
		
		temp = y3;
		y3 = y2;
		y2 = temp;
		
		tempD = i3;
		i3 = i2;
		i2 = tempD;
		
		tempD = d3;
		d3 = d2;
		d2 = tempD;
		
		tempD = tx3;
		tx3 = tx2;
		tx2 = tempD;
		
		tempD = ty3;
		ty3 = ty2;
		ty2 = tempD;
		
		tempPixel = c3;
		c3 = c2;
		c2 = tempPixel;
	};
	
	//pixel x coordinates
	double px1 = x1;
	double px2 = x1;
	
	double xStep1 = (double) (x3 - x1) / (y3 - y1); //xStep1 is the increase in x per unit y for the line from v1 to v3
	double xStep2 = (double) (x2 - x1) / (y2 - y1); //xStep2 is the increase in x per unit y for the line from v1 to v2
	
	//pixel colours
	//change in colours for line v1 -> v3
	double rStep1 = (double) (c3.red - c1.red) / (y3 - y1);
	double gStep1 = (double) (c3.green - c1.green) / (y3 - y1);
	double bStep1 = (double) (c3.blue - c1.blue) / (y3 - y1);
	
	//change in colours from line v1 -> v2
	double rStep2 = (double) (c2.red - c1.red) / (y2 - y1);
	double gStep2 = (double) (c2.green - c1.green) / (y2 - y1);
	double bStep2 = (double) (c2.blue - c1.blue) / (y2 - y1);
	
	//change in intensity for line v1 -> v3
	double iStep1 = (i3 - i1) / (y3 - y1);
	
	//change in intensity for line v1 -> v2
	double iStep2 = (i2 - i1) / (y2 - y1);
	
	/*
		Note about depth:
		As can be seen from the projection equation, depth is not linearly interpolated across the pixels;
		1 / depth is linearly interpolated.
	*/
	
	//change in 1 / depth
	double dStep1 = (1.f / d3 - 1.f / d1) / (y3 - y1);
	double dStep2 = (1.f / d2 - 1.f / d1) / (y2 - y1);
	
	//texture coordinates
	double txStep1 = (tx3 - tx1) / (y3 - y1);
	double tyStep1 = (ty3 - ty1) / (y3 - y1);
	double txStep2 = (tx2 - tx1) / (y2 - y1);
	double tyStep2 = (ty2 - ty1) / (y2 - y1);
	
	double texX1 = tx1;
	double texX2 = tx1;
	double texY1 = ty1;
	double texY2 = ty1;
	
	//calculate colours
	double r1 = c1.red;
	double g1 = c1.green;
	double b1 = c1.blue;
	
	double r2 = c1.red;
	double g2 = c1.green;
	double b2 = c1.blue;
	
	double intensity1 = i1;
	double intensity2 = i1;
	
	double depth1 = 1.f / d1;
	double depth2 = 1.f / d1;
	
	//iterate between y1 and y2
	for(int i = y1; i < y2; i++){
		//draw row
		this->drawHorizontalLine(px1, px2, i, intensity1, intensity2, depth1, depth2, texX1, texX2, texY1, texY2, bmp, Pixel(r1, g1, b1), Pixel(r2, g2, b2));
		
		//increment pixel x coordintes
		px1 += xStep1; //increment v1 -> v3 x coordinate
		px2 += xStep2; //increment v1 -> v2 x coordinate
		
		//increment colour interpolations
		r1 += rStep1;
		g1 += gStep1;
		b1 += bStep1;
		
		r2 += rStep2;
		g2 += gStep2;
		b2 += bStep2;
		
		//increment colour intensity
		intensity1 += iStep1;
		intensity2 += iStep2;
		
		//increment depth
		depth1 += dStep1;
		depth2 += dStep2;
		
		//increment texture coordinates
		texX1 += txStep1;
		texX2 += txStep2;
		texY1 += tyStep1;
		texY2 += tyStep2;
	};
	
	//re-calculate steps
	px2 = x2;
	r2 = c2.red;
	g2 = c2.green;
	b2 = c2.blue;
	intensity2 = i2;
	depth2 = 1.f / d2;
	texX2 = tx2;
	texY2 = ty2;
	
	xStep2 = (double) (x3 - x2) / (y3 - y2);
	rStep2 = (double) (c3.red - c2.red) / (y3 - y2);
	gStep2 = (double) (c3.green - c2.green) / (y3 - y2);
	bStep2 = (double) (c3.blue - c2.blue) / (y3 - y2);
	iStep2 = (i3 - i2) / (y3 - y2);
	dStep2 = (1.f / d3 - 1.f / d2) / (y3 - y2);
	txStep2 = (tx3 - tx2) / (y3 - y2);
	tyStep2 = (ty3 - ty2) / (y3 - y2);
	
	//draw the second half of the triangle
	for(int i = y2; i < y3; i++){
		//draw row
		this->drawHorizontalLine(px1, px2, i, intensity1, intensity2, depth1, depth2, texX1, texX2, texY1, texY2, bmp, Pixel(r1, g1, b1), Pixel(r2, g2, b2));
		
		//increment pixel x coordinates
		px1 += xStep1;
		px2 += xStep2;
		
		//increment colour interpolations
		r1 += rStep1;
		g1 += gStep1;
		b1 += bStep1;
		
		r2 += rStep2;
		g2 += gStep2;
		b2 += bStep2;
		
		//increment light intensity
		intensity1 += iStep1;
		intensity2 += iStep2;
		
		//increment depth
		depth1 += dStep1;
		depth2 += dStep2;
		
		//increment texture coordinates
		texX1 += txStep1;
		texX2 += txStep2;
		texY1 += tyStep1;
		texY2 += tyStep2;
	};
};

//draw textured triangle

//draw rectangle
void Renderer::drawRectangle(int left, int top, int right, int bottom, uint8_t red, uint8_t green, uint8_t blue){
	//draw rectangle
	for(int i = left; i < right; i++){
		for(int j = top; j < bottom; j++){
			this->window->drawPixel(i, j, red, green, blue);
		};
	};
};

//draw bitmap
void Renderer::drawBitmap(Bitmap * bitmap, int x, int y){
	for(int i = 0; i < bitmap->infoHeader.biWidth; i++){
		for(int j = 0; j < bitmap->infoHeader.biHeight; j++){
			Pixel colour = bitmap->pixels[j * bitmap->infoHeader.biWidth + i];
			this->window->drawPixel(x + i, y + j, 0, colour.red, colour.green, colour.blue);
		};
	};
};	

//transform triangle
Triangle Renderer::transformTriangle(Triangle t, Mat4x4f transform){
	//iterate through vertices
	for(int i = 0; i < 3; i++){
		//transform position
		t.vertices[i].position = Math::matrixProduct(transform, t.vertices[i].position);
	
		//transform normal
		//TODO: implement normal transformation using transpose of inverse method
		//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/geometry/transforming-normals.html
		//t->vertices[i].normal = Math::matrixProduct(transform, t->);
	};
	
	return t;
};

//cull back faces
//TODO: optimise this function to run before view-space / camera transformations, so out-of-sight triangles do not need to be transformed
bool Renderer::cullBackFace(Triangle triangle){
	//assume camera is at (0, 0, 0) (as triangles should be transformed to view space by this point)
	Vec4f normal = Math::crossProduct(triangle.vertices[1].position - triangle.vertices[0].position, triangle.vertices[2].position - triangle.vertices[1].position);
	
	//check angle between normal and vertex
	double result = Math::dotProduct(normal, triangle.vertices[0].position);
	
	return result < 0;
};

//convert triangle to pixel space
Triangle Renderer::convertTriangleToPixelSpace(Triangle triangle){
	for(int i = 0; i < 3; i++){
		//convert vertex
		triangle.vertices[i].position = Math::ndcToPixel(triangle.vertices[i].position, this->window->getWidth(), this->window->getHeight());
	};
	return triangle;
};

//shade vertex
Vertex Renderer::shadeVertex(Vertex vertex, Vec4f normal, std::vector<Light> lights){
	//reset intensity
	vertex.lightIntensity = 0;
	
	//iterate through lights
	for(int i = 0; i < lights.size(); i++){
		//check light type
		if(lights[i].type == AMBIENT_LIGHT){
			vertex.lightIntensity += lights[i].intensity;
		} else if(lights[i].type == POINT_LIGHT){
			//calculate vector from point to light
			Vec4f direction = vertex.position - lights[i].position;
			
			double diffuseIntensity = lights[i].intensity * Math::dotProduct(direction, normal) / (Math::magnitude(direction) * Math::magnitude(normal));
			
			if(diffuseIntensity > 0){
				vertex.lightIntensity += diffuseIntensity;
			};
		} else if(lights[i].type == DIRECTIONAL_LIGHT){
			//calculate diffuse intensity
			double diffuseIntensity = lights[i].intensity * Math::dotProduct(lights[i].direction, normal);
			
			if(diffuseIntensity > 0){
				vertex.lightIntensity += diffuseIntensity;
			};
		};
	};
	
	if(vertex.lightIntensity > 1){
		vertex.lightIntensity = 1;
	} else if(vertex.lightIntensity < 0){
		vertex.lightIntensity = 0;
	};
	
	return vertex;
};

//apply lighting
Triangle Renderer::applyLighting(Triangle triangle, std::vector<Light> lights){
	//calculate normal 
	Vec4f normal = Math::crossProduct(triangle.vertices[2].position - triangle.vertices[0].position, triangle.vertices[1].position - triangle.vertices[0].position);
	
	//apply lighting to each vertex
	for(int i = 0; i < 3; i++){
		triangle.vertices[i] = this->shadeVertex(triangle.vertices[i], normal, lights);
	};
	
	return triangle;
};

//clip triangle against plane
int Renderer::clipTriangleAgainstPlane(Triangle triangle, Vec4f planePos, Vec4f planeNormal, Triangle * clippedTriangle1, Triangle * clippedTriangle2){
	//create inside and outside vertex arrays
	Vec4f * insideVertices[3];
	Vec4f * outsideVertices[3];
	
	int insideCount = 0;
	int outsideCount = 0;
	
	//iterate through points and determine if they are inside or outside of the plane
	for(int i = 0; i < 3; i ++){
		//distance
		double distance = Math::shortestDistanceFromPointToPlane(planePos, planeNormal, triangle.vertices[i].position);
		
		//check if on the inside or outside area of the plane 
		if(distance >= 0){
			//distance from point to plane is in the direction of the normal
			//this means that the point is inside the plane and the distance is pointing outward
			insideVertices[insideCount] = &triangle.vertices[i].position;
			insideCount++;
		} else {
			//distance from point to plane is in the opposite direction to the normal
			//this means that the point is outside the plane and the distance is pointing inward
			outsideVertices[outsideCount] = &triangle.vertices[i].position;
			outsideCount++;
		};
	};
	
	//check the number of inside vertices
	if(insideCount == 3){
		//set triangle 1 to current triangle (no vertices need to be clipped)
		*clippedTriangle1 = triangle;
		return 1;
	} else if(insideCount == 2){
		//get first triangle
		Vec4f i1oIntercept = Math::lineIntersectPlane(planePos, planeNormal, *insideVertices[0], *outsideVertices[0] - *insideVertices[0]);
		Vec4f i2oIntercept = Math::lineIntersectPlane(planePos, planeNormal, *insideVertices[1], *outsideVertices[0] - *insideVertices[1]);
		
		//set first triangle
		*outsideVertices[0] = i1oIntercept;
		*clippedTriangle1 = triangle;
		
		//swap inside vertices 0 and i20 intercept
		*insideVertices[0] = i2oIntercept;
		*clippedTriangle2 = triangle;
		return 2;
	} else if(insideCount == 1){
		//find intecepts between inside to outside lines and the plane
		*outsideVertices[0] = Math::lineIntersectPlane(planePos, planeNormal, *insideVertices[0], *outsideVertices[0] - *insideVertices[0]);
		*outsideVertices[1] = Math::lineIntersectPlane(planePos, planeNormal, *insideVertices[0], *outsideVertices[1] - *insideVertices[0]);
		
		//set triangle
		*clippedTriangle1 = triangle;
		return 1;
	};
	
	return 0;
};

//project triangle
Triangle Renderer::projectTriangle(Triangle triangle){
	for(int i = 0; i < 3; i++){
		//project vertex
		triangle.vertices[i].position = Math::projectVector(triangle.vertices[i].position, this->window->getAspectRatio(), this->tanHalfFov);		
	};
	return triangle;
};

//Found possible cause of error - triangles are being duplicated when clipping occurs!
//for example, when triangles are clipped in two, the original triangle is not removed from the clipped triangles vector
//TODO: find a better way of clipping against all four planes! 

//clip against screen plane
void Renderer::clipAgainstScreenPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles){
	//set screen plane
	Vec4f screenPlanePoint(0.0f, 0.0f, this->getProjectionPlaneDistance(), 1.0f);
	Vec4f screenPlaneNormal(0.0f, 0.0f, -1.0f, 0.0f);
	
	//int Renderer::clipTriangleAgainstPlane(Triangle triangle, Vec4f planePos, Vec4f planeNormal, Triangle * clippedTriangle1, Triangle * clippedTriangle2)
	
	//
	Triangle clippedTriangle1;
	Triangle clippedTriangle2;
	int numberOfClippedTriangles = this->clipTriangleAgainstPlane(triangle, screenPlanePoint, screenPlaneNormal, &clippedTriangle1, &clippedTriangle2);
	
	if(numberOfClippedTriangles == 2){
		clippedTriangles->push_back(clippedTriangle1);
		clippedTriangles->push_back(clippedTriangle2);
		
		//this->clipAgainstLeftPlane(clippedTriangle1, clippedTriangles);
		//this->clipAgainstLeftPlane(clippedTriangle2, clippedTriangles);
	} else if(numberOfClippedTriangles == 1){
		clippedTriangles->push_back(clippedTriangle1);
		//this->clipAgainstLeftPlane(clippedTriangle1, clippedTriangles);
	};
};

//clip against left plane
void Renderer::clipAgainstLeftPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles){
	//set left plane
	Vec4f leftPlanePoint(0.0f, 0.0f, 0.0f, 1.0f);
	Vec4f leftPlaneNormal(-this->getProjectionPlaneDistance(), 0.0f, -1.0f, 0.0f);
	
	//int Renderer::clipTriangleAgainstPlane(Triangle triangle, Vec4f planePos, Vec4f planeNormal, Triangle * clippedTriangle1, Triangle * clippedTriangle2)
	
	//
	Triangle clippedTriangle1;
	Triangle clippedTriangle2;
	int numberOfClippedTriangles = this->clipTriangleAgainstPlane(triangle, leftPlanePoint, leftPlaneNormal, &clippedTriangle1, &clippedTriangle2);
	
	if(numberOfClippedTriangles == 2){
		clippedTriangles->push_back(clippedTriangle1);
		clippedTriangles->push_back(clippedTriangle2);
		
		this->clipAgainstRightPlane(clippedTriangle1, clippedTriangles);
		this->clipAgainstRightPlane(clippedTriangle2, clippedTriangles);
	} else if(numberOfClippedTriangles == 1){
		clippedTriangles->push_back(clippedTriangle1);
		this->clipAgainstRightPlane(clippedTriangle1, clippedTriangles);
	};
};

//clip against right plane
void Renderer::clipAgainstRightPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles){
	//set right plane
	Vec4f rightPlanePoint(0.0f, 0.0f, 0.0f, 1.0f);
	Vec4f rightPlaneNormal(this->getProjectionPlaneDistance(), 0.0f, -1.0f, 0.0f);;
	
	//int Renderer::clipTriangleAgainstPlane(Triangle triangle, Vec4f planePos, Vec4f planeNormal, Triangle * clippedTriangle1, Triangle * clippedTriangle2)
	
	//
	Triangle clippedTriangle1;
	Triangle clippedTriangle2;
	int numberOfClippedTriangles = this->clipTriangleAgainstPlane(triangle, rightPlanePoint, rightPlaneNormal, &clippedTriangle1, &clippedTriangle2);
	
	if(numberOfClippedTriangles == 2){
		clippedTriangles->push_back(clippedTriangle1);
		clippedTriangles->push_back(clippedTriangle2);
		
		this->clipAgainstTopPlane(clippedTriangle1, clippedTriangles);
		this->clipAgainstTopPlane(clippedTriangle2, clippedTriangles);
	} else if(numberOfClippedTriangles == 1){
		clippedTriangles->push_back(clippedTriangle1);
		this->clipAgainstTopPlane(clippedTriangle1, clippedTriangles);
	};
};

//clip against top plane
void Renderer::clipAgainstTopPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles){
	//set top plane
	Vec4f topPlanePoint(0.0f, 0.0f, 0.0f, 1.0f);
	Vec4f topPlaneNormal(0.0f, this->getProjectionPlaneDistance(), -1.0f, 0.0f);
	
	//int Renderer::clipTriangleAgainstPlane(Triangle triangle, Vec4f planePos, Vec4f planeNormal, Triangle * clippedTriangle1, Triangle * clippedTriangle2)
	
	//
	Triangle clippedTriangle1;
	Triangle clippedTriangle2;
	int numberOfClippedTriangles = this->clipTriangleAgainstPlane(triangle, topPlanePoint, topPlaneNormal, &clippedTriangle1, &clippedTriangle2);
	
	if(numberOfClippedTriangles == 2){
		clippedTriangles->push_back(clippedTriangle1);
		clippedTriangles->push_back(clippedTriangle2);
		
		this->clipAgainstBottomPlane(clippedTriangle1, clippedTriangles);
		this->clipAgainstBottomPlane(clippedTriangle2, clippedTriangles);
	} else if(numberOfClippedTriangles == 1){
		clippedTriangles->push_back(clippedTriangle1);
		this->clipAgainstBottomPlane(clippedTriangle1, clippedTriangles);
	};
};

//clip against bottom plane
void Renderer::clipAgainstBottomPlane(Triangle triangle, std::vector<Triangle> * clippedTriangles){
	//set bottom plane
	Vec4f bottomPlanePoint(0.0f, 0.0f, 0.0f, 1.0f);
	Vec4f bottomPlaneNormal(0.0f, this->getProjectionPlaneDistance(), -1.0f, 0.0f);
	
	Triangle clippedTriangle1;
	Triangle clippedTriangle2;
	int numberOfClippedTriangles = this->clipTriangleAgainstPlane(triangle, bottomPlanePoint, bottomPlaneNormal, &clippedTriangle1, &clippedTriangle2);
	
	if(numberOfClippedTriangles == 2){
		clippedTriangles->push_back(clippedTriangle1);
		clippedTriangles->push_back(clippedTriangle2);
	} else if(numberOfClippedTriangles == 1){
		clippedTriangles->push_back(clippedTriangle1);
	};
};

//draw 3d triangle
void Renderer::draw3dTriangle(Triangle t, Mat4x4f transform, Mat4x4f viewTransform, std::vector<Light> lights){
	//transform triangle to world space
	t = this->transformTriangle(t, transform);
	
	//apply lighting to triangle in world space
	t = this->applyLighting(t, lights);
	
	//transform triangle to view space
	t = this->transformTriangle(t, viewTransform);
	
	//TODO - implement proper clipping
	
	//check if face is visible
	if(this->cullBackFace(t) && t.vertices[0].position.z > this->getProjectionPlaneDistance() && t.vertices[1].position.z > this->getProjectionPlaneDistance() && t.vertices[2].position.z > this->getProjectionPlaneDistance()){
		//project triangle
		t = this->projectTriangle(t);
		
		//check against x and y bounds
		if(!((t.vertices[0].position.x < -1 && t.vertices[1].position.x < -1 && t.vertices[2].position.x < -1) || (t.vertices[0].position.x > 1 && t.vertices[1].position.x > 1 && t.vertices[2].position.x > 1) || (t.vertices[0].position.y < -1 && t.vertices[1].position.y < -1 && t.vertices[2].position.y < -1) || (t.vertices[0].position.y > 1 && t.vertices[1].position.y > 1 && t.vertices[2].position.y > 1))){
			t = this->convertTriangleToPixelSpace(t);
			
			this->drawShadedTriangle(t.vertices[0].position.x, t.vertices[0].position.y, t.vertices[1].position.x, t.vertices[1].position.y, t.vertices[2].position.x, t.vertices[2].position.y, t.vertices[0].lightIntensity, t.vertices[1].lightIntensity, t.vertices[2].lightIntensity, t.vertices[0].position.z, t.vertices[1].position.z, t.vertices[2].position.z, t.vertices[0].textureCoord.x, t.vertices[0].textureCoord.y, t.vertices[1].textureCoord.x, t.vertices[1].textureCoord.y, t.vertices[2].textureCoord.x, t.vertices[2].textureCoord.y, t.texture, Pixel(255, 255, 255), Pixel(255, 255, 255), Pixel(255, 255, 255));	
		};
		
		/*
		//clip triangle against screen plane
		std::vector<Triangle> clippedTriangles;
		clippedTriangles.push_back(t);
		
		//clip triangle against screen plane
		//this->clipAgainstScreenPlane(t, &clippedTriangles);
		
	//	this->clipTria
		
		for(int i = 0; i < clippedTriangles.size(); i++){
			//project triangle
			clippedTriangles[i] = this->projectTriangle(clippedTriangles[i]);
		
			//get rid of triangle if behind camera
			if(cliooed){
		
			//convert triangle to pixel space
			clippedTriangles[i] = this->convertTriangleToPixelSpace(clippedTriangles[i]);
			
			//draw triangle
			//this->drawWireframeTriangle(clippedTriangles[i].vertices[0].position.x, clippedTriangles[i].vertices[0].position.y, clippedTriangles[i].vertices[1].position.x, clippedTriangles[i].vertices[1].position.y, clippedTriangles[i].vertices[2].position.x, clippedTriangles[i].vertices[2].position.y, Pixel(255, 255, 255), Pixel(255, 255, 255), Pixel(255, 255, 255));	
		
			this->drawShadedTriangle(clippedTriangles[i].vertices[0].position.x, clippedTriangles[i].vertices[0].position.y, clippedTriangles[i].vertices[1].position.x, clippedTriangles[i].vertices[1].position.y, clippedTriangles[i].vertices[2].position.x, clippedTriangles[i].vertices[2].position.y, clippedTriangles[i].vertices[0].lightIntensity, clippedTriangles[i].vertices[1].lightIntensity, clippedTriangles[i].vertices[2].lightIntensity, clippedTriangles[i].vertices[0].position.z, clippedTriangles[i].vertices[1].position.z, clippedTriangles[i].vertices[2].position.z, clippedTriangles[i].vertices[0].textureCoord.x, clippedTriangles[i].vertices[0].textureCoord.y, clippedTriangles[i].vertices[1].textureCoord.x, clippedTriangles[i].vertices[1].textureCoord.y, clippedTriangles[i].vertices[2].textureCoord.x, clippedTriangles[i].vertices[2].textureCoord.y, clippedTriangles[i].texture, Pixel(255, 255, 255), Pixel(255, 255, 255), Pixel(255, 255, 255));	
		};*/
	};
};

//draw 3d mesh
void Renderer::draw3dModel(Model * model, Camera * camera, std::vector<Light> lights){
	//calculate model transformation matrix
	Mat4x4f transform = Math::enlargementMatrix(model->enlargement);
	transform = Math::matrixProduct(Math::rotationMatrix(model->rotation), transform);
	transform = Math::matrixProduct(Math::translationMatrix(model->translation), transform);
	
	//calculate view space transformation
	Mat4x4f viewTransform = camera->getCameraTransformationMatrix();
	
	//iterate through triangles
	for(int i = 0; i < model->mesh->triangles.size(); i++){
		//draw 3d triangle
		this->draw3dTriangle(model->mesh->triangles[i], transform, viewTransform, lights);
	};
};

//getters
double Renderer::getFov(){
	return this->fov;
};

double Renderer::getProjectionPlaneDistance(){
	return this->tanHalfFov;
};

//setters
void Renderer::setFov(double fov){
	this->fov = fov;
	this->tanHalfFov = tan(this->fov / 2);
};