//rasteriser
//cl main.c /EHsc /O2 /link gdi32.lib user32.lib /out:./game.exe

//include headers
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <time.h>

//define window size constants
#define BUFFER_WIDTH 256
#define BUFFER_HEIGHT 256

//define viewport constants
#define VIEWPORT_WIDTH 1
#define VIEWPORT_HEIGHT 1

//define pi
#define PI 3.141592

//define lights count
#define LIGHT_COUNT 32

//light type enumeration
enum LIGHT_TYPES {
	AMBIENT_LIGHT,
	POINT_LIGHT,
	DIRECTIONAL_LIGHT
};

//create pixel structure
//this is a 24-bit structure that stores a red, green and blue byte
typedef struct Pixel {
	uint8_t blue;
	uint8_t green;
	uint8_t red;
} Pixel;

//define render buffer structure
typedef struct RenderBuffer {
	HWND windowHandle; //handle to the window that the buffer belongs to
	HDC deviceContextHandle; //handle to device context (a device context is an area of memory stored by the OS that will be rendered directly to the window client area)
	Pixel * pixels; //pixel buffer to render to the screen
	float * pixelZ; //pixel z buffer
	BITMAPINFO bitmapInfo; //a bitmap info structure that stores necessary metadata for our renderBuffer (required to draw our data to the screen)
	int scale;
	int windowClientWidth;
	int windowClientHeight;
} RenderBuffer;

//vector structures
typedef struct Vector2f {
	float x;
	float y;
} Vector2f;

typedef struct Vector4f {
	float x;
	float y;
	float z;
	float w;
} Vector4f;

//create matrix structure
typedef struct Matrix4x4 {
	float data[4][4];
} Matrix4x4;

//create triangle structure
typedef struct Triangle {
	Vector4f vertices[3];
	float intensities[3];
	Pixel colour;
} Triangle;

//create mesh structure
typedef struct Mesh {
	int trianglesCount;
	Triangle * triangles;
} Mesh;

//create model structure
typedef struct Model {
	Mesh * mesh;
	Vector4f position;
	Vector4f scale;
	Vector4f rotation;
} Model;

//create camera structure
typedef struct Camera {
	Vector4f position;
	Vector4f rotation;
} Camera;

//game clock
typedef struct GameClock {
	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime;
	LARGE_INTEGER clockFrequency;
	double deltaTime;
} GameClock;

//light
typedef struct Light {
	int alive;
	int type;
	float intensity;
	Vector4f position;
	Vector4f direction;
} Light;

//load mesh from file
void loadMeshFromFile(char filepath[], Mesh * mesh);

//initialise models
void initialiseModels();

//initialise
void initialise();

//vector functions
float magnitudeVector4f(Vector4f v1);
Vector4f addVector4f(Vector4f v1, Vector4f v2);
Vector4f subtractVector4f(Vector4f v1, Vector4f v2);
Vector4f scalarProductVector4f(Vector4f v1, float scalar);
float dotProductVector4f(Vector4f v1, Vector4f v2);
Vector4f crossProduct(Vector4f v1, Vector4f v2);

//matrix functions
Matrix4x4 initialiseMatrix4x4(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p);
Matrix4x4 scalarProductMatrix4x4(Matrix4x4 mat, float scalar);
Vector4f matrix4x4Vectorf4fProduct(Matrix4x4 mat, Vector4f vec);
Matrix4x4 matrix4x4Product(Matrix4x4 mat1, Matrix4x4 mat2);

//miscellaneous mathematical functions
float sign(float num);
float getPixelZ(float x, float y);
void setPixelZ(float x, float y, float z);

//primitive rendering functions
void drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
void drawLine(int x1, int y1, int x2, int y2, uint8_t red, uint8_t green, uint8_t blue);
void drawTriangle(int x1, int y1, float z1, float i1, int x2, int y2, float z2, float i2, int x3, int y3, float z3, float i3, uint8_t red, uint8_t green, uint8_t blue);
void drawWireframeTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t red, uint8_t green, uint8_t blue);

//transformation functions
Vector2f projectCoordinates(Vector4f v);
Vector2f rotate2dPoint(float x, float y, float angle);
Vector4f rotatePoint(Vector4f v1, Vector4f rotation);
Vector4f rotatePoint(Vector4f v1, Vector4f rotation);
Triangle rotateTriangle(Triangle triangle, Vector4f rotation);
Triangle translateTriangle(Triangle triangle, Vector4f translation);

//render model
void renderModel(Model * model);

//main loop function
void mainLoop();

//handle events function
void handleEvents();

//render function
void render();

//declare window procedure (event handler) function
LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT messageID, WPARAM wParam, LPARAM lParam);

//create global variables
int running = 1; //1 when program is running, 0 when program is closed
RenderBuffer renderBuffer; //our global render buffer#
float fov = PI / 2;
float fovDistance;
Mesh obj_spaceship;
Model mod_spaceship;
Model mod_spaceship2;
Camera camera;
GameClock gameClock;
Light lights[LIGHT_COUNT];

float testx1;
float testy1;
float testx2;
float testy2;
float testx3;
float testy3;

