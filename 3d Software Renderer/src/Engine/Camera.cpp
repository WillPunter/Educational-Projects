//Camera.cpp

#include "Camera.hpp"

//constructor
Camera::Camera(){
	this->position = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
	this->rotation = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
};

Camera::Camera(double px, double py, double pz, double rx, double ry, double rz){
	this->position = Vec4f(px, py, pz, 1.0f);
	this->rotation = Vec4f(rx, ry, rz, 0.0f);
};

Camera::Camera(Vec4f position, Vec4f rotation){
	this->position = position;
	this->rotation = rotation;
};

//get camera transformation matrix
Mat4x4f Camera::getCameraTransformationMatrix(){
	//translate then rotate around y axis, then x axis, then z axis	
	//Z(X(Y(TV))
	//note that operations are applied in reverse, because we move the world relative to the camera, rather than the other way around
	return Math::matrixProduct(Math::matrixProduct(Math::rotationAroundZAxis(-this->rotation.z), Math::matrixProduct(Math::rotationAroundXAxis(-this->rotation.x), Math::rotationAroundYAxis(-this->rotation.y))), Math::translationMatrix(Math::scalarProduct(-1.0f, this->position)));
};

//get direction vector
Vec4f Camera::getDirectionVector(){
	Mat4x4f rotationMatrix = Math::rotationMatrix(Math::scalarProduct(1, this->rotation));
	Vec4f dir(0.0f, 0.0f, 1.0f, 0.0f);
	return Math::matrixProduct(rotationMatrix, dir);
};
	
//point at vertex
void Camera::pointAtVertex(Vec4f point){
	Vec4f direction = point - this->position;
	this->rotation.x = atan2(direction.y, sqrt(direction.x * direction.x + direction.y * direction.y));
	this->rotation.y = atan2(direction.x, direction.z);
	this->rotation.z = 0;
};

//move
void Camera::move(Vec4f translation){
	this->position += translation;
};
	
//rotate
void Camera::rotate(Vec4f angle){
	this->rotation += angle;
};

//move forward
void Camera::moveForward(double distance){
	this->position += Math::scalarProduct(distance, this->getDirectionVector());
};

//move right
void Camera::moveRight(double distance){
	//get right vector using up vector and right vector
	Vec4f worldUp(0.0f, 1.0f, 0.0f, 0.0f);
	this->position += Math::scalarProduct(distance, Math::crossProduct(worldUp, this->getDirectionVector()));
};