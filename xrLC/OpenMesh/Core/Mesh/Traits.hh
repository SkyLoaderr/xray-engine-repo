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
//   $Revision: 1.16 $
//   $Date: 2004/01/13 15:28:49 $
//                                                                            
//=============================================================================


/** \file Core/Mesh/Traits.hh
    This file defines the default traits and some convenience macros.
*/


//=============================================================================
//
//  CLASS Traits
//
//=============================================================================

#ifndef OPENMESH_TRAITS_HH
#define OPENMESH_TRAITS_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Math/VectorT.hh>
#include <OpenMesh/Core/Attributes/Attributes.hh>
#include <OpenMesh/Core/Mesh/Kernels/Common/Handles.hh>


//== NAMESPACES ===============================================================


namespace OpenMesh {


//== CLASS DEFINITION =========================================================


/// Macro for defining the vertex attributes. See \ref mesh_type.
#define VertexAttributes(_i) enum { VertexAttributes = _i }

/// Macro for defining the halfedge attributes. See \ref mesh_type.
#define HalfedgeAttributes(_i) enum { HalfedgeAttributes = _i }

/// Macro for defining the edge attributes. See \ref mesh_type.
#define EdgeAttributes(_i) enum { EdgeAttributes = _i }

/// Macro for defining the face attributes. See \ref mesh_type.
#define FaceAttributes(_i) enum { FaceAttributes = _i }

/// Macro for defining the vertex traits. See \ref mesh_type.
#define VertexTraits \
  template <class Base, class Refs> struct VertexT : public Base

/// Macro for defining the halfedge traits. See \ref mesh_type.
#define HalfedgeTraits \
  template <class Base, class Refs> struct HalfedgeT : public Base

/// Macro for defining the edge traits. See \ref mesh_type.
#define EdgeTraits \
  template <class Base, class Refs> struct EdgeT : public Base

/// Macro for defining the face traits. See \ref mesh_type.
#define FaceTraits \
  template <class Base, class Refs> struct FaceT : public Base



//== CLASS DEFINITION =========================================================


/** \class DefaultTraits Traits.hh <OpenMesh/Mesh/Traits.hh>
    
    Base class for all traits.  All user traits should be derived from
    this class. You may enrich all basic items by additional
    properties or define one or more of the types \c Point, \c Normal,
    \c TexCoord, or \c Color.

    \see The Mesh docu section on \ref mesh_type.
    \see Traits.hh for a list of macros for traits classes.
*/
struct DefaultTraits
{
  /// The default coordinate type is OpenMesh::Vec3f.
  typedef Vec3f  Point;

  /// The default normal type is OpenMesh::Vec3f.
  typedef Vec3f  Normal;

  /// The default texture coordinate type is OpenMesh::Vec2f.
  typedef Vec2f  TexCoord;

  /// The default color type is OpenMesh::Vec3uc.
  typedef Vec3uc Color;

#ifndef DOXY_IGNORE_THIS
  VertexTraits    {};
  HalfedgeTraits  {};
  EdgeTraits      {};
  FaceTraits      {};
#endif
  
  VertexAttributes(0);
  HalfedgeAttributes(Attributes::PrevHalfedge);
  EdgeAttributes(0);
  FaceAttributes(0);
};


//== CLASS DEFINITION =========================================================


/** Helper class to merge two mesh traits. 
 *  \internal
 *
 *  With the help of this class it's possible to merge two mesh traits.
 *  Whereby \c _Traits1 overrides equally named symbols of \c _Traits2.
 *
 *  For your convenience use the provided defines \c OM_Merge_Traits
 *  and \c OM_Merge_Traits_In_Template instead.
 *
 *  \see OM_Merge_Traits, OM_Merge_Traits_In_Template
 */
template <class _Traits1, class _Traits2> struct MergeTraits
{
#ifndef DOXY_IGNORE_THIS
  struct Result
  {
    // Mipspro needs this (strange) typedef
    typedef _Traits1  T1;
    typedef _Traits2  T2;


    VertexAttributes   ( T1::VertexAttributes   | T2::VertexAttributes   );
    HalfedgeAttributes ( T1::HalfedgeAttributes | T2::HalfedgeAttributes );
    EdgeAttributes     ( T1::EdgeAttributes     | T2::EdgeAttributes     );
    FaceAttributes     ( T1::FaceAttributes     | T2::FaceAttributes     );


    typedef typename T1::Point    Point;
    typedef typename T1::Normal   Normal;
    typedef typename T1::Color    Color;
    typedef typename T1::TexCoord TexCoord;

    template <class Base, class Refs> class VertexT :
      public T1::template VertexT<
      typename T2::template VertexT<Base, Refs>, Refs>
    {};

    template <class Base, class Refs> class HalfedgeT :
      public T1::template HalfedgeT<
      typename T2::template HalfedgeT<Base, Refs>, Refs>
    {};


    template <class Base, class Refs> class EdgeT :
      public T1::template EdgeT<
      typename T2::template EdgeT<Base, Refs>, Refs>
    {};


    template <class Base, class Refs> class FaceT :
      public T1::template FaceT<
      typename T2::template FaceT<Base, Refs>, Refs>
    {};
  };
#endif
};


/** 
    Macro for merging two traits classes _S1 and _S2 into one traits class _D.
    Note that in case of ambiguities class _S1 overrides _S2, especially
    the point/normal/color/texcoord type to be used is taken from _S1::Point/
    _S1::Normal/_S1::Color/_S1::TexCoord.
*/
#define OM_Merge_Traits(_S1, _S2, _D) \
  typedef OpenMesh::MergeTraits<_S1, _S2>::Result _D;


/** 
    Macro for merging two traits classes _S1 and _S2 into one traits class _D.
    Same as OM_Merge_Traits, but this can be used inside template classes.
*/
#define OM_Merge_Traits_In_Template(_S1, _S2, _D) \
  typedef typename OpenMesh::MergeTraits<_S1, _S2>::Result _D;


//=============================================================================
} // namespace OpenMesh
//=============================================================================
#endif // OPENMESH_TRAITS_HH defined
//=============================================================================

