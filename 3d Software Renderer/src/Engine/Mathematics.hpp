//Mathematics.hpp

#ifndef MATHEMATICS_HPP
#define MATHEMATICS_HPP

/*
	Important note:
		This file currently contains classes for floating point vectors and matrices of the following types:
			2d vector
			3d vector
			4d vector
			4x4 matrix
		These have been chosen because they are all useful for 3d graphics and will be used in the game engine extensively.
		If different types are needed, or different dimensions of vectors or matrices are needed, these should be added ad hoc.
		
		This is because it would take a long time to write the code for vector and matrix operations of many different dimensions.
		Since these other-sized matrices and vectors are unlikely to be used, the time would be better spent on other features of the engine
		until there is a need for different matrix and vector features.
*/

//include necessary headers
#include <math.h>
#include <iostream>

//define PI as a constant
#define PI 3.14159

//vector2f
class Vec2f {
	public:
		double x;
		double y;
		
		Vec2f(){this->x = 0; this->y = 0;};		
		Vec2f(double x, double y) : x(this->x), y(this->y) {};
		
		//overload arithmetic operators
		Vec2f operator+(Vec2f & v){
			Vec2f result;
			result.x = this->x + v.x;
			result.y = this->y + v.y;
			return result;
		};
		
		Vec2f operator-(Vec2f & v){
			Vec2f result;
			result.x = this->x - v.x;
			result.y = this->y - v.y;
			return result;
		};
		
		Vec2f operator+=(Vec2f & v){
			this->x += v.x;
			this->y += v.y;
			return *this;
		};
		
		Vec2f operator-=(Vec2f & v){
			this->x -= v.x;
			this->y -= v.y;
			return *this;
		};
};

//vector3f
class Vec3f {
	public:
		double x;
		double y;
		double z;
		
		Vec3f(){this->x = 0; this->y = 0; this->z = 0;};		
		Vec3f(double x, double y, double z) : x(this->x), y(this->y), z(this->z) {};

		//overload arithmetic operators
		Vec3f operator+(Vec3f & v){
			Vec3f result;
			result.x = this->x + v.x;
			result.y = this->y + v.y;
			result.z = this->z + v.z;
			return result;
		};
		
		Vec3f operator-(Vec3f & v){
			Vec3f result;
			result.x = this->x - v.x;
			result.y = this->y - v.y;
			result.z = this->z - v.z;
			return result;
		};
		
		Vec3f operator+=(Vec3f & v){
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		};
		
		Vec3f operator-=(Vec3f & v){
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		};
};

//vector4f
//IMPORTANT: note that the Vec4f class should be used for 3d homogenous coordinates and not for 4d coordinates - the operations defined on the Vec4f class will not work for 4d coordinates!
class Vec4f {
	public:
		double x;
		double y;
		double z;
		double w;
		
		Vec4f(){this->x = 0; this->y = 0; this->z = 0; this->w = 0;};

		//Note that the parameter inside the brackets is the function argument, the number outside the brackets is the data member, e.g. for x(x), the first x is the data member and the second x is the function argument
		Vec4f(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {};
		
		//overload arithmetic operators
		Vec4f operator+(Vec4f & v){
			Vec4f result;
			result.x = this->x + v.x;
			result.y = this->y + v.y;
			result.z = this->z + v.z;
			return result;
		};
		
		Vec4f operator-(Vec4f & v){
			Vec4f result;
			result.x = this->x - v.x;
			result.y = this->y - v.y;
			result.z = this->z - v.z;
			return result;
		};
		
		Vec4f operator+=(Vec4f & v){
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		};
		
		Vec4f operator-=(Vec4f & v){
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		};
};

//matrix
class Mat4x4f {
	public:
		//data members
		double data[4][4];
		