//function definitions
//load mesh from file
void loadMeshFromFile(char filePath[], Mesh * mesh){
	//attempt to open file
	FILE * file = fopen(filePath, "r");
	
	if(file){
		//get file size
		fseek(file, 0, SEEK_END);
		int fileSize = ftell(file);
		rewind(file);
		
		//create buffer to store file data
		char * buffer = malloc(fileSize);
		
		//read data into buffer
		fread(buffer, 1, fileSize, file);
		
		/*
			In a .obj, the data is in text format.
			Points are represented using the format "v  %f %f %f", where the floating point numbers are the xyz coordinates.
			Triangles (faces) are represented using the format "f %d %d %d", where the integers are the vertex/point numbers (starting at 1)
			
			Next, read in the number of vertices and the number of faces.
		*/
		
		int vertexCount = 0;
		mesh->trianglesCount = 0;
		
		int i;
		
		for(i = 0; i < fileSize; i++){
			//check for start of line
			if(i == 0 || buffer[i - 1] == '\n'){
				//check starting character
				if(buffer[i] == 'v'){
					vertexCount += 1;
				} else if(buffer[i] == 'f'){
					mesh->trianglesCount += 1;
				};
			};
		};
		
		//allocate data for vertex array
		Vector4f * vertices = (Vector4f *) malloc(vertexCount * sizeof(Vector4f));
		
		//allocate data for triangles
		mesh->triangles = (Triangle *) malloc(mesh->trianglesCount * sizeof(Triangle));
		
		//create current vertex and face variable 
		int currentVertex = 0;
		int currentFace = 0;
		
		//iterate through buffer
		for(i = 0; i < fileSize; i++){
			//check for start of line
			if(i == 0 || buffer[i - 1] == '\n'){
				//check starting character
				if(buffer[i] == 'v'){
					//get vertex coordinates
					float x;
					float y;
					float z;
					
					//read x, y and z coordinates in from buffer
					sscanf(buffer + i, "v %f %f %f\r\n", &x, &y, &z);
					
					//print for debugging
					//printf("\nNew Vertex: %f %f %f\n", x, y, z);
					
					//add to vertex
					vertices[currentVertex].x = x;
					vertices[currentVertex].y = y;
					vertices[currentVertex].z = z;
					
					//increment to next vertex
					currentVertex += 1;
				} else if(buffer[i] == 'f'){
					//get vertex indices
					int v1;
					int v2;
					int v3;
					
					//read vertex indices in from buffer
					sscanf(buffer + i, "f %d %d %d", &v1, &v2, &v3);
					
					//add vertices at given index into 
					mesh->triangles[currentFace].vertices[0] = vertices[v1 - 1];
					mesh->triangles[currentFace].vertices[1] = vertices[v2 - 1];
					mesh->triangles[currentFace].vertices[2] = vertices[v3 - 1];
					
					//set triangle colour to white
					mesh->triangles[currentFace].colour.red = 255;
					mesh->triangles[currentFace].colour.green = 255;
					mesh->triangles[currentFace].colour.blue = 255;
					
					//increment to next face
					currentFace += 1;
				};
			};
		};
		
		//close file
		fclose(file);
	} else {
			printf("Failed to open mesh file");
		};
};

//initialise models
void initialiseModels(){
	//load spaceship from file
	//"D:\programming\programming\C Programming\3d Graphics\3dEnginev1\res\mountains.obj.txt"
	loadMeshFromFile("./res/mountains.obj.txt", &obj_spaceship);
	
	//create spaceship model
	memset(&mod_spaceship, 0, sizeof(mod_spaceship));
	mod_spaceship.mesh = &obj_spaceship;
	mod_spaceship.position.x = -0.5;
	mod_spaceship.position.y = 0;
	mod_spaceship.position.z = 15;
	mod_spaceship.scale.x = 1;
	mod_spaceship.scale.y = 1;
	mod_spaceship.scale.z = 1;
	mod_spaceship.rotation.x = 0;
	mod_spaceship.rotation.y = 0;
	mod_spaceship.rotation.x = 0;
	
	//create spaceship model
	memset(&mod_spaceship2, 0, sizeof(mod_spaceship));
	mod_spaceship2.mesh = &obj_spaceship;
	mod_spaceship2.position.x = -0.5;
	mod_spaceship2.position.y = -10;
	mod_spaceship2.position.z = 15;
	mod_spaceship2.scale.x = 1;
	mod_spaceship2.scale.y = 0.5;
	mod_spaceship2.scale.z = 1;
	mod_spaceship2.rotation.x = 0;
	mod_spaceship2.rotation.y = 0;
	mod_spaceship2.rotation.x = PI / 2;
};

//initialise
void initialise(){
	/*
		Calculate fov distance
		
		fov distance = adjacent
		viewport width / 2 = opposite
		fov / 2 = angle
		
		fovDistance = (VIEWPORT_WIDTH / 2.f) / tan(fov / 2)
	*/
	fovDistance = (float) (VIEWPORT_WIDTH / 2.f) / tan((float) fov / 2);
	
	//initialise models
	initialiseModels();
	
	srand(time(0));
	
	testx1 = rand() % BUFFER_WIDTH - BUFFER_WIDTH / 2;
	testx2 = rand() % BUFFER_WIDTH - BUFFER_WIDTH / 2;
	testx3 = rand() % BUFFER_WIDTH - BUFFER_WIDTH / 2;
	
	testy1 = rand() % BUFFER_HEIGHT - BUFFER_HEIGHT / 2;
	testy2 = rand() % BUFFER_HEIGHT - BUFFER_HEIGHT / 2;
	testy3 = rand() % BUFFER_HEIGHT - BUFFER_HEIGHT / 2;
	
	//set camera coordinates
	camera.position.x = 0;
	camera.position.y = 0;
	camera.position.z = 0;
	
	camera.rotation.x = 0;
	camera.rotation.y = 0;
	camera.rotation.z = 0;
	
	//create lights
	lights[0].alive = 1;
	lights[0].type = AMBIENT_LIGHT;
	lights[0].intensity = 0.4;
	
	lights[1].alive = 1;
	lights[1].type = POINT_LIGHT;
	lights[1].intensity = 0.6;
	lights[1].direction.x = -0.5;
	lights[1].direction.y = 0.5;
	lights[1].direction.z = 0;
};

//vector functions
float magnitudeVector4f(Vector4f v1){
	return sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
};

Vector4f addVector4f(Vector4f v1, Vector4f v2){
	//add components
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	
	/*
		Homogenous coordinates are used here.
		w is > 0 when the Vector4f represents a
		point, and is <= 0 when the Vector4f represents
		a directional vector.
		
		Two points added together will produce a point
		(i.e. 1 + 1 = 2)
		
		A point added with a vector will produce a point
		(i.e. 1 + 0 = 1)
		
		Two vectors added will produce a resultant vector
		(i.e. 0 + 0 = 0)
	*/
	v1.w += v2.w;
	
	return v1;
};

Vector4f subtractVector4f(Vector4f v1, Vector4f v2){
	//subtract components
	v1.x -= v2.x;
	v1.y -= v2.y;
	v1.z -= v2.z;
	v1.w -= v2.w;
	
	return v1;
};
	
