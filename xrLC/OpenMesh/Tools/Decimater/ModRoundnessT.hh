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
//   $Revision: 1.7 $
//   $Date: 2004/01/12 17:36:30 $
//                                                                            
//=============================================================================

/** \file ModRoundnessT.hh
    
 */

//=============================================================================
//
//  CLASS ModRoundnessT
//
//=============================================================================

#ifndef OPENMESH_TOOLS_MODROUNDNESST_HH
#define OPENMESH_TOOLS_MODROUNDNESST_HH


//== INCLUDES =================================================================

#include <OpenMesh/Tools/Decimater/ModBaseT.hh>
#include <math.h>

#if defined(OM_CC_MSVC)
#  define OM_ENABLE_WARNINGS 4244
#  pragma warning(disable : OM_ENABLE_WARNINGS )
#endif

//== NAMESPACE ================================================================

namespace OpenMesh { // BEGIN_NS_OPENMESH
namespace Decimater { // BEGIN_NS_DECIMATER


//== CLASS DEFINITION =========================================================


/** Compute error value based on roundness criteria.
 */
template <class DecimaterType>
class ModRoundnessT : public ModBaseT<DecimaterType>
{
public:

  DECIMATING_MODULE( ModRoundnessT, DecimaterType, Roundness );

public:

  // typedefs
  typedef typename Mesh::Point                      Point;
  typedef typename vector_traits<Point>::value_type value_type;

public:
   
  /// Constructor
  ModRoundnessT( DecimaterType &_dec ) :
    Base(_dec, false), 
    min_r_(-1.0)
  { }
 
  /// Destructor
  ~ModRoundnessT() { }

public: // inherited
   
  /** Compute collapse priority due to roundness of triangle.
   *
   *  The roundness is computed by dividing the radius of the
   *  circumference by the length of the shortest edge. The result is
   *  normalized.  
   *
   * \return [0:1] or ILLEGAL_COLLAPSE in non-binary mode
   * \return LEGAL_COLLAPSE or ILLEGAL_COLLAPSE in binary mode
   * \see set_min_roundness()
   */
  float collapse_priority(const CollapseInfo& _ci)  
  {    
    using namespace OpenMesh;

    typename Mesh::ConstVertexOHalfedgeIter voh_it(mesh(), _ci.v0);
    double                                  r;
    double                                  priority = 0.0; //==LEGAL_COLLAPSE
    typename Mesh::FaceHandle               fhC, fhB;
    Vec3f                                   B,C;
    
    if ( min_r_ < 0.0 ) // continues mode
    {      
      C   = vector_cast<Vec3f>(mesh().point( mesh().to_vertex_handle(voh_it)));
      fhC = mesh().face_handle( voh_it.handle() );

      for (++voh_it; voh_it; ++voh_it) 
      {
	B   = C;
	fhB = fhC;
	C   = vector_cast<Vec3f>(mesh().point(mesh().to_vertex_handle(voh_it)));
	fhC = mesh().face_handle( voh_it.handle() );

	if ( fhB == _ci.fl || fhB == _ci.fr )
	  continue;
      
	// simulate collapse using position of v1
	r = roundness( vector_cast<Vec3f>(_ci.p1), B, C );
      
	// return the maximum non-roundness
	priority = std::max( priority, (1.0-r) );
      }
    }
    else // binary mode
    {
      C   = vector_cast<Vec3f>(mesh().point( mesh().to_vertex_handle(voh_it)));
      fhC = mesh().face_handle( voh_it.handle() );

      for (++voh_it; voh_it && (priority==LEGAL_COLLAPSE); ++voh_it) 
      {
	B   = C;
	fhB = fhC;
	C   = vector_cast<Vec3f>(mesh().point(mesh().to_vertex_handle(voh_it)));
	fhC = mesh().face_handle( voh_it.handle() );

	if ( fhB == _ci.fl || fhB == _ci.fr )
	  continue;

	priority = (roundness( vector_cast<Vec3f>(_ci.p1), B, C ) < min_r_)
	  ? ILLEGAL_COLLAPSE 
	  : LEGAL_COLLAPSE;
      }
    }

    return priority;
  }
  
   

public: // specific methods