		//overload arithmetic operators
		Mat4x4f operator+(Mat4x4f & m){
			Mat4x4f result;
			
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					result.data[i][j] = this->data[i][j] + m.data[i][j];
				};
			};
		};
		
		Mat4x4f operator-(Mat4x4f & m){
			Mat4x4f result;
			
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					result.data[i][j] = this->data[i][j] - m.data[i][j];
				};
			};
		};
		
		Mat4x4f operator+=(Mat4x4f & m){			
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					this->data[i][j] += m.data[i][j];
				};
			};
			
			return *this;
		};
		
		Mat4x4f operator-=(Mat4x4f & m){			
			for(int i = 0; i < 4; i++){
				for(int j = 0; j < 4; j++){
					this->data[i][j] -= m.data[i][j];
				};
			};
			
			return *this;
		};
};

//maths class - this is a static class that is not meant to be instantiated
//it provides methods for matrix and vector operations
class Math {
	public:
		//magnitude
		static double magnitude(Vec2f a){
			return sqrt(a.x * a.x + a.y * a.y);
		};
		
		static double magnitude(Vec3f a){
			return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
		};
		
		static double magnitude(Vec4f a){
			return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
		};
		
		//normalise
		static Vec2f normalise(Vec2f a){
			double mag = magnitude(a);
			a.x /= mag;
			a.y /= mag;
			return a;
		};
		
		static Vec3f normalise(Vec3f a){
			double mag = magnitude(a);
			a.x /= mag;
			a.y /= mag;
			a.z /= mag;
			return a;
		};
		
		static Vec4f normalise(Vec4f a){
			double mag = magnitude(a);
			a.x /= mag;
			a.y /= mag;
			a.z /= mag;
			a.w /= mag;
			return a;
		};
		
		//scalar product
		static Vec2f scalarProduct(double s, Vec2f v){
			v.x *= s;
			v.y *= s;
			return v;
		};
		
		static Vec3f scalarProduct(double s, Vec3f v){
			v.x *= s;
			v.y *= s;
			v.z *= s;
			return v;
		};
		
		static Vec4f scalarProduct(double s, Vec4f v){
			v.x *= s;
			v.y *= s;
			v.z *= s;
			return v;
		};
		
		//dot product
		static double dotProduct(Vec2f a, Vec2f b){
			return a.x * b.x + a.y * b.y;
		};
	
		static double dotProduct(Vec3f a, Vec3f b){
			return a.x * b.x + a.y * b.y + a.z * b.z;
		};
		
		static double dotProduct(Vec4f a, Vec4f b){
			return a.x * b.x + a.y * b.y + a.z * b.z;
		};
		
		//cross product
		static Vec3f crossProduct(Vec3f a, Vec3f b){
			Vec3f result;
			result.x = a.y * b.z - a.z * b.y;
			result.y = a.x * b.z - a.z * b.x; //x and z may need to be swapped around
			result.z = a.x * b.y - a.y * b.x; 
			return result;
		};
		
		static Vec4f crossProduct(Vec4f a, Vec4f b){
			Vec4f result;
			result.x = a.y * b.z - a.z * b.y;
			result.y = a.z * b.x - a.x * b.z; //x and z may need to be swapped around
			result.z = a.x * b.y - a.y * b.x; 
			return result;
		};
		
		
		//matrix-matrix multiplication
		/*
			Matrix-matrix multiplication is defined by the following:
			[a1 a2 a3 a4]	[b1 b2 b3 b4]
			[a5 a6 a7 a8]	[b5 b6 b7 b8]
			[a9 aa ab ac] * [b9 ba bb bc]
			[ad ae af ag]	[bd be bf bg]
			
			[a1b1+a2b5+a3b9+a4bd a1b2+a2b6+a3ba+a4be a1b3+a2b7+a3bb+a4bf a1b4+a2b8+a3bc+a4bg]
			[a5b1+a6b5+a7b9+a8bd a5b2+a6b6+a7ba+a8be a5b3+a6b7+a7bb+a8bf a5b4+a6b8+a7bc+a8bg]
			[a9b1+aab5+abb9+acbd a9b2+aab6+abba+acbe a9b3+aab7+abbb+acbf a9b4+aab8+abbc+acbg]
			[adb1+aeb5+afb9+agbd adb2+aeb6+afba+agbe adb3+aeb7+afbb+agbf adb4+aeb8+afbc+agbg]
			
			To account for column major order, we can use:
			
		*/
		
