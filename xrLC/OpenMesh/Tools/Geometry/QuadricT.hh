//=============================================================================
//                                                                            
//                               OpenMesh                                     
//        Copyright (C) 2003 by Computer Graphics Group, RWTH Aachen          
//                           www.openmesh.org                                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//                                License                                     
//                                                                            
//   This library is free software; you can redistribute it and/or modify it 
//   under the terms of the GNU Library General Public License as published  
//   by the Free Software Foundation, version 2.                             
//                                                                             
//   This library is distributed in the hope that it will be useful, but       
//   WITHOUT ANY WARRANTY; without even the implied warranty of                
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
//   Library General Public License for more details.                          
//                                                                            
//   You should have received a copy of the GNU Library General Public         
//   License along with this library; if not, write to the Free Software       
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 
//                                                                            
//-----------------------------------------------------------------------------
//                                                                            
//   $Revision: 1.3 $
//   $Date: 2003/04/08 10:53:00 $
//                                                                            
//=============================================================================

/** \file QuadricT.hh
    
 */

//=============================================================================
//
//  CLASS QuadricT
//
//=============================================================================

#ifndef OPENMESH_GEOMETRY_QUADRIC_HH
#define OPENMESH_GEOMETRY_QUADRIC_HH


//== INCLUDES =================================================================

#include "Config.hh"
#include <OpenMesh/Core/Math/VectorT.hh>

//== NAMESPACE ================================================================

namespace OpenMesh { //BEGIN_NS_OPENMESH
namespace Geometry { //BEGIN_NS_GEOMETRY


//== CLASS DEFINITION =========================================================


/** /class QuadricT QuadricT.hh <OSG/Geometry/Types/QuadricT.hh>

    Stores a quadric as a 4x4 symmetrix matrix. Used by the
    error quadric based mesh decimation algorithms.
**/

template <class Scalar,
          class Vector3Elem = VectorT<Scalar, 3>,
          class Vector4Elem = VectorT<Scalar, 4> >
class QuadricT
{
public:
   typedef Scalar           value_type;
   typedef QuadricT<Scalar> type;
   typedef QuadricT<Scalar> Self;
   //   typedef VectorInterface<Scalar, VecStorage3<Scalar> > Vec3;
   //   typedef VectorInterface<Scalar, VecStorage4<Scalar> > Vec4;
   typedef Vector3Elem      Vec3;
   typedef Vector4Elem      Vec4;
   
   /// construct with upper triangle of symmetrix 4x4 matrix
   QuadricT(Scalar _a, Scalar _b, Scalar _c, Scalar _d,
                       Scalar _e, Scalar _f, Scalar _g,
                                  Scalar _h, Scalar _i,
                                             Scalar _j)
         : a(_a), b(_b), c(_c), d(_d), 
                  e(_e), f(_f), g(_g),    
                         h(_h), i(_i), 
                                j(_j)
   {}


  /// constructor from given plane equation: ax+by+cz+d=0
   QuadricT( Scalar _a=0.0, Scalar _b=0.0, Scalar _c=0.0, Scalar _d=0.0 )
         :  a(_a*_a), b(_a*_b),  c(_a*_c),  d(_a*_d),
                      e(_b*_b),  f(_b*_c),  g(_b*_d),
                                 h(_c*_c),  i(_c*_d),
                                            j(_d*_d)
   {}


   /// set all entries to zero
   void clear()  { a = b = c = d = e = f = g = h = i = j = 0.0; }

  
   /// add quadrics
   QuadricT<Scalar>& operator+=( const QuadricT<Scalar>& _q )
   {
      a += _q.a;  b += _q.b;  c += _q.c;  d += _q.d;
                  e += _q.e;  f += _q.f;  g += _q.g;
                              h += _q.h;  i += _q.i;
                                          j += _q.j;
      return *this;
   }


   /// multiply by scalar
   QuadricT<Scalar>& operator*=( Scalar _s)
   {
      a *= _s;  b *= _s;  c *= _s;  d *= _s;
              e *= _s;  f *= _s;  g *= _s;
                        h *= _s;  i *= _s;
                                  j *= _s;
      return *this;
   }


   /// multiply 4D vector from right: Q*v
   Vec4 operator*(const Vec4& _v) const
   {
      return Vec4(_v[0]*a + _v[1]*b + _v[2]*c + _v[3]*d,
                  _v[0]*b + _v[1]*e + _v[2]*f + _v[3]*g,
                  _v[0]*c + _v[1]*f + _v[2]*h + _v[3]*i,
                  _v[0]*d + _v[1]*g + _v[2]*i + _v[3]*j);
   }
  

   /// evaluate quadric Q at vector v: v*Q*v
   Scalar operator()(const Vec3 _v) const
   {
      Scalar x(_v[0]), y(_v[1]), z(_v[2]);
      return a*x*x + 2.0*b*x*y + 2.0*c*x*z + 2.0*d*x
                   +     e*y*y + 2.0*f*y*z + 2.0*g*y
	                       +     h*z*z + 2.0*i*z
                                           +     j;
   }


   /// evaluate quadric Q at vector v: v*Q*v
   Scalar operator()(const Vec4 _v) const
   {
      Scalar x(_v[0]), y(_v[1]), z(_v[2]), w(_v[3]);
      return a*x*x + 2.0*b*x*y + 2.0*c*x*z + 2.0*d*x*w
                   +     e*y*y + 2.0*f*y*z + 2.0*g*y*w
	                       +     h*z*z + 2.0*i*z*w
                                           +     j*w*w;
   }
  

private:

   Scalar a, b, c, d, 
             e, f, g, 
                h, i, 
                   j;
};


/// Quadric using floats
typedef QuadricT<float> Quadricf;

/// Quadric using double
typedef QuadricT<double> Quadricd;


//=============================================================================
} // END_NS_GEOMETRY
} // END_NS_OPENMESH
//============================================================================
#endif // OPENMESH_GEOMETRY_HH defined
//=============================================================================
