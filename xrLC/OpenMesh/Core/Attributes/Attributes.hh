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
//   $Revision: 1.11 $
//   $Date: 2004/01/13 15:27:50 $
//                                                                            
//=============================================================================


/** 
    \file Attributes.hh
    This file provides some macros containing attribute usage.
*/


#ifndef OPENMESH_ATTRIBUTES_HH
#define OPENMESH_ATTRIBUTES_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Attributes/Status.hh>


//== NAMESPACES ===============================================================


namespace OpenMesh {
namespace Attributes {

 
//== CLASS DEFINITION  ========================================================

/** Attribute bits
 * 
 *  Use the bits to define a standard property at compile time using traits.
 *
 *  \include traits5.cc
 *
 *  \see \ref mesh_type
 */
enum AttributeBits 
{ 
  None          = 0,  ///< Clear all attribute bits
  Normal        = 1,  ///< Add normals to mesh item (vertices/faces)
  Color         = 2,  ///< Add colors to mesh item (vertices/faces)
  PrevHalfedge  = 4,  ///< Add storage for previous halfedge (halfedges). The bit is set by default in the DefaultTraits.
  Status        = 8,  ///< Add status to mesh item (all items)
  TexCoord      = 16  ///< Add texture coordinates (vertices)
};


//=============================================================================
} // namespace Attributes
} // namespace OpenMesh
//=============================================================================
#endif // OPENMESH_ATTRIBUTES_HH defined
//=============================================================================