		static Mat4x4f matrixProduct(Mat4x4f a, Mat4x4f b){
			Mat4x4f result;
			
			//iterate through rows (i) of result
			for(int i = 0; i < 4; i++){
				//iterate through columns (j) of result
				for(int j = 0; j < 4; j++){
					result.data[i][j] = a.data[i][0] * b.data[0][j] + a.data[i][1] * b.data[1][j] + a.data[i][2] * b.data[2][j] + a.data[i][3] * b.data[3][j];
				};
			};
			
			return result;
		};
		
		//matrix-vector multiplication
		/*
			Matrix-vector multiplication is defined by the following:
			[0 1 2 3]	[g]		[0*g + 1*h + 2*i + 3*j]
			[4 5 6 7] * [h] =	[4*g + 5*h + 6*i + 7*j]
			[8 9 a b]	[i]		[8*g + 9*h + a*i + n*j]
			[c d e f]	[j]		[c*g + d*h + e*i + f*j]
		*/
		static Vec4f matrixProduct(Mat4x4f m, Vec4f v){
			Vec4f result;
			result.x = m.data[0][0] * v.x + m.data[0][1] * v.y + m.data[0][2] * v.z + m.data[0][3] * v.w;
			result.y = m.data[1][0] * v.x + m.data[1][1] * v.y + m.data[1][2] * v.z + m.data[1][3] * v.w;
			result.z = m.data[2][0] * v.x + m.data[2][1] * v.y + m.data[2][2] * v.z + m.data[2][3] * v.w;
			result.w = m.data[3][0] * v.x + m.data[3][1] * v.y + m.data[3][2] * v.z + m.data[3][3] * v.w;
			return result;
		};
		
		/*
			The transpose of a matrix is defined by mapping:
				[0 1 2 3]
				[4 5 6 7]
				[8 9 a b]
				[c d e f]
			To:
				[0 4 8 c]
				[1 5 9 d]
				[2 6 a e]
				[3 7 b f]
		*/
		static Mat4x4f matrixTranspose(Mat4x4f a){
			double temp;
			
			for(int i = 0; i < 4; i++){
				//start after diagonal line - numbers on the diagonal do not have to be swapped
				//numbers before the diagonal do not have to be swapped directly, as they will be swapped as they are to be swapped with the numbers to the right of the diagonal
				for(int j = i + 1; j < 4; j++){
					//swap data[i][j] and data[j][i]
					temp = a.data[i][j];
					a.data[i][j] = a.data[j][i];
					a.data[j][i] = temp;
				};
			};
		};
		
		//identity matrix
		static Mat4x4f identityMatrix(){
			Mat4x4f m;
			memset(&m, 0, sizeof(Mat4x4f));
			
			for(int i = 0; i < 4; i++){
				m.data[i][i] = 1;
			};
			
			return m;
		};
		
		//enlargment matrix
		static Mat4x4f enlargementMatrix(double xScale, double yScale, double zScale){
			Mat4x4f m = identityMatrix();
			m.data[0][0] = xScale;
			m.data[1][1] = yScale;
			m.data[2][2] = zScale;
			m.data[3][3] = 1;
			return m;
		};
		
		static Mat4x4f enlargementMatrix(Vec4f v){
			Mat4x4f m = identityMatrix();
			m.data[0][0] = v.x;
			m.data[1][1] = v.y;
			m.data[2][2] = v.z;
			m.data[3][3] = 1;
			return m;
		};
		
