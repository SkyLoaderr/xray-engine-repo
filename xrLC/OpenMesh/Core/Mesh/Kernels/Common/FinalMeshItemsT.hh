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
//   $Revision: 1.12 $
//   $Date: 2004/01/08 15:50:42 $
//                                                                            
//=============================================================================

#ifndef OPENMESH_MESH_ITEMS_HH
#define OPENMESH_MESH_ITEMS_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Utils/GenProg.hh>
#include <OpenMesh/Core/Utils/vector_traits.hh>
#include <OpenMesh/Core/Mesh/Kernels/Common/Handles.hh>


//== NAMESPACES ===============================================================


namespace OpenMesh {


//== CLASS DEFINITION =========================================================

/// Definition of the mesh entities (items).
template 
<
  class InternalItems, 
  class Traits,
  bool  IsTriMesh
>
struct FinalMeshItemsT
{

  //--- build Refs structure ---
#ifndef DOXY_IGNORE_THIS
  struct Refs
  {
    typedef typename Traits::Point            Point;
    typedef typename vector_traits<Point>::value_type Scalar;

    typedef typename Traits::Normal           Normal;
    typedef typename Traits::Color            Color;
    typedef typename Traits::TexCoord         TexCoord;

    typedef OpenMesh::VertexHandle    VertexHandle;
    typedef OpenMesh::HalfedgeHandle  HalfedgeHandle;
    typedef OpenMesh::EdgeHandle      EdgeHandle;
    typedef OpenMesh::FaceHandle      FaceHandle;
  };
#endif



  //--- export Refs types ---

  typedef typename Refs::Point           Point;
  typedef typename Refs::Scalar          Scalar;
  typedef typename Refs::Normal          Normal;
  typedef typename Refs::Color           Color;
  typedef typename Refs::TexCoord        TexCoord;



  //--- get attribute bits from Traits ---

  enum Attribs
  { 
    VAttribs = Traits::VertexAttributes,
    HAttribs = Traits::HalfedgeAttributes,
    EAttribs = Traits::EdgeAttributes,
    FAttribs = Traits::FaceAttributes 
  };
	 
  
  
  //--- merge internal items with traits items ---

  typedef typename InternalItems::Vertex                     InternalVertex;
  typedef typename Traits::template VertexT<InternalVertex, Refs>  Vertex;


  typedef typename GenProg::IF<
    (bool)(HAttribs & Attributes::PrevHalfedge),
    typename InternalItems::Halfedge_with_prev,
    typename InternalItems::Halfedge_without_prev
  >::Result   InternalHalfedge;
  typedef typename Traits::template HalfedgeT<InternalHalfedge, Refs>
    Halfedge;


  typedef typename InternalItems::template Edge<Halfedge>      InternalEdge;
  typedef typename Traits::template EdgeT<InternalEdge, Refs>  Edge;


  typedef typename InternalItems::template Face<IsTriMesh>     InternalFace;
  typedef typename Traits::template FaceT<InternalFace, Refs>  Face;
};


//=============================================================================
} // namespace OpenMesh
//=============================================================================
#endif // OPENMESH_MESH_ITEMS_HH defined
//=============================================================================