Vector4f scalarProductVector4f(Vector4f v1, float scalar){
	v1.x *= scalar;
	v1.y *= scalar;
	v1.z *= scalar;
	
	return v1;
};

float dotProductVector4f(Vector4f v1, Vector4f v2){
	/*
		Dot product identity:
		<A, B> = Ax * Bx + Ay * By + Az * Bz
		= |A| * |B| * cos(a)
	*/
	
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
};

Vector4f crossProduct(Vector4f v1, Vector4f v2){
	/*
		The cross product returns a vector perpendicular to the two vectors
		
	*/
	
	Vector4f normal; 
	normal.x = v1.y * v2.z - v1.z * v2.y;
	normal.y = v1.x * v2.z - v1.z * v2.x;
	normal.z = v1.x * v2.y - v1.y * v2.x;
	normal.w = 0; //this is a vector, not a point
	
	return normal;
};

//matrix functions
Matrix4x4 initialiseMatrix4x4(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p){
	/*
		Note: each sub-array represents a row
		e.g. mat.data[0] is the first row,
		mat.data[1] is the second row, etc.
	*/
	
	Matrix4x4 mat;
	
	mat.data[0][0] = a;
	mat.data[0][1] = b;
	mat.data[0][2] = c;
	mat.data[0][3] = d;
	
	mat.data[1][0] = e;
	mat.data[1][1] = f;
	mat.data[1][2] = g;
	mat.data[1][3] = h;
	
	mat.data[2][0] = i;
	mat.data[2][1] = j;
	mat.data[2][2] = k;
	mat.data[2][3] = l;
	
	mat.data[3][0] = m;
	mat.data[3][1] = n;
	mat.data[3][2] = o;
	mat.data[3][3] = p;

	return mat;
};

Matrix4x4 scalarProductMatrix4x4(Matrix4x4 mat, float scalar){
	int i;
	
	for(i = 0; i < 16; i++){
		mat.data[i / 4][i % 4] *= scalar;
	};
	
	return mat;
};

Vector4f matrix4x4Vectorf4fProduct(Matrix4x4 mat, Vector4f vec){
	Vector4f resultant;
	
	resultant.x = mat.data[0][0] * vec.x + mat.data[0][1] * vec.y + mat.data[0][2] * vec.z + mat.data[0][3] * vec.w;
	resultant.y = mat.data[1][0] * vec.x + mat.data[1][1] * vec.y + mat.data[1][2] * vec.z + mat.data[1][3] * vec.w;
	resultant.z = mat.data[2][0] * vec.x + mat.data[2][1] * vec.y + mat.data[2][2] * vec.z + mat.data[2][3] * vec.w;
	resultant.w = mat.data[3][0] * vec.x + mat.data[3][1] * vec.y + mat.data[3][2] * vec.z + mat.data[3][3] * vec.w;
	
	if(resultant.w > 0){
		resultant.w = 1;
	} else {
		resultant.w = 0;
	};
	
	return resultant;
};

Matrix4x4 matrix4x4Product(Matrix4x4 mat1, Matrix4x4 mat2){
	Matrix4x4 resultant;
	
	int i;
	int j;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 4; j++){
			resultant.data[i][j] = mat1.data[i][0] * mat1.data[0][j] + mat1.data[i][1] * mat1.data[1][j] + mat1.data[i][2] * mat1.data[2][j] + mat1.data[i][3] * mat1.data[3][j];
		};
	};
	
	return resultant;
};

//mathematical functions
//viewport to canvas
Vector2f viewportToCanvas(Vector2f v){
	Vector2f resultant;
	resultant.x = ((float) v.x / (float) VIEWPORT_WIDTH) * (float) BUFFER_WIDTH;
	resultant.y = ((float) v.y / (float) VIEWPORT_HEIGHT) * (float) BUFFER_HEIGHT;
	return resultant;
};

//sign
float sign(float num){
	if(num > 0){
		return 1;
	} else if(num < 0){
		return -1;
	} else {
		return 0;
	};
};

//get pixel z
float getPixelZ(float x, float y){
	if(x > -BUFFER_WIDTH / 2 && x < BUFFER_WIDTH / 2 && y > -BUFFER_HEIGHT / 2 && y < BUFFER_HEIGHT / 2){
		return renderBuffer.pixelZ[(int) ((y + BUFFER_HEIGHT / 2) * BUFFER_WIDTH + x + BUFFER_WIDTH / 2)];
	} else {
		return 9999999;
	};
};

//set pixel z
void setPixelZ(float x, float y, float z){
	if(x > -BUFFER_WIDTH / 2 && x < BUFFER_WIDTH / 2 && y > -BUFFER_HEIGHT / 2 && y < BUFFER_HEIGHT / 2){
		renderBuffer.pixelZ[(int) ((y + BUFFER_HEIGHT / 2) * BUFFER_WIDTH + x + BUFFER_WIDTH / 2)] = z;
	};
};

//primitive rendering functions
//draw pixel
void drawPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue){
	//check
	if(x > -BUFFER_WIDTH / 2 && x < BUFFER_WIDTH / 2 && y > -BUFFER_HEIGHT / 2 && y < BUFFER_HEIGHT / 2){
		Pixel * pixel = renderBuffer.pixels + ((y + BUFFER_HEIGHT / 2) * BUFFER_WIDTH) + x + BUFFER_WIDTH / 2;
		pixel->red = red;
		pixel->green = green;
		pixel->blue = blue;
	};
};

//draw line
void drawLine(int x1, int y1, int x2, int y2, uint8_t red, uint8_t green, uint8_t blue){
	float dx = (float) x2 - x1;
	float dy = (float) y2 - y1;
	
	//x and y increment
	float xInc;
	float yInc;
	int numberOfSteps;
	
	//check if dy > dx
	if(abs(dy) > abs(dx)){
		numberOfSteps = abs(dy);
		
		xInc = dx / numberOfSteps;
		yInc = sign(dy);
	} else {
		numberOfSteps = abs(dx);
		
		xInc = sign(dx);
		yInc = dy / numberOfSteps;
	};
	
	//set coordnates
	float currentX = x1;
	float currentY = y1;
	
	//create start x and end x
	int i;
	
	for(i = 0; i < numberOfSteps; i++){
		//draw pixel
		drawPixel((int) currentX, (int) currentY, red, green, blue);
		
		//increment pixels
		currentX += xInc;
		currentY += yInc;
	};
};

