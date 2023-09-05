//Camera.hpp

#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "Mathematics.hpp"

//create camera class
class Camera {
	//public
	public:
		//constructor
		Camera();
		Camera(double px, double py, double pz, double rx, double ry, double rz);
		Camera(Vec4f position, Vec4f rotation);
		
		//get camera transformation matrix
		Mat4x4f getCameraTransformationMatrix();
		
		//get direction vector
		Vec4f getDirectionVector();
		
		//point at vertex
		void pointAtVertex(Vec4f point);
		
		//move
		void move(Vec4f translation);
		
		//rotate
		void rotate(Vec4f angle);
		
		//move forward
		void moveForward(double distance);
		
		//move right
		void Camera::moveRight(double distance);
		
	//private
	private:
		//private data members
		Vec4f position;
		Vec4f rotation;
		
};

#endif