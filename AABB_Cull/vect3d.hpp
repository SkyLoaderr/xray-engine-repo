//==========================================================================
// vect3d.hpp : 3d vector class declaration
//==========================================================================

#ifndef VECT3D
#define VECT3D

#include <stdio.h>
#include <math.h>

class Vect3d
{ 
  public:
    float x, y, z;
    
    Vect3d (void) 
      {};
    Vect3d (const float X, const float Y, const float Z) 
      { x=X; y=Y; z=Z; };
    Vect3d (const Vect3d& v) 
      { x=v.x; y=v.y; z=v.z; };
    void Set (const float X, const float Y, const float Z)
      { x=X; y=Y; z=Z; }

    Vect3d& operator = (const Vect3d& A)          // ASSIGNMENT (=)
      { x=A.x; y=A.y; z=A.z; 
        return(*this);  };
    Vect3d operator + (const Vect3d& A) const     // ADDITION (+)
      { Vect3d Sum(x+A.x, y+A.y, z+A.z); 
        return(Sum); };
    Vect3d operator - (const Vect3d& A) const     // SUBTRACTION (-)
      { Vect3d Diff(x-A.x, y-A.y, z-A.z);
        return(Diff); };
    float operator * (const Vect3d& A) const      // DOT-PRODUCT (*)
      { float DotProd = x*A.x+y*A.y+z*A.z; 
        return(DotProd); };
    Vect3d operator / (const Vect3d& A) const     // CROSS-PRODUCT (/)
      { Vect3d CrossProd(y*A.z-z*A.y, z*A.x-x*A.z, x*A.y-y*A.x); 
        return(CrossProd); };
    Vect3d operator * (const float s) const       // MULTIPLY BY SCALAR (*)
      { Vect3d Scaled(x*s, y*s, z*s); 
        return(Scaled); };
    Vect3d operator / (const float s) const       // DIVIDE BY SCALAR (/)
      { Vect3d Scaled(x/s, y/s, z/s);
        return(Scaled); };

    void operator += (const Vect3d A)             // ACCUMULATED VECTOR ADDITION (+=)
      { x+=A.x; y+=A.y; z+=A.z; };
    void operator -= (const Vect3d A)             // ACCUMULATED VECTOR SUBTRACTION (+=)
      { x-=A.x; y-=A.y; z-=A.z; };
    void operator *= (const float s)              // ACCUMULATED SCALAR MULT (*=)
      { x*=s; y*=s; z*=s; };
    void operator /= (const float s)              // ACCUMULATED SCALAR DIV (/=)
      { x/=s; y/=s; z/=s; };
    Vect3d operator - (void) const                // NEGATION (-)
      { Vect3d Negated(-x, -y, -z);
        return(Negated); };

    float operator [] (const int i) const         // ALLOWS VECTOR ACCESS AS AN ARRAY.
      { return( (i==0)?x:((i==1)?y:z) ); };
    float & operator [] (const int i)             
      { return( (i==0)?x:((i==1)?y:z) ); };

    float Length (void) const                     // LENGTH OF VECTOR
      { return ((float)sqrt(x*x+y*y+z*z)); };
    float LengthSqr (void) const                  // LENGTH OF VECTOR (SQUARED)
      { return (x*x+y*y+z*z); };
    void Normalize (void)                         // NORMALIZE VECTOR
      { float L = Length();                       // CALCULATE LENGTH
        x/=L; y/=L; z/=L; };                      // DIVIDE COMPONENTS BY LENGTH

    void Print()
      { printf("(%.3f, %.3f, %.3f)\n",x, y, z); }

    void min (const Vect3d & a)
      { if (x>a.x) x=a.x; if (y>a.y) y=a.y; if (z>a.z) z=a.z; }
    void max (const Vect3d & a)
      { if (x<a.x) x=a.x; if (y<a.y) y=a.y; if (z<a.z) z=a.z; }
    Vect3d multComponents (const Vect3d& A) const
      { return Vect3d (x*A.x, y*A.y, z*A.z); }
};

#endif