//draw triangle
void drawTriangle(int x1, int y1, float z1, float i1, int x2, int y2, float z2, float i2, int x3, int y3, float z3, float i3, uint8_t red, uint8_t green, uint8_t blue){
	//sort points
	if(y2 < y1){
		int temp = x2;
		x2 = x1;
		x1 = temp;
		
		temp = y2;
		y2 = y1;
		y1 = temp;
		
		float temp2 = z2;
		z2 = z1;
		z1 = temp2;
		
		temp2 = i2;
		i2 = i1;
		i1 = temp2;
	};
	
	if(y3 < y2){
		int temp = x3;
		x3 = x2;
		x2 = temp;
		
		temp = y3;
		y3 = y2;
		y2 = temp;
		
		float temp2 = z3;
		z3 = z2;
		z2 = temp2;
		
		temp2 = i3;
		i3 = i2;
		i2 = temp2;
	};
	
	if(y2 < y1){
		int temp = x2;
		x2 = x1;
		x1 = temp;
		
		temp = y2;
		y2 = y1;
		y1 = temp;
		
		float temp2 = z2;
		z2 = z1;
		z1 = temp2;
		
		temp2 = i2;
		i2 = i1;
		i1 = temp2;
	};
	
	//calculate
	int l12Dy = y2 - y1;
	int l13Dy = y3 - y1;
	int l23Dy = y3 - y2;
	
	//check if triangle has non-zero height
	if(l13Dy != 0){
		int i;
		
		//create x and y
		float l12X = (float) x1;
		float l13X = (float) x1;
		float l23X = (float) x2;
		
		int minX;
		int maxX;
		
		//create intensity variables
		float l12I = i1;
		float l13I = i1;
		float l23I = i2;
		
		float leftI;
		float rightI;
		
		//create z variables
		float l12Z = z1;
		float l13Z = z1;
		float l23Z = z2;
		
		float leftZ;
		float rightZ;
	
		//calculate l13xInc - change in x per unit y
 		float l13xInc = (float) (x3 - x1) / l13Dy;
		
		//calculate l13iInc - change in intensity per unit y
		float l13iInc = (float) (i3 - i1) / l13Dy;
		
		//calculate l13zInc - change in z per unit y
		float l13zInc = (float) (z3 - z1) / l13Dy;
		
		//check if first half of triangle has a height > 0
		if(l12Dy != 0){
			//calculate l12xInc  - change in x per unit y
			float l12xInc = (float) (x2 - x1) / l12Dy;
			
			//calculate l12iInc - change in intensity per unit y
			float l12iInc = (float) (i2 - i1) / l12Dy;
			
			//calculate l12zInc - change in intensity per unit y
			float l12zInc = (float) (z2 - z1) / l12Dy;
			
			//draw all rows of first half of triangle
			for(i = y1; i < y2; i++){
				//calculate minx and max
				if(l12X <= l13X){
					minX = (int) l12X;
					maxX = (int) l13X;
				
					leftI = l12I;
					rightI = l13I;
					
					leftZ = l12Z;
					rightZ = l12Z;
				} else {
					minX = (int) l13X;
					maxX = (int) l12X;
					
					leftI = l13I;
					rightI = l12I;
					
					leftZ = l13Z;
					rightZ = l12Z;
				};
				
				float horizontalIIncrement = (rightI - leftI) / (maxX - minX);
				float horizontalZIncrement = (rightZ - leftZ) / (maxX - minX);
								
				int j;
				for(j = minX; j <= maxX; j++){
					//check z coordinate
					if(leftZ <= getPixelZ(j, i)){
						setPixelZ(j, i, leftZ);
						drawPixel(j, i, (uint8_t)((float) red * leftI), (uint8_t)((float) green * leftI), (uint8_t) ((float) blue * leftI));
					};
					
					leftI += horizontalIIncrement;
					leftZ += horizontalZIncrement;
				};
				
				//increment x coordinates
				l12X += l12xInc;
				l13X += l13xInc;
				
				l12I += l12iInc;
				l13I += l13iInc;
				
				l12Z += l12zInc;
				l13Z += l13zInc;
			};
		};
		
		//set start of line connecting point 2->3
		l23X = x2;	
		
		//check if second half has a height > 0
		if(l23Dy != 0){
			//calculate l23xInc
			float l23xInc = (float) (x3 - x2) / l23Dy;
			
			//calculate l23iInc - change in intensity per unit y
			float l23iInc = (float) (i3 - i2) / l23Dy;
			
			//calculate l23zInc - change in intensity per unit y
			float l23zInc = (float) (z3 - z2) / l23Dy;
			
			//draw all rows of first half of triangle
			for(i = y2; i < y3; i++){
				//calculate minx and max
				if(l23X <= l13X){
					minX = (int) l23X;
					maxX = (int) l13X;
					
					leftI = l23I;
					rightI = l13I;
					
					leftZ = l23Z;
					rightZ = l13Z;
				} else {
					minX = (int) l13X;
					maxX = (int) l23X;
					
					leftI = l13I;
					rightI = l23I;
					
					leftZ = l13Z;
					rightZ = l23Z;
				};
				
				float horizontalIIncrement = (rightI - leftI) / (maxX - minX);
				float horizontalZIncrement = (rightZ - leftZ) / (maxX - minX);
				
				int j;
				for(j = minX; j <= maxX; j++){
					//check z coordinate
					if(leftZ <= getPixelZ(j, i)){
						setPixelZ(j, i, leftZ);
						drawPixel(j, i, (uint8_t)((float) red * leftI), (uint8_t)((float) green * leftI), (uint8_t) ((float) blue * leftI));
					};
					
					leftI += horizontalIIncrement;
					leftZ += horizontalZIncrement;
				};
				
				//increment x coordinates
				l23X += l23xInc;
				l13X += l13xInc;
				
				l23I += l23iInc;
				l13I += l13iInc;
				
				l23Z += l23zInc;
				l13Z += l13zInc;
			};
		};
	};
};

