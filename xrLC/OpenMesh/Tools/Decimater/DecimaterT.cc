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
//   $Revision: 1.19 $
//   $Date: 2004/01/12 17:36:30 $
//                                                                            
//=============================================================================

/** \file DecimaterT.cc
 */


//=============================================================================
//
//  CLASS DecimaterT - IMPLEMENTATION
//
//=============================================================================

#define OPENMESH_DECIMATER_DECIMATERT_CC


//== INCLUDES =================================================================

#include <OpenMesh/Tools/Decimater/DecimaterT.hh>

#include <vector>
#if defined(OM_CC_MIPS)
#  include <float.h>
#else
#  include <cfloat>
#endif


//== NAMESPACE =============================================================== 


namespace OpenMesh  {
namespace Decimater {

   
//== IMPLEMENTATION ========================================================== 

  

template <class Mesh>
DecimaterT<Mesh>::
DecimaterT( Mesh& _mesh ) 
  : mesh_(_mesh), 
    heap_(NULL), 
    cmodule_(NULL),
    initialized_(false)
{
  // default properties
  mesh_.request_vertex_status();
  mesh_.request_edge_status();
  mesh_.request_face_status();
  mesh_.request_face_normals();
  
  // private vertex properties
  mesh_.add_property( collapse_target_ );
  mesh_.add_property( priority_ );
  mesh_.add_property( heap_position_ );
}
   

//-----------------------------------------------------------------------------


template <class Mesh>
DecimaterT<Mesh>::
~DecimaterT()
{
  // default properties
  mesh_.release_vertex_status();
  mesh_.release_edge_status();
  mesh_.release_face_status();
  mesh_.release_face_normals();

  // private vertex properties
  mesh_.remove_property(collapse_target_);
  mesh_.remove_property(priority_);
  mesh_.remove_property(heap_position_);

  // dispose modules
  {
    typename ModuleList::iterator m_it, m_end = bmodules_.end();
    for( m_it=bmodules_.begin(); m_it!=m_end; ++m_it)
      delete *m_it;
    bmodules_.clear();
    if (cmodule_)
      delete cmodule_;
  }
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
DecimaterT<Mesh>::
info( std::ostream& _os )
{
  typename ModuleList::iterator m_it, m_end = bmodules_.end();

  _os << "binary modules: " << bmodules_.size() << std::endl;
  for( m_it=bmodules_.begin(); m_it!=m_end; ++m_it)
    _os << "  " << (*m_it)->name() << std::endl;

  _os << "priority module: "
      << (cmodule_ ? cmodule_->name().c_str() : "<None>") << std::endl;
  _os << "is initialized : " << (initialized_ ? "yes" : "no") << std::endl;
}


//-----------------------------------------------------------------------------


template <class Mesh> 
bool 
DecimaterT<Mesh>::
initialize()
{
  typename ModuleList::iterator m_it, m_end = bmodules_.end();

  Module *quadric=NULL;

  cmodule_ = NULL;
  for (m_it=bmodules_.begin(); m_it != m_end; ++m_it)
  {
    if ( (*m_it)->name() == "Quadric")
      quadric = *m_it;

    if ( ! (*m_it)->is_binary() )
    {
      if ( !cmodule_ ) // only one non-binary module allowed!
	cmodule_ = *m_it;
      else
	return false;
    }
    (*m_it)->initialize();
  }
   
  if (!cmodule_) // one non-binary module is mandatory!
  {
    if (!quadric)
      return false;
    else
    {
      cmodule_ = quadric; // let the quadric become the priority module
    }
  }
   

  m_it = std::find(bmodules_.begin(), bmodules_.end(), cmodule_ );
  bmodules_.erase( m_it );

  return initialized_ = true;
}


//-----------------------------------------------------------------------------

template <class Mesh>
bool
DecimaterT<Mesh>::is_collapse_legal(const CollapseInfo& _ci)
{
  //   std::clog << "DecimaterT<>::is_collapse_legal()\n";

  // locked ? deleted ?
  if (mesh_.status(_ci.v0).locked() || 
      mesh_.status(_ci.v0).deleted())
    return false;



  //--- feature test ---

  if (mesh_.status(_ci.v0).feature() &&
      !mesh_.status(mesh_.edge_handle(_ci.v0v1)).feature())
    return false;



  //--- test one ring intersection ---

  typename Mesh::VertexVertexIter  vv_it;
  
  for (vv_it = mesh_.vv_iter(_ci.v0); vv_it; ++vv_it)
    mesh_.status(vv_it).set_tagged(false);

  for (vv_it = mesh_.vv_iter(_ci.v1); vv_it; ++vv_it)
    mesh_.status(vv_it).set_tagged(true);

  for (vv_it = mesh_.vv_iter(_ci.v0); vv_it; ++vv_it)
    if (mesh_.status(vv_it).tagged() && 
	vv_it.handle() != _ci.vl && 
	vv_it.handle() != _ci.vr)
      return false;

  // if both are invalid OR equal -> fail
  if (_ci.vl == _ci.vr) return false;


  //--- test boundary cases ---

  bool b0 = mesh_.is_boundary(_ci.v0), b1 = mesh_.is_boundary(_ci.v1);

  if (b0)
  {
    // don't collapse a boundary vertex to an inner one
    if (!b1)  
      return false;

    else 
    {
      // edge between two boundary vertices has to be a boundary edge
      if (!(mesh_.is_boundary(_ci.v0v1) || 
	    mesh_.is_boundary(_ci.v1v0) ) )
	return false;
    }

    // only one one ring intersection
    if (_ci.vl.is_valid() && 
	_ci.vr.is_valid())
      return false;
  }

  // v0vl and v1vl must not both be boundary edges
  if (_ci.vl.is_valid() &&
      mesh_.is_boundary(_ci.vlv1) && 
      mesh_.is_boundary(_ci.v0v1))
    return false;
  
  // v0vr and v1vr must not be both boundary edges
  if (_ci.vr.is_valid() &&
      mesh_.is_boundary(_ci.vrv0) && 
      mesh_.is_boundary(_ci.v1vr))
    return false;
  
  // there have to be at least 2 incident faces at v0
  if (mesh_.cw_rotated_halfedge_handle(
	      mesh_.cw_rotated_halfedge_handle(_ci.v0v1)) == _ci.v0v1)
    return false;


  // collapse passed all tests -> ok
  return true;
}


//-----------------------------------------------------------------------------


template <class Mesh>
float
DecimaterT<Mesh>::collapse_priority(const CollapseInfo& _ci)
{
  typename ModuleList::iterator m_it, m_end = bmodules_.end();

  for (m_it = bmodules_.begin(); m_it != m_end; ++m_it)
  {                         
    if ( (*m_it)->collapse_priority(_ci) < 0.0)
      return -1.0; // ILLEGAL_COLLAPSE
  }     
  return cmodule_->collapse_priority(_ci);
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
DecimaterT<Mesh>::heap_vertex(VertexHandle _vh)
{
  //   std::clog << "heap_vertex: " << _vh << std::endl;

  float                           prio, best_prio(FLT_MAX);
  typename Mesh::HalfedgeHandle   heh, collapse_target;


  // find best target in one ring
  typename Mesh::VertexOHalfedgeIter voh_it(mesh_, _vh);
  for (; voh_it; ++voh_it)
  {
    heh = voh_it.handle();
    CollapseInfo  ci(mesh_, heh);

    if (is_collapse_legal(ci))
    {
      prio = collapse_priority(ci);
      if (prio >= 0.0 && prio < best_prio)
      {
	best_prio       = prio;
	collapse_target = heh;
      }
    }
  }

  // target found -> put vertex on heap
  if (collapse_target.is_valid())
  {
    //     std::clog << "  added|updated" << std::endl;
    mesh_.property(collapse_target_, _vh) = collapse_target;
    mesh_.property(priority_, _vh)        = best_prio;

    if (heap_->is_stored(_vh))  heap_->update(_vh);
    else                        heap_->insert(_vh);
  }

  // not valid -> remove from heap
  else
  {
    //     std::clog << "  n/a|removed" << std::endl;
    if (heap_->is_stored(_vh))  heap_->remove(_vh);

    mesh_.property(collapse_target_, _vh) = collapse_target;
    mesh_.property(priority_, _vh)        = -1;
  }
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
DecimaterT<Mesh>::
postprocess_collapse(CollapseInfo& _ci)
{
  typename ModuleList::iterator m_it, m_end = bmodules_.end();
  
  for (m_it = bmodules_.begin(); m_it != m_end; ++m_it)
    (*m_it)->postprocess_collapse(_ci);

  cmodule_->postprocess_collapse(_ci);
} 


//-----------------------------------------------------------------------------


template <class Mesh>
size_t
DecimaterT<Mesh>::decimate( size_t _n_collapses )
{
  if ( !is_initialized() )
    return 0;

  typename Mesh::VertexIter         v_it, v_end(mesh_.vertices_end());
  typename Mesh::VertexHandle       vp;
  typename Mesh::HalfedgeHandle     v0v1;
  typename Mesh::VertexVertexIter   vv_it;
  typename Mesh::VertexFaceIter     vf_it;
  unsigned int                      n_collapses(0);
   
  typedef std::vector<typename Mesh::VertexHandle>  Support;
  typedef typename Support::iterator                SupportIterator;
   
  Support            support(15);
  SupportIterator    s_it, s_end;


  // check _n_collapses
  if (!_n_collapses) _n_collapses = mesh_.n_vertices();


  // initialize heap
  HeapInterface  HI(mesh_, priority_, heap_position_);
  heap_ = std::auto_ptr<DeciHeap>(new DeciHeap(HI));
  heap_->reserve(mesh_.n_vertices());
  

  for (v_it = mesh_.vertices_begin(); v_it != v_end; ++v_it)
  {
    heap_->reset_heap_position( v_it.handle() );
    if (!mesh_.status(v_it).deleted())
      heap_vertex( v_it.handle() );
  }


  // process heap
  while ((!heap_->empty()) && (n_collapses < _n_collapses))
  {
    // get 1st heap entry
    vp   = heap_->front();
    v0v1 = mesh_.property(collapse_target_, vp);
    heap_->pop_front();


    // setup collapse info
    CollapseInfo ci(mesh_, v0v1);


    // check topological correctness AGAIN !
    if (!is_collapse_legal(ci))
      continue;
    

    // store support (= one ring of *vp)
    vv_it = mesh_.vv_iter(ci.v0);
    support.clear();
    for (; vv_it; ++vv_it)
      support.push_back(vv_it.handle());


    // perform collapse
    mesh_.collapse(v0v1);
    ++n_collapses;


    // update triangle normals
    vf_it = mesh_.vf_iter(ci.v1);
    for (; vf_it; ++vf_it)
      if (!mesh_.status(vf_it).deleted())
	mesh_.set_normal(vf_it, mesh_.calc_face_normal(vf_it.handle()));

    
    // post-process collapse
    postprocess_collapse(ci);


    // update heap (former one ring of decimated vertex)
    for (s_it = support.begin(), s_end = support.end();
	 s_it != s_end; ++s_it)
    {
      assert(!mesh_.status(*s_it).deleted());
      heap_vertex(*s_it);
    }
  }


  // delete heap
  heap_.reset();


  // DON'T do garbage collection here! It's up to the application.
  return n_collapses;
}


//=============================================================================
} // END_NS_DECIMATER
} // END_NS_OPENMESH
//=============================================================================

