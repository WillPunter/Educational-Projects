/*
	Space Arena Shooter Game
	By Will Punter
	
	The game will be software-rendered and written in Win32.
	
	This game is a top-down space shooter, where the player must fight enemies in an arena.
	It will have a campaign (a story-based mode) and an endless arcade mode.
	
	There will be 15 levels in the arcade mode, each about 5 minutes long when done perfectly.
	It will be very difficult, and a bullet-hell game in nature.
	
	Compile with Visual Studio command prompt, using the following:
	cl /EHsc ./../src/main.cpp ./../src/Engine/Window.cpp ./../src/Engine/Renderer.cpp ./../src/Engine/Pixel.cpp ./../src/Engine/Camera.cpp /O2 /link gdi32.lib user32.lib /out:./game.exe
*/

#include "./Engine/Renderer.hpp"
#include "./Engine/Mathematics.hpp"
#include "./Engine/Model.hpp"
#include "./Engine/Camera.hpp"

//PROBLEM: the mountains 3d model runs much better in the C based engine, 3d model 1, on my hard drive - find out why

//entry point
int main(){
	//create window
	Window * window = new Window("Test Window", 640, 360);
	Renderer renderer(window);

	//set fullscreen
	window->setFullscreen(true);

	//set fov
	renderer.setFov(PI / 2);
	
	double deltaTimeAverage[1000];
	int deltaTimeIndex = 0;
	
	//load mesh
	Mesh m;
	Bitmap bitmap;
	Mesh::loadMeshFromObjFile("./res/Castle.obj", &m, "./res/Low.bmp", &bitmap);
	
	//calculate aspect ratio
	double aspectRatio = (double) window->getWidth() / window->getHeight();
	double angle = 0.0f;
	
	//create 3d model
	Model model(&m, Vec4f(1.0f, 1.0f, 1.0f, 0.0f), Vec4f(angle, 0, 0, 0.0f), Vec4f(-0.50f, 0.0f, 15.0f, 1.0f));
	Model model2(&m, Vec4f(1.0f, 1.0f, 1.0f, 0.0f), Vec4f(angle, 0, 0, 0.0f), Vec4f(4.0f, 0.0f, 15.0f, 1.0f));
	
	//create camera
	Camera camera = Camera();
	camera.move(Vec4f(0.0f, 0.0f, 0.0f, 1.0f));
	
	//create lights
	std::vector <Light> lights;
	lights.push_back(Light(AMBIENT_LIGHT, Vec4f(), Vec4f(), 0.4));
	lights.push_back(Light(POINT_LIGHT, Vec4f(0.0f, 0.0f, 0.0f, 1.0f), Vec4f(0.0f, 1.0f, 0.0f, 0.0f), 0.6));
	
	double x = 300;
	double y = 240;
	
	//main loop
	while(window->getRunning()){
		//update delta time
		window->updateDeltaTime();
		
		//std::cout << "Delta time: " << window->getDeltaTime() << std::endl;
		deltaTimeAverage[deltaTimeIndex] = window->getDeltaTime();
		deltaTimeIndex++;
		
		if(deltaTimeIndex == 1000){
			deltaTimeIndex = 0;
			
			double total = 0;
			for(int i = 0; i < 1000; i++){
				total += deltaTimeAverage[i];
			};
			
			//std::cout << "Delta time: " << total / 1000.f << std::endl;
		};
		
		//handle events
		window->handleEvents();
		
		//clear screen
		window->clearScreen(166, 200, 255);
		
		//rotate
		if(GetAsyncKeyState(VK_LEFT)){
			camera.rotate(Vec4f(0.0f, 6.0f * window->getDeltaTime(), 0.0f, 0.0f));
			x -= window->getDeltaTime() * 100;
		};
		
		if(GetAsyncKeyState(VK_RIGHT)){
			camera.rotate(Vec4f(0.0f, -6.0f * window->getDeltaTime(), 0.0f, 0.0f));
			x += window->getDeltaTime() * 100;
		};
		
		if(GetAsyncKeyState(VK_UP)){
			camera.rotate(Vec4f(6.0f * window->getDeltaTime(), 0.0f, 0.0f, 0.0f));
			y += window->getDeltaTime() * 100;
		};
		
		if(GetAsyncKeyState(VK_DOWN)){
			camera.rotate(Vec4f(-6.0f * window->getDeltaTime(), 0.0f, 0.0f, 0.0f));
			y -= window->getDeltaTime() * 100;
		};
		
		if(GetAsyncKeyState('W')){
			camera.moveForward(10.0f * window->getDeltaTime());
		};
		
		if(GetAsyncKeyState('S')){
			camera.moveForward(-10.0f * window->getDeltaTime());
		};
		
		if(GetAsyncKeyState('D')){
			camera.moveRight(10.0f * window->getDeltaTime());
		};
		
		if(GetAsyncKeyState('A')){
			camera.moveRight(-10.0f * window->getDeltaTime());
		};
		
		//draw 3d model
		renderer.draw3dModel(&model, &camera, lights);
		//=renderer.draw3dModel(&model2, &camera, lights);
		//renderer.drawBitmap(&bitmap, 0, 0);
		
		//void drawShadedTriangle(int x1, int y1, int x2, int y2, int x3, int y3, double i1, double i2, double i3, double d1, double d2, double d3, double tx1, double ty1, double tx2, double ty2, double tx3, double ty3, Bitmap * bmp, Pixel c1, Pixel c2, Pixel c3){
		
		//This renders wrong
		//renderer.drawShadedTriangle(50, 300, 500, 400, 100, 50, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.03125f, 0.625f, 0.0625f, 0.5f, &bitmap, Pixel(), Pixel(), Pixel());
		//renderer.drawShadedTriangle(200, 300, x, y, 100, 200, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.03125f, 0.625f, 0.0625f, 0.5f, &bitmap, Pixel(), Pixel(), Pixel());
		
		//this works - the vertices are ordered correctly
		//renderer.drawShadedTriangle(50, 50, 200, 250, 300, 75, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.03125f, 0.625f, 0.0625f, 0.5f, &bitmap, Pixel(), Pixel(), Pixel());
		
		//this works - the vertices are not ordered correctly
		//renderer.drawShadedTriangle(200, 250, 50, 50, 300, 75, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.03125f, 0.625f, 0.0625f, 0.5f, &bitmap, Pixel(), Pixel(), Pixel());
		
		//this also works
		//renderer.drawShadedTriangle(200, 250, 300, 75, 50, 50, 1.0f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.5f, 0.03125f, 0.625f, 0.0625f, 0.5f, &bitmap, Pixel(), Pixel(), Pixel());
		
		/*
			The textures seem to break when the vertices are not in the correct order
			-There is not much evidence to support tthis anymore
			
			The problem appears to be related to obtuse angles
		*/
		
		//swap buffers (display new frame)
		window->swapBuffers();
	};
	
	return 0;
};