//draw wireframe triangle
void drawWireframeTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t red, uint8_t green, uint8_t blue){
	drawLine(x1, y1, x2, y2, red, green, blue);
	drawLine(x2, y2, x3, y3, red, green, blue);
	drawLine(x1, y1, x3, y3, red, green, blue);
};

//project coordinates
Vector2f projectCoordinates(Vector4f v){
	/*
		vy / (z + s) = vy' / s
		vy' = vy * s / (z + s)
	*/
	
	float ratio = fovDistance / (v.z + fovDistance);
	
	Vector2f resultant;
	resultant.x = v.x * ratio;
	resultant.y = v.y * ratio;
	
	return resultant;
};

//rotate 2d
Vector2f rotate2dPoint(float x, float y, float angle){
	/*
		Rotation in 2d:
		x = lcos(a)
		y = lsin(a)
		
		rx = lcos(a + b)
		ry = lsin(a + b)
		
		rx = lcos(a)cos(b) - lsin(a)sin(b)
		ry = lsin(a)cos(b) + lsin(b)cos(a)
		
		rx = xcos(b) - ysin(b)
		ry = ycos(b) + xsin(b)
	*/
	
	//pre-calculate sin and cos of angle
	float sinA = sin(angle);
	float cosA = cos(angle);
	
	Vector2f resultant;
	resultant.x = x * cosA - y * sinA;
	resultant.y = y * cosA+ x * sinA;
	
	return resultant;
};

//rotate point
Vector4f rotatePoint(Vector4f v1, Vector4f rotation){
	//rotate in yz plane
	Vector2f v2 = rotate2dPoint(v1.z, v1.y, rotation.y);
	v1.z = v2.x;
	v1.y = v2.y;
	
	//rotate in xy plane
	v2 = rotate2dPoint(v1.x, v1.y, rotation.x);
	v1.x = v2.x;
	v1.y = v2.y;
	
	//rotate in xz plane
	v2 = rotate2dPoint(v1.x, v1.z, rotation.z);
	v1.x = v2.x;
	v1.z = v2.y;
	
	return v1;
};

//scale triangle
Triangle scaleTriangle(Triangle triangle, Vector4f scale){
	triangle.vertices[0].x *= scale.x;
	triangle.vertices[0].y *= scale.y;
	triangle.vertices[0].z *= scale.z;
	
	triangle.vertices[1].x *= scale.x;
	triangle.vertices[1].y *= scale.y;
	triangle.vertices[1].z *= scale.z;
	
	triangle.vertices[2].x *= scale.x;
	triangle.vertices[2].y *= scale.y;
	triangle.vertices[2].z *= scale.z;
	
	return triangle;
};

//rotate triangle
Triangle rotateTriangle(Triangle triangle, Vector4f rotation){
	triangle.vertices[0] = rotatePoint(triangle.vertices[0], rotation);
	triangle.vertices[1] = rotatePoint(triangle.vertices[1], rotation);
	triangle.vertices[2] = rotatePoint(triangle.vertices[2], rotation);
	return triangle;
};

//translate triangle
Triangle translateTriangle(Triangle triangle, Vector4f translation){
	triangle.vertices[0].x += translation.x;
	triangle.vertices[0].y += translation.y;
	triangle.vertices[0].z += translation.z;
	
	triangle.vertices[1].x += translation.x;
	triangle.vertices[1].y += translation.y;
	triangle.vertices[1].z += translation.z;
	
	triangle.vertices[2].x += translation.x;
	triangle.vertices[2].y += translation.y;
	triangle.vertices[2].z += translation.z;
	
	return triangle;
};

//draw 3d triangle
void render3dTriangle(Triangle triangle){
	//project each vertex
	Vector2f screenCoords[3];
	
	//out of frame
	int outOfFrame = 0;
	
	int i;
	for(i = 0; i < 3; i++){
		//clip triangles out of frame (basic clipping)
		if(triangle.vertices[i].z > 0){
			screenCoords[i] = viewportToCanvas(projectCoordinates(triangle.vertices[i]));
		} else {
			outOfFrame = 1;
		};
	};
	
	//out of frame
	if(!outOfFrame){
		//reset intensities
		triangle.intensities[0] = 0;
		triangle.intensities[1] = 0;
		triangle.intensities[2] = 0;
		
		//apply lighting
		int j;
		for(i = 0; i < LIGHT_COUNT; i++){
			//check if light is active
			if(lights[i].alive == 1){
				//check type
				if(lights[i].type == AMBIENT_LIGHT){
					//add intensity to all vertices
					for(j = 0; j < 3; j++){
						triangle.intensities[j] += lights[i].intensity;
					};
				} else {
					//calculate surface normal 
					Vector4f v1 = subtractVector4f(triangle.vertices[1], triangle.vertices[0]);
					Vector4f v2 = subtractVector4f(triangle.vertices[2], triangle.vertices[0]);
					Vector4f normal = crossProduct(v2, v1);
					
					//iterate through each point
					for(j = 0; j < 3; j++){
						//calcuate light vector
						Vector4f lightVector;
						
						//check light type
						if(lights[i].type == DIRECTIONAL_LIGHT){
							//direcitonal light
							lightVector = lights[i].direction;
							printf("B");
						} else {
							//point light
							lightVector = subtractVector4f(triangle.vertices[j], lights[i].position);
						};
						
						//calculate intensity
						triangle.intensities[j] += dotProductVector4f(lightVector, normal) / (magnitudeVector4f(lightVector) * magnitudeVector4f(normal)) * lights[i].intensity;
					};
				};
			};
		};
		
		//correct light intensities
		for(i = 0; i < 3; i++){
			if(triangle.intensities[i] > 1){
				triangle.intensities[i] = 1;
			} else if(triangle.intensities[i] < 0){
				triangle.intensities[i] = 0;
			};
		};
		
		//draw triangles
		drawTriangle(screenCoords[0].x, screenCoords[0].y, triangle.vertices[0].z, triangle.intensities[0], screenCoords[1].x, screenCoords[1].y, triangle.vertices[1].z, triangle.intensities[1], screenCoords[2].x, screenCoords[2].y, triangle.vertices[2].z, triangle.intensities[2], triangle.colour.red, triangle.colour.green, triangle.colour.blue);
		
		//draw wireframe triangle
		//drawWireframeTriangle(screenCoords[0].x, screenCoords[0].y, screenCoords[1].x, screenCoords[1].y, screenCoords[2].x, screenCoords[2].y, triangle.colour.red, triangle.colour.green, triangle.colour.blue);
	};
};

