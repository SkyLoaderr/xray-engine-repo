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
//   $Revision: 1.2 $
//   $Date: 2003/04/08 10:14:21 $
//                                                                            
//=============================================================================


//=============================================================================
//
//  CLASS BaseMesh
//
//=============================================================================


#ifndef OPENMESH_BASEMESH_HH
#define OPENMESH_BASEMESH_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Mesh/Iterators/IteratorsT.hh>
#include <OpenMesh/Core/Mesh/Iterators/CirculatorsT.hh>
#include <OpenMesh/Core/Attributes/Attributes.hh>
#include <vector>


//== NAMESPACES ===============================================================


namespace OpenMesh {


//== CLASS DEFINITION =========================================================


/** \class BaseMesh BaseMesh.hh <OpenMesh/Mesh/BaseMesh.hh>

    Base class for all meshes.
*/

class BaseMesh {
public:
  virtual ~BaseMesh(void) {;}
};


//=============================================================================
} // namespace OpenMesh
//=============================================================================

//=============================================================================
#endif // OPENMESH_BASEMESH_HH defined
//=============================================================================
