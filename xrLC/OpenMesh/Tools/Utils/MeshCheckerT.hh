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
//   $Date: 2004/01/13 15:23:32 $
//                                                                            
//=============================================================================


#ifndef OPENMESH_MESHCHECKER_HH
#define OPENMESH_MESHCHECKER_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/System/omstream.hh>
#include <OpenMesh/Core/Utils/GenProg.hh>
#include <OpenMesh/Core/Attributes/Attributes.hh>
#include <iostream>


//== NAMESPACES ===============================================================


namespace OpenMesh {
namespace Utils {

//== CLASS DEFINITION =========================================================

	      
/** Check integrity of mesh.
 *
 *  This class provides several functions to check the integrity of a mesh.
 */
template <class Mesh>
class MeshCheckerT
{
public:
   
  /// constructor
  MeshCheckerT(const Mesh& _mesh) : mesh_(_mesh) {}
 
  /// destructor
  ~MeshCheckerT() {}


  /// what should be checked?
  enum CheckTargets
  {
    CHECK_EDGES     = 1,
    CHECK_VERTICES  = 2,
    CHECK_FACES     = 4,
    CHECK_ALL       = 255,
  };

  
  /// check it, return true iff ok
  bool check( unsigned int _targets=CHECK_ALL,
	      std::ostream&  _os=omerr );


private:

  bool is_deleted(typename Mesh::VertexHandle _vh) 
  { return (mesh_.has_vertex_status() ? mesh_.status(_vh).deleted() : false); }

  bool is_deleted(typename Mesh::EdgeHandle _eh) 
  { return (mesh_.has_edge_status() ? mesh_.status(_eh).deleted() : false); }

  bool is_deleted(typename Mesh::FaceHandle _fh) 
  { return (mesh_.has_face_status() ? mesh_.status(_fh).deleted() : false); }


  // ref to mesh
  const Mesh&  mesh_;
};


//=============================================================================
} // namespace Utils
} // namespace OpenMesh
//=============================================================================
#if defined(OM_INCLUDE_TEMPLATES) && !defined(OPENMESH_MESHCHECKER_C)
#define OPENMESH_MESHCHECKER_TEMPLATES
#include "MeshCheckerT.cc"
#endif
//=============================================================================
#endif // OPENMESH_MESHCHECKER_HH defined
//=============================================================================