//render model
void renderModel(Model * model){
	//draw each triangle
	int i;
	
	for(i = 0; i < model->mesh->trianglesCount; i++){
		//create render triangle
		Triangle renderTriangle = model->mesh->triangles[i];
		
		//scale triangle
		renderTriangle = scaleTriangle(renderTriangle, model->scale);
		
		//rotate triangle
		renderTriangle = rotateTriangle(renderTriangle, model->rotation);
		
		//translate triangle
		renderTriangle = translateTriangle(renderTriangle, model->position);
		
		//translate with respect to camera
		renderTriangle = translateTriangle(renderTriangle, scalarProductVector4f(camera.position, -1));
		
		//rotate
		renderTriangle = rotateTriangle(renderTriangle, scalarProductVector4f(camera.rotation, -1));
		
		//check if triangle is facing the camera
		Vector4f cameraVec = scalarProductVector4f(renderTriangle.vertices[0], -1);
		Vector4f sideVec = subtractVector4f(renderTriangle.vertices[1], renderTriangle.vertices[0]);
		Vector4f sideVec2 = subtractVector4f(renderTriangle.vertices[2], renderTriangle.vertices[0]);
		Vector4f normalVec = crossProduct(sideVec2, sideVec);
		normalVec.y *= -1;
		
		//only draw if dot product (and thus cos of angle) of normal vector and camera vector is > 0
		if(dotProductVector4f(normalVec, cameraVec) < 0){
			//render triangle
			render3dTriangle(renderTriangle);
		};
	};
};

//point towards
void pointTowards(Vector4f p){
	p = subtractVector4f(p, camera.position);
	camera.rotation.x = -atan2(p.y, p.x);
	camera.rotation.y = -atan2(p.y, p.z);
	camera.rotation.z = -atan2(p.x, p.z);
};

/*
	Entry point
	-In Win32, the entry point is WinMain, not main
	
	-Like main, WinMain returns an int
	
	-The calling convention of the function is called WINAPI (you do not need to know what this means as it isn't used elsewhere)
	
	-The calling convention of a function determines how it stores data on the program's call stack
	
	-Note that all parameters are passed into the function by the OS
	
	-On Windows, a handle is a numerical identifier used to identify objects and structures controlled by the OS. Handles pop up quite frequently.
	
	-A HINSTANCE is a handle to an application instance - the first parameter is the numerical identifier for a specific instance of
	the program, given to it by the OS at runtime
	
	-The second parameter, hPrevInstace is a feature that is now useless. It served a purpose on 16-bit Windows versions, but now
	must simply be included for compatiblity's sake (each version of Windows mostly aims to be compatible with the previous versions)
	
	-The third parameter is the command line arguments as a single string. LPSTR stands for long pointer to string, and is simply just
	a redefinition of a char * on modern systems.
	
	-Note that long pointers are also a redundant feature from 16-bit windows, when pointers could either be 16-bit or 32-bit. Nowadays,
	a long pointer and a pointer are the same on Windows.
	
	-The fourth parameter is an integer used to specify how the program should be displayed. Again, this is given to us by the OS - we
	do not set this, as we do not set any of the parameters of WinMain.
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstace, LPSTR cmd, int nCmdShow){
	//create window class structure and initialise to 0
	WNDCLASSEX windowClass = {0};
	
	//set window class properties 
	//set size of structure
	//cbSize is short for "count bytes size" - it is the size of the structure in bytes
	windowClass.cbSize = sizeof(WNDCLASSEX);
	
	//set class name - this is a string used by the operating system to identify our window class
	//lpszClassName is short for "long pointer to string class name"
	windowClass.lpszClassName = "MAIN_WINDOW_CLASS";
	
	//set window style - this is the default behaviour of the windows. We are telling the window to redraw when either horizontally or vertically resized (or both)
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	
	//set window procedure - this is the event handler function, and is declared at the top of the file
	//lpfnWndProc is short for "long pointer to function window procedure", and is used to identify the window procedure (event handler function) for this window class 
	windowClass.lpfnWndProc = windowProcedure;
		
	/*
	set background colour - we set a handle to a colour we create.
	A brush is just a drawing style stored on the OS. Here, we are
	asking the OS to create a solid colour "brush" and return a
	handle to it so that we can use it.
	*/
	windowClass.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(100, 100, 100));
	
	//send window class to operating system to be registered
	RegisterClassEx(&windowClass);
	
	//create window size structure
	//note that here, we specify the size of the client area, i.e. the area we can draw to
	RECT windowSize = {0, 0, BUFFER_WIDTH, BUFFER_HEIGHT};
	
	//get window frame size
	//AdjustWindowRectEx takes a client area, and adds the size of the window frame to it
	AdjustWindowRectEx(
		&windowSize, //pointer to client area rectangle to be modified
		WS_OVERLAPPEDWINDOW, //window style(behaviour) - WS_OVERLAPPEDWINDOW is the default
		0, //no menu bar (this is a boolean value)
		WS_EX_OVERLAPPEDWINDOW	//extended style (behaviour) - WS_EX_OVERLAPPEDWINDOW is the default
	);
	
	//windowSize now contains the size of the window (including the window frame)
	
	//create window from window class
	//a HWND is a window handle
	renderBuffer.windowHandle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW, //default extended window style - a basic window that may be overlapped
		windowClass.lpszClassName, //the class name of the window class this window will use
		"My First Window!!!", //the title text of the window
		WS_OVERLAPPEDWINDOW, //default window style - a basic window that may be overlapped
		CW_USEDEFAULT, CW_USEDEFAULT, //the starting x and y coordinates of the window - use default
		windowSize.right - windowSize.left, //width of window frame
		windowSize.bottom - windowSize.top, //height of window frame
		NULL, //no parent window
		NULL, //no menu bar - we will not need one for our window
		hInstance, //application instance that window belongs to
		NULL //no LPARAM (an LPARAM is an additional piece of data in Win32)
	);

	//check if window created successfully
	if(!renderBuffer.windowHandle){
		//window failed to create - display an alert
		MessageBox(
			NULL, //no parent window - this should display on its own
			"Error - could not create window", //inner text
			"Window Error", //the title text of the message box
			MB_OK | MB_ICONERROR //flags - message box has an OK button, message box has an error icon
		);
		
		return -1;
	};
	
	//allocate memory for render buffer pixels
	renderBuffer.pixels = (Pixel *) malloc(BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(Pixel));
	
	//z buffer
	renderBuffer.pixelZ = (float *) malloc(BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(float));
	
	//check if memory could not be allocated - terminate program if so
	if(!renderBuffer.pixels){
		//failed to allocate memory for render buffer
		MessageBox(
			NULL, //no parent window - this should display on its own
			"Error - could not allocate memory for render buffer", //inner text
			"Render Buffer Error", //the title text of the message box
			MB_OK | MB_ICONERROR //flags - message box has an OK button, message box has an error icon
		);
		
		return -1;
	};
	
	/*
		Get window device context and set renderBuffer context to it.
		The window's device context is the region of memory that is rendered to it.
		This is managed by the OS, so we need to retrieve and handle to it. We do this
		so that we can use the StretchDIBits function to send our buffer data to said region
		of memory, as we cannot simply write to the region ourselves.
	*/
	renderBuffer.deviceContextHandle = GetDC(renderBuffer.windowHandle);
	
	//set all bitmap info properties to 0
	memset(&renderBuffer.bitmapInfo.bmiHeader, 0 , sizeof(BITMAPINFOHEADER));
	
	/*
		Fill out bitmapinfo structure for renderBuffer.
		This is necessary so that when we wish to send our data to the window's device context
		the OS knows how to interpret our data.
		
		Remeber that our buffer is really just a large bitmap.
		
		The BITMAPINFO structure is made up of two parts: the bitmap info header and the colour
		table. The bitmap info header contains all of the metadata we must set before we can
		draw our buffer data to the screen. We do not need to worry about the colour table, as
		it is mostly only used for defining colour codes for 16-bit or 8-bit colour palettes.
	*/
	renderBuffer.bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); //the size of the BITMAPINFOHEADER structure in bytes
	renderBuffer.bitmapInfo.bmiHeader.biWidth = BUFFER_WIDTH; //the width of our buffer in pixels
	renderBuffer.bitmapInfo.bmiHeader.biHeight = BUFFER_HEIGHT; //the height of our buffer in pixels
	renderBuffer.bitmapInfo.bmiHeader.biPlanes = 1; //this is the number of planes to render - this has to be set to 1
	renderBuffer.bitmapInfo.bmiHeader.biBitCount = 24; //we are using 24-bit colours
	renderBuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB; //uncompressed - we are simply using 3 bytes for the RGB values of each pixel
	
	//there are other properties of the BITMAPINFOHEADER structure, but we can set these all to 0 as they are not relevant to us
	 
	//initialise engine
	initialise();
	
	//show window if window created successfully
	//this function takes the window handle and an integer indicating how it should be shown
	//this integer is the nCmdShow parameter passed by the OS upon starting our program
	ShowWindow(renderBuffer.windowHandle, nCmdShow);
	
	//start program loop
	mainLoop();
	
	return 0;
};


