//=============================================================================
//                                                                            
//                               OpenMesh                                     
//      Copyright (C) 2001-2003 by Computer Graphics Group, RWTH Aachen       
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
//   $Date: 2003/07/07 13:59:08 $
//                                                                            
//=============================================================================

//=============================================================================
//
//  CLASS StripifierT - IMPLEMENTATION
//
//=============================================================================

#define OPENMESH_STRIPIFIERT_C

//== INCLUDES =================================================================

#include <OpenMesh/Tools/Utils/StripifierT.hh>
#include <list>


//== NAMESPACES ===============================================================

namespace OpenMesh {


//== IMPLEMENTATION ==========================================================


template <class Mesh>
unsigned int
StripifierT<Mesh>::
stripify()
{
  // preprocess:  add new properties
  mesh_.add_property( processed_ );
  mesh_.add_property( used_ ); 

  // build strips
  clear();
  build_strips();


  // postprocess:  remove properties
  mesh_.remove_property(processed_);
  mesh_.remove_property(used_);


  return n_strips();
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
StripifierT<Mesh>::
build_strips()
{
  Strip                           experiments[3];
  typename Mesh::HalfedgeHandle   h[3];
  unsigned int                    best_idx, best_length, length;
  FaceHandles                     faces[3];
  typename FaceHandles::iterator  fh_it, fh_end;
  typename Mesh::FaceIter         f_it, f_end=mesh_.faces_end();

  

  // init faces to be un-processed and un-used
  for (f_it=mesh_.faces_begin(); f_it!=f_end; ++f_it)
    processed(f_it) = used(f_it) = false;



  for (f_it=mesh_.faces_begin(); true; )
  {
    // find start face
    for (; f_it!=f_end; ++f_it)
      if (!processed(f_it))
	break;
    if (f_it==f_end) break; // stop if all have been processed


    // collect starting halfedges
    h[0] = mesh_.halfedge_handle(f_it.handle());
    h[1] = mesh_.next_halfedge_handle(h[0]);
    h[2] = mesh_.next_halfedge_handle(h[1]);


    // build 3 strips, take best one
    best_length = best_idx = 0;
    for (unsigned int i=0; i<3; ++i)
    {
      build_strip(h[i], experiments[i], faces[i]);
      if ((length = experiments[i].size()) > best_length) 
      {
	best_length = length;
	best_idx    = i;
      }

      for (fh_it=faces[i].begin(), fh_end=faces[i].end(); 
	   fh_it!=fh_end; ++fh_it)
	used(*fh_it) = false;
    }


    // update processed status
    fh_it  = faces[best_idx].begin();
    fh_end = faces[best_idx].end();
    for (; fh_it!=fh_end; ++fh_it)
      processed(*fh_it) = true;


    
    // add best strip to strip-list
    strips_.push_back(experiments[best_idx]);
  }
}


//-----------------------------------------------------------------------------


template <class Mesh>
void
StripifierT<Mesh>::
build_strip(typename Mesh::HalfedgeHandle _start_hh,
	    Strip& _strip,
	    FaceHandles& _faces)
{
  std::list<unsigned int>  strip;
  typename Mesh::HalfedgeHandle   hh;
  typename Mesh::FaceHandle       fh;


  // reset face list
  _faces.clear();


  // init strip
  strip.push_back(mesh_.from_vertex_handle(_start_hh).idx());
  strip.push_back(mesh_.to_vertex_handle(_start_hh).idx());


  // walk along the strip: 1st direction
  hh = mesh_.prev_halfedge_handle(mesh_.opposite_halfedge_handle(_start_hh));
  while (1)
  {
    // go right
    hh = mesh_.next_halfedge_handle(hh);
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break; 
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_back(mesh_.to_vertex_handle(hh).idx());

    // go left
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break; 
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_back(mesh_.to_vertex_handle(hh).idx());
  }


  // walk along the strip: 2nd direction
  bool flip(false);
  hh = mesh_.prev_halfedge_handle(_start_hh);
  while (1)
  {
    // go right
    hh = mesh_.next_halfedge_handle(hh);
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break; 
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_front(mesh_.to_vertex_handle(hh).idx());
    flip = true;

    // go left
    hh = mesh_.opposite_halfedge_handle(hh);
    hh = mesh_.next_halfedge_handle(hh);
    if (mesh_.is_boundary(hh)) break;
    fh = mesh_.face_handle(hh);
    if (processed(fh) || used(fh)) break; 
    _faces.push_back(fh);
    used(fh) = true;
    strip.push_front(mesh_.to_vertex_handle(hh).idx());
    flip = false;
  }

  if (flip) strip.push_front(strip.front());



  // copy final strip to _strip
  _strip.clear();
  _strip.reserve(strip.size());
  std::copy(strip.begin(), strip.end(), std::back_inserter(_strip));
}


//=============================================================================
} // namespace OpenMesh
//=============================================================================
