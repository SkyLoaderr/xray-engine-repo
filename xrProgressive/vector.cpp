#include "stdafx.h"
#include "progmesh.h"
#include "vector.h"

float  sqr(float a) {return a*a;}

// vector (floating point) implementation

float magnitude_sqr(Vector &v) {
    return sqr(v.x) + sqr( v.y)+ sqr(v.z);
}
float magnitude(Vector &v) {
    return sqrtf(sqr(v.x) + sqr( v.y)+ sqr(v.z));
}
Vector normalize(Vector v) {
    float d=magnitude(v);
    if (d==0) {
		printf("Cant normalize ZERO vector\n");
		VERIFY(0);
		d=0.1f;
	}
    v.x/=d;
    v.y/=d;
    v.z/=d;
    return v;
}

Vector operator+(Vector& v1,Vector& v2) {return Vector(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z);}
Vector operator-(Vector& v1,Vector& v2) {return Vector(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z);}
Vector operator-(Vector& v)            {return Vector(-v.x,-v.y,-v.z);}
Vector operator*(Vector& v1,float s)   {return Vector(v1.x*s,v1.y*s,v1.z*s);}
Vector operator*(float s, Vector& v1)  {return Vector(v1.x*s,v1.y*s,v1.z*s);}
Vector operator/(Vector& v1,float s)   {return v1*(1.0f/s);}
float  operator^(Vector& v1,Vector& v2) {return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;}
Vector operator*(Vector& v1,Vector& v2) {
    return Vector(
                v1.y * v2.z - v1.z*v2.y,
                v1.z * v2.x - v1.x*v2.z,
                v1.x * v2.y - v1.y*v2.x);
}
Vector planelineintersection(Vector n,float d,Vector p1,Vector p2){
	// returns the point where the line p1-p2 intersects the plane n&d
        Vector dif  = p2-p1;
        float dn= n^dif;
        float t = -(d+(n^p1) )/dn;
        return p1 + (dif*t);
}

#define C_EPS 0.00001f
int concurrent(Vector a,Vector b) {
	return(fabsf(a.x-b.x)<C_EPS && fabsf(a.y-b.y)<C_EPS && fabsf(a.z-b.z)<C_EPS);
}