/*
	mainLoop
	-The job of this function is to iterate until the "running" global variable is set to 0
	-Each iteration, it should handle any events and render the current data to the screen
*/
void mainLoop(){
	//iterate while window is running
	while(running){
		//start time
		QueryPerformanceCounter(&gameClock.startTime);
		
		//handle events for global render buffer
		handleEvents();
	
		//render to global render buffer
		render();
		
		//get clock frequency
		QueryPerformanceFrequency(&gameClock.clockFrequency);
		
		//end time
		QueryPerformanceCounter(&gameClock.endTime);
		
		//calculate delta time
		gameClock.deltaTime = (float) (gameClock.endTime.QuadPart - gameClock.startTime.QuadPart) / (gameClock.clockFrequency.QuadPart);
	};
};

/*
	handleEvents
	-The job of this function is to handle all events for the window that the global render buffer belongs to
	-All messages on the message queue are processed by using the PeekMessage function in a while loop
*/
void handleEvents(){
	//create message structure to store incoming event
	//remember "message" is just the Windows name for event
	MSG message;
	
	//start message loop
	/*
		The PeekMessage function takes five parameters
		-The first is a pointer to a message structure to fill out
		-The second is the window to get events for. This can be
		set to NULL to detect all events that occur on the system.
		-The third is the minimum message ID to retrieve
		-The fourth is the maximum message ID to retreive
		-Note that by setting both the minimum and maximum
		message IDs to 0, the PeekMessage call will detect all
		messages and thus override the minimum-maximum range
		setting
		-The fifth is for flags. We want to add the PM_REMOVE
		flag, which will remove our message from the system's
		message queue once processed
		
		Note that PeekMessage will not block, so if no messages are
		present on the system's message queue, the loop simply just ends.
	*/
	
	//get all messages
	while(PeekMessage(&message, renderBuffer.windowHandle, 0, 0, PM_REMOVE)){
		/*
			Translate the message.
			This involves converting key codes into characters for text-based events
			we shouldn't need it, but it is a good practice to include it anyway,
			as if we decide to use a message requiring translation later and forget
			this line, the resulting errors may be hard to debug
		*/
		TranslateMessage(&message);
		
		//dispatch message - send message and its corresponding window to the relevant window procedure
		DispatchMessage(&message);
	};
};