		//rotation matrices
		//rotation around an arbitrary direction vector
		/*
			Derivation:
			The vector (V) to rotate, the axis unit vector (D) to rotate around and the angle (a) are all specified.
			The component of V in the direction of D, (V.D)D is not rotated.
			The component of V perpendicular to D, V - (V.D)D is rotated.
			The rotation of the perpendicular vector can be expressed in terms of the two direction vectors perpendicular to D.
			The second perpendicular vector (P) can be found by taking the cross product of V and D and normalising the result.
			The rotated perpendicular component of V is given by cos(a) * first perpendicular + sin(a) * second perpendicular.
			So the total vector is given by: parallel component + cos(a) * first perpendicular + sin(a) * second perpendicular
			Numerically, this is given by:
				(V.D)D + cos(a) * (V - (V.D)D) + sin(a) * (DxV)
				=
				cos(a)V + (1 - cos(a))(V.D)D + sin(a)(DxV)
				=
				(cos(a)Vx)				(VxDxDx + VyDyDx + VzDzDx)			(DyVz - DzVy)
				(cos(a)Vy)+	(1-cos(a))*	(VxDxDy + VyDyDy + VzDzDy) + sin(a)*(DzVx - DxVz)
				(cos(a)Vz)				(VxDxDz + VyDyDz + VzDzDz)			(DxVy - DyVx)
				
				( [cos(a) + (1-cos(a))DxDx]Vx + [(1-cos(a))DyDx + sin(a)Dz]Vy + [(1-cos(a)) * DzDx - sin(a)Dy]Vz )
				( [(1-cos(a))DxDy - sin(a)Dz]Vx + [cos(a) + (1-cos(a))DyDy]Vy + [(1-cos(a)DzDy + sin(a)Dx]Vz )
				( [(1-cos(a))DxDz + sin(a)Dy]Vx + [(1-cos(a))DyDz - sin(a)Dx]Vy + [cos(a) + (1-cos(a))DzDz]Vz )
				
				c = cos(a)
				s = sin(a)
				x = Dx
				y = Dy
				z = Dz
				
				This gives the matrix:
				[(c + (1-c)x^2) ((1-c)xy + sz) ((1-c)xz - sy) 0]
				[((1-c)xy - sz) (c + (1-c)y^2) ((1-c)yz + sx) 0]
				[((1-c)xz + sy) ((1-c)yz - sx) (c + (1-c)z^2) 0]
				[0				0			   0			  0]
		*/	
		
		static Mat4x4f rotationMatrixFromAxis(Vec4f d, double angle){
			//calculate sin and cos of angle
			double sinAngle = sin(angle);
			double cosAngle = cos(angle);
			
			Mat4x4f result = {0};
			
			std::cout << "<" << d.x << ", " << d.y << ", " << d.z << ">" << std::endl;
			
			//first row
			result.data[0][0] = cosAngle + (1 - cosAngle) * d.x * d.x;
			result.data[0][1] = (1 - cosAngle) * d.x * d.y + sinAngle * d.z;
			result.data[0][2] = (1 - cosAngle) * d.x * d.z - sinAngle * d.y;
			result.data[0][3] = 0;
			
			//second row
			result.data[1][0] = (1 - cosAngle) * d.x * d.y - sinAngle * d.z;
			result.data[1][1] = cosAngle + (1 - cosAngle) * d.y * d.y;
			result.data[1][2] = (1 - cosAngle) * d.y * d.z + sinAngle * d.x;
			result.data[1][3] = 0;
			
			//third row
			result.data[2][0] = (1 - cosAngle) * d.x * d.z + sinAngle * d.y;
			result.data[2][1] = (1 - cosAngle) * d.y * d.z - sinAngle * d.x;
			result.data[2][2] = cosAngle + (1 - cosAngle) * d.z * d.z;
			result.data[2][3] = 0;
			
			//fourth row
			result.data[3][0] = 0;
			result.data[3][1] = 0;
			result.data[3][2] = 0;
			result.data[3][3] = 1;
			
			return result;
		};
		
		//rotation around x-axis
		static Mat4x4f rotationAroundXAxis(double angle){
			//generate identity matrix
			Mat4x4f result = Math::identityMatrix();
			
			//calculate sin and cos of angle
			double sinAngle = sin(angle);
			double cosAngle = cos(angle);
			
			//set z transformation
			result.data[2][2] = cosAngle;
			result.data[2][1] = -sinAngle;
			
			//set y transformation
			result.data[1][2] = sinAngle;
			result.data[1][1] = cosAngle;
			
			return result;
		};
		
		//rotation around y-axis
		static Mat4x4f rotationAroundYAxis(double angle){
			//generate identity matrix
			Mat4x4f result = Math::identityMatrix();
			
			//calculate sin and cos of angle
			double sinAngle = sin(angle);
			double cosAngle = cos(angle);
			
			//set x transformation
			result.data[0][0] = cosAngle;
			result.data[0][2] = -sinAngle;
			
			//set y transformation
			result.data[2][0] = sinAngle;
			result.data[2][2] = cosAngle;
	
			return result;
		};
		
