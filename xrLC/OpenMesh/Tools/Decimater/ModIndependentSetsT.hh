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
//   $Revision: 1.4 $
//   $Date: 2004/01/12 17:36:30 $
//                                                                            
//=============================================================================

/** \file ModQuadricT.hh
    
 */

//=============================================================================
//
//  CLASS ModQuadricT
//
//=============================================================================

#ifndef OPENMESH_TOOLS_MODINDEPENDENTSETST_HH
#define OPENMESH_TOOLS_MODINDEPENDENTSETST_HH


//== INCLUDES =================================================================

#include <OpenMesh/Tools/Decimater/ModBaseT.hh>


//== NAMESPACE ================================================================

namespace OpenMesh { // BEGIN_NS_OPENMESH
namespace Decimater { // BEGIN_NS_DECIMATER


//== CLASS DEFINITION =========================================================


/** Lock one-ring around remaining vertex after a collapse to prevent
 *  further collapses of halfedges incident to the one-ring vertices.
 */
template <class DecimaterType>
class ModIndependentSetsT : public ModBaseT<DecimaterType>
{
public:

  DECIMATING_MODULE( ModIndependentSetsT, DecimaterType, IndependentSets );

  /// Constructor
  ModIndependentSetsT( DecimaterType &_dec ) : Base(_dec, true) {}


  /// override 
  void postprocess_collapse(const CollapseInfo& _ci)
  { 
    typename Mesh::VertexVertexIter vv_it;

    mesh().status(_ci.v1).set_locked(true);
    vv_it = mesh().vv_iter(_ci.v1);
    for (; vv_it; ++vv_it)
      mesh().status(vv_it).set_locked(true);    
  }


private:

  /// hide this method
  void set_binary(bool _b) {}
};


//=============================================================================
} // END_NS_DECIMATER
} // END_NS_OPENMESH
//=============================================================================
#endif // OPENMESH_TOOLS_MODINDEPENDENTSETST_HH defined
//=============================================================================