//render function
void render(){
	//set all pixels to 0 red, 0 blue, 0 green
	memset(renderBuffer.pixels, 0, BUFFER_WIDTH * BUFFER_HEIGHT * sizeof(Pixel));
	
	//reset z buffer
	int i;
	
	for(i = 0; i < BUFFER_WIDTH * BUFFER_HEIGHT; i++){
		renderBuffer.pixelZ[i] = 999999;
	};
	
	//draw triangle
	renderModel(&mod_spaceship);
	//renderModel(&mod_spaceship2);
	
	if(GetAsyncKeyState(VK_LEFT)){
		camera.rotation.z += 4.f * gameClock.deltaTime;	
	};
	
	if(GetAsyncKeyState(VK_RIGHT)){
		camera.rotation.z -= 4.f * gameClock.deltaTime;	
	};
	
	if(GetAsyncKeyState(VK_UP)){
		camera.rotation.y += 4.f * gameClock.deltaTime;	
	};
	
	if(GetAsyncKeyState(VK_DOWN)){
		camera.rotation.y -= 4.f * gameClock.deltaTime;	
	};
	
	if(GetAsyncKeyState(VK_SPACE)){
		//pointTowards(mod_spaceship.position);
		camera.position.y += 4 * gameClock.deltaTime;
	};
	
	if(GetAsyncKeyState(VK_LSHIFT)){
		//pointTowards(mod_spaceship.position);
		camera.position.y -= 4 * gameClock.deltaTime;
	};
	
	if(GetAsyncKeyState('W')){
		camera.position.z += cos(camera.rotation.z) * 8 * gameClock.deltaTime;
		camera.position.x -= sin(camera.rotation.z) * 8 * gameClock.deltaTime;
	};
	
	if(GetAsyncKeyState('S')){
		camera.position.z -= cos(camera.rotation.z) * 8 * gameClock.deltaTime;
		camera.position.x += sin(camera.rotation.z) * 8 * gameClock.deltaTime;
	};
	
	if(GetAsyncKeyState('A')){
		camera.position.z += cos(camera.rotation.z + PI / 2) * 8 * gameClock.deltaTime;
		camera.position.x -= sin(camera.rotation.z + PI / 2) * 8 * gameClock.deltaTime;
	};
	
	if(GetAsyncKeyState('D')){
		camera.position.z += cos(camera.rotation.z - PI / 2) * 8 * gameClock.deltaTime;
		camera.position.x -= sin(camera.rotation.z - PI / 2) * 8 * gameClock.deltaTime;
	};

	//printf("\n-----\nPosition: (%f, %f, %f)\n", camera.position.x, camera.position.y, camera.position.z);
	//printf("Rotation: (%f, %f, %f)\n-----\n", camera.rotation.x, camera.rotation.y, camera.rotation.z);
	
	/*
		Send renderbuffer data to client area of window.
		We can do this with the StretchDIBits function.
		This takes many parameters, which are detailed below:
	*/
	StretchDIBits(
		renderBuffer.deviceContextHandle, //a handle to the device context we wish to render to
		renderBuffer.windowClientWidth / 2 - (renderBuffer.scale * BUFFER_WIDTH) / 2, //the x coordinate of the top left coordinate of our buffer on the window client area
		renderBuffer.windowClientHeight / 2 - (renderBuffer.scale * BUFFER_HEIGHT) / 2, //the y coordinate of the top left coordinate of our buffer on the window client area
		BUFFER_WIDTH * renderBuffer.scale, //the width of the buffer on the window client area
		BUFFER_HEIGHT * renderBuffer.scale, //the height of the buffer on the window client area
		0, //the starting x coordinate on the source buffer to render from (we want to render all data, so this is 0)
		0, //the starting y coordinate on the source buffer to render from (we want to render all data, so this is 0)
		BUFFER_WIDTH, //the width of the source buffer
		BUFFER_HEIGHT, //the height of the source buffer
		renderBuffer.pixels, //a pointer to the actual data we want to send
		&renderBuffer.bitmapInfo, //a pointer to the bitmap info structure for our renderBuffer
		DIB_RGB_COLORS, //use RGB colours
		SRCCOPY //copy the source into the window's buffer
	);
};

/*
	Window procedure - this is the window procedure, the event handler function.
	It takes four parameters:
	-The handle (numerical ID) to the window calling the function
	-The message ID (indicating what type of event has occurred)
	-wParam, a general purpose parameter used to store event-dependent data
	-lParam, another general purpose parameter
	
	Note that it is common to see WPARAM and LPARAM pop up in many Win32 functions.
	Remember that these are just general purpose parameters, and typically
	contain integer values or pointers
	
	Note that an LRESULT is just a "long long int" type.
*/
LRESULT CALLBACK windowProcedure(HWND windowHandle, UINT messageID, WPARAM wParam, LPARAM lParam){
	//check event type/ID
	switch(messageID){
		//window is closed (i.e. X button is clicked)
		case WM_CLOSE:{
			//send quit message to close window
			PostQuitMessage(0);
			
			//set running to 0 to close program
			running = 0;
			return 0;
		};
		
		//window is resized
		case WM_SIZE:{
			//get window client area size
			RECT windowClientRect;
			GetClientRect(windowHandle, &windowClientRect);
			
			//set window width and height in renderBuffer structure
			renderBuffer.windowClientWidth = windowClientRect.right - windowClientRect.left;
			renderBuffer.windowClientHeight = windowClientRect.bottom - windowClientRect.top;
			
			//calculate the maximum scale that the renderBuffer can be increased by so that it fits in the window
			int i = 1;
			
			while(BUFFER_WIDTH * i <= renderBuffer.windowClientWidth && BUFFER_HEIGHT * i <= renderBuffer.windowClientHeight){
				//increment i
				i += 1;
			};
			
			//subtract 1 from i, since i is equal to the first integer scale where the buffer extends out of the bounds of the window and we want the largest scale before this
			i -= 1;
			
			//set buffer scale to i
			renderBuffer.scale = i;
			
			break;
		};
	};
	
	/*
		There are thousands of different types of message
		on Windows. We cannot check for them all, so we
		can simply call the default window procedure on all
		our parameters for the vast majority of events
	*/
	
	//return default window procedure
	return DefWindowProc(windowHandle, messageID, wParam, lParam);
};