		//rotation around z-axis
		static Mat4x4f rotationAroundZAxis(double angle){
			//generate identity matrix
			Mat4x4f result = Math::identityMatrix();
			
			//calculate sin and cos of angle
			double sinAngle = sin(angle);
			double cosAngle = cos(angle);
			
			//set x transformation
			result.data[0][0] = cosAngle;
			result.data[0][1] = -sinAngle;
			
			//set y transformation
			result.data[1][0] = sinAngle;
			result.data[1][1] = cosAngle;
			
			return result;
		};
		
		//rotation matrix
		static Mat4x4f rotationMatrix(double x, double y, double z){
			return Math::matrixProduct(Math::rotationAroundYAxis(y), Math::matrixProduct(Math::rotationAroundZAxis(z), Math::rotationAroundXAxis(x)));
		};
		
		static Mat4x4f rotationMatrix(Vec4f r){
			return Math::rotationMatrix(r.x, r.y, r.z);
		};
		
		//create translation matrix
		static Mat4x4f translationMatrix(double x, double y, double z){
			Mat4x4f result = Math::identityMatrix();
			result.data[0][3] = x;
			result.data[1][3] = y;
			result.data[2][3] = z;
			return result;
		};
		
		static Mat4x4f translationMatrix(Vec4f t){
			return Math::translationMatrix(t.x, t.y, t.z);
		};
		
		//rotation around x axis
		
		//rotation around y axis
		
		//rotation around z axis
		
		//projection vector
		/*
			Vy / Vz = V'y / V'z
			V'y = V'z * Vy / Vz
			V'y = (1/tanHalfFOV) * Vy / Vz
		*/
		
		static Vec4f projectVector(Vec4f v, double aspectRatio, double tanHalfFOV){
			v.x = (1.f / tanHalfFOV) * v.x / v.z;
			v.y = (1.f / tanHalfFOV) * v.y / v.z * aspectRatio;
			return v;
		};
		
		//NDC to pixel coordinates
		static Vec4f ndcToPixel(Vec4f v, int bufferWidth, int bufferHeight){
			v.x = (v.x + 1) * bufferWidth / 2;
			v.y = (v.y + 1) * bufferHeight / 2;
			return v;
		};
		
		/*
			Shortest distance from point to plane
			
			The shortest distance from the point to the plane is along the normal.
			
			We can get this by projecting the vector point -> plane point onto the normal unit vector.
		*/
		
		static double shortestDistanceFromPointToPlane(Vec4f planePos, Vec4f planeNormal, Vec4f point){
			return Math::dotProduct((planePos - point), Math::normalise(planeNormal));
		};
		
		//line intersect plane
		/*
			Planes are exrpressed in the form r . n = r0.n or r.n = k, where r is any point on the plane,
			n is the plane normal and r0 is a point known to be on the plane.
			
			(r - r0) . n = 0
			
			r.n - r0.n = 0
			r.n = r0.n
			
			(line start + unknown * line direction) . plane normal = known plane point . plane normal
			(S + k * D) . N = L
			
			(Sx + kDx)Nx + (Sy + kDy)Ny + (Sz + kDz)Nz = L
			
			k(DxNx + DyNy + DzNz) = L - (SxNx + SyDy + SzDz)
			k = (L - (SxNx + SyDy + SzDz)) / (DxNx + DyNy + DzNz)
			k = (L - S.N) / (D.N)
		*/
		static Vec4f lineIntersectPlane(Vec4f planePos, Vec4f planeNormal, Vec4f lineStart, Vec4f lineDirection){
			//normalise plane normal
			planeNormal = Math::normalise(planeNormal);
			
			//calculate r0.n
			double constant = Math::dotProduct(planePos, planeNormal);
			
			//calculate scale 
			double scale = (constant - Math::dotProduct(lineStart, planeNormal)) / Math::dotProduct(lineDirection, planeNormal);
			
			//return vector4f
			return lineStart + Math::scalarProduct(scale, lineDirection);
		};
};

#endif