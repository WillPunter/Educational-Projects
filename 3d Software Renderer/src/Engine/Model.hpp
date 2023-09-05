//Model.hpp

#ifndef MODEL_HPP
#define MODEL_HPP

#include "Mesh.hpp"

//mesh
class Model {
	public:
		//properties
		Mesh * mesh;
		Vec4f enlargement;
		Vec4f rotation;
		Vec4f translation;
		
		//constructor
		Model(Mesh * mesh){
			this->mesh = mesh;
			this->enlargement = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
			this->rotation = Vec4f(0.0f, 0.0f, 0.0f, 0.0f);
			this->translation = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
		};
		
		Model(Mesh * mesh, Vec4f enlargement, Vec4f rotation, Vec4f translation){
			this->mesh = mesh;
			this->enlargement = enlargement;
			this->rotation = rotation;
			this->translation = translation;
		};
};

#endif