  void set_min_angle( float _angle, bool _binary=true )
  {
    assert( _angle > 0 && _angle < 60 );

    _angle = M_PI * _angle /180.0;

    Vec3f A,B,C;

    A = Vec3f(             0, 0,           0);
    B = Vec3f( 2*cos(_angle), 0,           0);
    C = Vec3f(   cos(_angle), sin(_angle), 0);

    double r1 = roundness(A,B,C);

    _angle = 0.5 * ( M_PI - _angle );

    A = Vec3f(             0, 0,           0);
    B = Vec3f( 2*cos(_angle), 0,           0);
    C = Vec3f(   cos(_angle), sin(_angle), 0);

    double r2 = roundness(A,B,C);

    set_min_roundness( std::min(r1,r2), true );
  }

  /** Set a minimum roundness value.
   *  \param _min_roundness in range (0,1)
   *  \param _binary Set true, if the binary mode should be enabled, 
   *                 else false. In latter case the collapse_priority()
   *                 returns a float value if the constrain does not apply
   *                 and ILLEGAL_COLLAPSE else.
   */
  void set_min_roundness( value_type _min_roundness, bool _binary=true )
  {
    assert( 0.0 <= _min_roundness && _min_roundness <= 1.0 );
    min_r_  = _min_roundness;
    set_binary(_binary);
  }

  /// Unset minimum value constraint and enable non-binary mode.
  void unset_min_roundness()
  {
    min_r_  = -1.0;
    set_binary(false);
  }

  // Compute a normalized roundness of a triangle ABC
  //
  // Having
  //   A,B,C corner points of triangle
  //   a,b,c the vectors BC,CA,AB
  //   Area  area of triangle
  //
  // then define
  //
  //      radius of circumference 
  // R := -----------------------
  //      length of shortest edge
  //
  //       ||a|| * ||b|| * ||c||    
  //       ---------------------
  //             4 * Area                 ||a|| * ||b|| * ||c||
  //    = ----------------------- = -----------------------------------
  //      min( ||a||,||b||,||c||)   4 * Area * min( ||a||,||b||,||c|| )
  //
  //                      ||a|| * ||b|| * ||c||
  //    = -------------------------------------------------------
  //      4 *  1/2 * ||cross(B-A,C-A)||  * min( ||a||,||b||,||c|| )
  //
  //                         a'a * b'b * c'c
  // R� = ----------------------------------------------------------
  //       4 * cross(B-A,C-A)'cross(B-A,C-A) * min( a'a, b'b, c'c )
  //
  //                      a'a * b'b * c'c
  // R = 1/2 * sqrt(---------------------------)
  //                 AA * min( a'a, b'b, c'c )
  //
  // At angle 60� R has it's minimum for all edge lengths = sqrt(1/3)
  //
  // Define normalized roundness 
  //
  // nR := sqrt(1/3) / R
  //
  //                         AA * min( a'a, b'b, c'c )
  //     = sqrt(4/3) * sqrt(---------------------------)
  //                              a'a * b'b * c'c
  //
  double roundness( const Vec3f& A, const Vec3f& B, const Vec3f &C )
  {
    const value_type epsilon = value_type(10e-11);

    static const value_type sqrt43 = sqrt(4.0/3.0); // 60�,a=b=c, **)

    Vec3f vecAC     = C-A;
    Vec3f vecAB     = B-A;

    // compute squared values to avoid sqrt-computations
    value_type aa = (B-C).sqrnorm();
    value_type bb = vecAC.sqrnorm();
    value_type cc = vecAB.sqrnorm();
    value_type AA = cross(vecAC,vecAB).sqrnorm(); // without factor 1/4   **)

    if ( AA < epsilon )
      return 0.0;

    double nom   = AA * std::min( std::min(aa,bb),cc );
    double denom = aa * bb * cc;
    double nR    = sqrt43 * sqrt(nom/denom);

    return nR;
  }

private:
  
  value_type min_r_;
};


//=============================================================================
} // END_NS_DECIMATER
} // END_NS_OPENMESH
//=============================================================================
#if defined(OM_CC_MSVC) && defined(OM_ENABLE_WARNINGS)
#  pragma warning(default : OM_ENABLE_WARNINGS)
#  undef OM_ENABLE_WARNINGS
#endif
//=============================================================================
#endif // OPENMESH_TOOLS_PROGMESHT_HH defined
//=============================================================================

