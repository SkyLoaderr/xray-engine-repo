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
//   $Date: 2004/01/08 15:51:04 $
//                                                                            
//=============================================================================


//=============================================================================
//
//  CLASS ArrayKernelT
//
//=============================================================================


#ifndef OPENMESH_ARRAY_KERNEL_HH
#define OPENMESH_ARRAY_KERNEL_HH


//== INCLUDES =================================================================


#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Mesh/Kernels/Common/AttribKernelT.hh>
#include <OpenMesh/Core/Utils/GenProg.hh>
#include <vector>



//== NAMESPACES ===============================================================


namespace OpenMesh {


//== CLASS DEFINITION =========================================================


/** \ingroup mesh_kernels_group
    
    Mesh kernel using arrays for mesh item storage.

    This mesh kernel uses the std::vector as container to store the
    mesh items. Therefore all handle types are internally represented
    by integers. To get the index from a handle use the handle's \c
    idx() method.

    \note For a description of the minimal kernel interface see
    OpenMesh::Mesh::BaseKernel.
    \note You do not have to use this class directly, use the predefined
    mesh-kernel combinations in \ref mesh_types_group.
    \see OpenMesh::Concepts::KernelT, \ref mesh_type 
*/

template <class AttribKernel, class FinalMeshItems>
class ArrayKernelT : public AttribKernel
{
public:
  
  typedef ArrayKernelT<AttribKernel, FinalMeshItems>  This;
  typedef AttribKernel                                Base;

  // attributes
  typedef typename Base::HasPrevHalfedge              HasPrevHalfedge;

  // item types
  typedef typename FinalMeshItems::Vertex             Vertex;
  typedef typename FinalMeshItems::Halfedge           Halfedge;
  typedef typename FinalMeshItems::Edge               Edge;
  typedef typename FinalMeshItems::Face               Face;
  typedef typename FinalMeshItems::Point              Point;
  typedef typename FinalMeshItems::Scalar             Scalar;

  // handles
  typedef OpenMesh::VertexHandle       VertexHandle;    
  typedef OpenMesh::HalfedgeHandle     HalfedgeHandle;  
  typedef OpenMesh::EdgeHandle         EdgeHandle;      
  typedef OpenMesh::FaceHandle         FaceHandle;      



  // --- constructor/destructor ---


  ArrayKernelT() {}
  ~ArrayKernelT() { clear(); }



  // --- handle -> item ---

  VertexHandle handle(const Vertex& _v) const  { 
    return VertexHandle(&_v - &vertices_.front());
  }

  HalfedgeHandle handle(const Halfedge& _he) const {
    unsigned int eh(((char*)&edges_.front() - (char*)&_he) % sizeof(Edge));
    assert((&_he == &edges_[eh].halfedges_[0]) || 
	   (&_he == &edges_[eh].halfedges_[1]));
    return ((&_he == &edges_[eh].halfedges_[0]) ? 
	    HalfedgeHandle(eh<<1) : 
	    HalfedgeHandle((eh<<1)+1));
  }

  EdgeHandle handle(const Edge& _e) const {
    return EdgeHandle(&_e - &edges_.front());
  }

  FaceHandle handle(const Face& _f) const {
    return FaceHandle(&_f - &faces_.front());
  }



#define SIGNED(x) signed( (x) )


  // --- item -> handle ---
  
  const Vertex& vertex(VertexHandle _vh) const { 
    assert(_vh.idx() >= 0 && _vh.idx() < SIGNED(n_vertices())); 
    return vertices_[_vh.idx()];
  }

  Vertex& vertex(VertexHandle _vh) { 
    assert(_vh.idx() >= 0 && _vh.idx() < SIGNED(n_vertices())); 
    return vertices_[_vh.idx()];
  }


  const Halfedge& halfedge(HalfedgeHandle _heh) const { 
    assert(_heh.idx() >= 0 && _heh.idx() < SIGNED(n_edges()*2));
    return edges_[_heh.idx() >> 1].halfedges_[_heh.idx() & 1];
  }

  Halfedge& halfedge(HalfedgeHandle _heh) { 
    assert(_heh.idx() >= 0 && _heh.idx() < SIGNED(n_edges())*2);
    return edges_[_heh.idx() >> 1].halfedges_[_heh.idx() & 1];
  }


  const Edge& edge(EdgeHandle _eh) const { 
    assert(_eh.idx() >= 0 && _eh.idx() < SIGNED(n_edges()));
    return edges_[_eh.idx()];
  }

  Edge& edge(EdgeHandle _eh) { 
    assert(_eh.idx() >= 0 && _eh.idx() < SIGNED(n_edges()));
    return edges_[_eh.idx()];
  }


  const Face& face(FaceHandle _fh) const { 
    assert(_fh.idx() >= 0 && _fh.idx() < SIGNED(n_faces()));
    return faces_[_fh.idx()]; 
  }

  Face& face(FaceHandle _fh) { 
    assert(_fh.idx() >= 0 && _fh.idx() < SIGNED(n_faces()));
    return faces_[_fh.idx()]; 
  }

#undef SIGNED



  // --- get i'th items ---

  VertexHandle vertex_handle(unsigned int _i) const {
    return (_i < n_vertices()) ? handle( vertices_[_i] ) : VertexHandle();
  }
 
  HalfedgeHandle halfedge_handle(unsigned int _i) const {
    return (_i < n_halfedges()) ? halfedge_handle(edge_handle(_i/2), _i%2) : HalfedgeHandle();
  }

  EdgeHandle edge_handle(unsigned int _i) const {
    return (_i < n_edges()) ? handle(edges_[_i]) : EdgeHandle();
  }

  FaceHandle face_handle(unsigned int _i) const {
    return (_i < n_faces()) ? handle(faces_[_i]) : FaceHandle();
  }


  // --- add new items ---

  void reserve( unsigned int _n_vertices,
		unsigned int _n_edges,
		unsigned int _n_faces ) {
    vertices_.reserve(_n_vertices);
    edges_.reserve(_n_edges);  
    faces_.reserve(_n_faces);

    vprops_reserve(_n_vertices);
    hprops_reserve(_n_edges*2);
    eprops_reserve(_n_edges);
    fprops_reserve(_n_faces);
  }


public:

  VertexHandle new_vertex() {    
    vertices_.push_back(Vertex());
    vprops_resize(n_vertices());

    return handle(vertices_.back());
  }


  VertexHandle new_vertex(const Point& _p) {
    vertices_.push_back(Vertex());
    vprops_resize(n_vertices());

    VertexHandle vh(handle(vertices_.back()));
    set_point(vh, _p);
    return vh;
  }


  HalfedgeHandle new_edge(VertexHandle _start_vertex_handle, 
			  VertexHandle _end_vertex_handle) 
  {
    edges_.push_back(Edge());
    eprops_resize(n_edges());
    hprops_resize(n_halfedges());

    EdgeHandle eh(handle(edges_.back()));
    HalfedgeHandle heh0(halfedge_handle(eh, 0));
    HalfedgeHandle heh1(halfedge_handle(eh, 1));
    set_vertex_handle(heh0, _end_vertex_handle);
    set_vertex_handle(heh1, _start_vertex_handle);
    return heh0;
  }


  FaceHandle new_face() {
    faces_.push_back(Face());
    fprops_resize(n_faces());
    return handle(faces_.back());
  }

  FaceHandle new_face(const Face& _f) {
    faces_.push_back(_f);
    fprops_resize(n_faces());
    return handle(faces_.back());
  }

public:


  
  // --- deletion ---

  void garbage_collection(bool _v=true, bool _e=true, bool _f=true);

  void clear() {
    vertices_.clear();
    edges_.clear();
    faces_.clear();

    vprops_resize(0);
    eprops_resize(0);
    hprops_resize(0);
    fprops_resize(0);
  }

  void resize( unsigned int _n_vertices,
	       unsigned int _n_edges,
	       unsigned int _n_faces ) 
  {
    vertices_.resize(_n_vertices);
    edges_.resize(_n_edges);
    faces_.resize(_n_faces);

    vprops_resize(n_vertices());
    hprops_resize(n_halfedges());
    eprops_resize(n_edges());
    fprops_resize(n_faces());
  }




  // --- number of items ---

  unsigned int n_vertices()  const { return vertices_.size(); }
  unsigned int n_halfedges() const { return 2*edges_.size(); }
  unsigned int n_edges()     const { return edges_.size(); }
  unsigned int n_faces()     const { return faces_.size(); }

  bool vertices_empty()  const { return vertices_.empty(); }
  bool halfedges_empty() const { return edges_.empty(); }
  bool edges_empty()     const { return edges_.empty(); }
  bool faces_empty()     const { return faces_.empty(); }





  // --- vertex connectivity ---

  HalfedgeHandle halfedge_handle(VertexHandle _vh) const { 
    return vertex(_vh).halfedge_handle_; 
  }

  void set_halfedge_handle(VertexHandle _vh, HalfedgeHandle _heh) { 
    vertex(_vh).halfedge_handle_ = _heh;
  }


 
  // --- halfedge connectivity ---


  VertexHandle to_vertex_handle(HalfedgeHandle _heh) const { 
    return halfedge(_heh).vertex_handle_; 
  }

  VertexHandle from_vertex_handle(HalfedgeHandle _heh) const { 
    return to_vertex_handle(opposite_halfedge_handle(_heh));
  }

  void set_vertex_handle(HalfedgeHandle _heh, VertexHandle _vh) {
    halfedge(_heh).vertex_handle_ = _vh;  
  }



  FaceHandle face_handle(HalfedgeHandle _heh) const { 
    return halfedge(_heh).face_handle_; 
  }

  void set_face_handle(HalfedgeHandle _heh, FaceHandle _fh) { 
    halfedge(_heh).face_handle_ = _fh;  
  }


  HalfedgeHandle next_halfedge_handle(HalfedgeHandle _heh) const { 
    return halfedge(_heh).next_halfedge_handle_; 
  }

  void set_next_halfedge_handle(HalfedgeHandle _heh, HalfedgeHandle _nheh) { 
    halfedge(_heh).next_halfedge_handle_ = _nheh;
    set_prev_halfedge_handle(_nheh, _heh);
  }


  void set_prev_halfedge_handle(HalfedgeHandle _heh, HalfedgeHandle _pheh) {
    set_prev_halfedge_handle(_heh, _pheh, HasPrevHalfedge());
  }

  void set_prev_halfedge_handle(HalfedgeHandle _heh, HalfedgeHandle _pheh,
				GenProg::True) { 
    halfedge(_heh).prev_halfedge_handle_ = _pheh;
  }
  void set_prev_halfedge_handle(HalfedgeHandle _heh, HalfedgeHandle _pheh,
				GenProg::False) {}


  HalfedgeHandle prev_halfedge_handle(HalfedgeHandle _heh) const { 
    return prev_halfedge_handle(_heh, HasPrevHalfedge() );
  }

  HalfedgeHandle prev_halfedge_handle(HalfedgeHandle _heh,
				      GenProg::True) const { 
    return halfedge(_heh).prev_halfedge_handle_; 
  }

  HalfedgeHandle prev_halfedge_handle(HalfedgeHandle _heh, 
				      GenProg::False) const {
    HalfedgeHandle  heh(_heh);
    HalfedgeHandle  next_heh(next_halfedge_handle(heh));
    while (next_heh != _heh) {
      heh = next_heh;
      next_heh = next_halfedge_handle(next_heh);
    }
    return heh;
  }


  HalfedgeHandle opposite_halfedge_handle(HalfedgeHandle _heh) const { 
    return HalfedgeHandle((_heh.idx() & 1) ? _heh.idx()-1 : _heh.idx()+1);
  }


  HalfedgeHandle ccw_rotated_halfedge_handle(HalfedgeHandle _heh) const {
    return opposite_halfedge_handle(prev_halfedge_handle(_heh));
  }


  HalfedgeHandle cw_rotated_halfedge_handle(HalfedgeHandle _heh) const {
    return next_halfedge_handle(opposite_halfedge_handle(_heh));
  }



  // --- edge connectivity ---


  HalfedgeHandle halfedge_handle(EdgeHandle _eh, unsigned int _i) const {
    assert(_i<=1);
    return HalfedgeHandle((_eh.idx() << 1) + _i);
  }
  

  EdgeHandle edge_handle(HalfedgeHandle _heh) const {
    return EdgeHandle(_heh.idx() >> 1);
  }



  // --- face connectivity ---


  HalfedgeHandle halfedge_handle(FaceHandle _fh) const { 
    return face(_fh).halfedge_handle_; 
  }

  void set_halfedge_handle(FaceHandle _fh, HalfedgeHandle _heh) { 
    face(_fh).halfedge_handle_ = _heh;
  }



private:

  // iterators
  typedef std::vector<Vertex>                         VertexContainer;
  typedef std::vector<Edge>                           EdgeContainer;
  typedef std::vector<Face>                           FaceContainer;
  typedef typename VertexContainer::iterator          KernelVertexIter;
  typedef typename VertexContainer::const_iterator    KernelConstVertexIter;
  typedef typename EdgeContainer::iterator            KernelEdgeIter;
  typedef typename EdgeContainer::const_iterator      KernelConstEdgeIter;
  typedef typename FaceContainer::iterator            KernelFaceIter;
  typedef typename FaceContainer::const_iterator      KernelConstFaceIter;


  KernelVertexIter vertices_begin()             { return vertices_.begin(); }
  KernelConstVertexIter vertices_begin() const  { return vertices_.begin(); }
  KernelVertexIter vertices_end()               { return vertices_.end(); }
  KernelConstVertexIter vertices_end() const    { return vertices_.end(); }

  KernelEdgeIter edges_begin()                  { return edges_.begin(); }
  KernelConstEdgeIter edges_begin() const       { return edges_.begin(); }
  KernelEdgeIter edges_end()                    { return edges_.end(); }
  KernelConstEdgeIter edges_end() const         { return edges_.end(); }

  KernelFaceIter faces_begin()                  { return faces_.begin(); }
  KernelConstFaceIter faces_begin() const       { return faces_.begin(); }
  KernelFaceIter faces_end()                    { return faces_.end(); }
  KernelConstFaceIter faces_end() const         { return faces_.end(); }


private:

  VertexContainer    vertices_;
  EdgeContainer      edges_;
  FaceContainer      faces_;
};


//-----------------------------------------------------------------------------


template <class AttribKernel, class FinalMeshItems>
void
ArrayKernelT<AttribKernel, FinalMeshItems>::
garbage_collection(bool _v, bool _e, bool _f)
{
  int            i, i0, i1,
                 nV(n_vertices()), 
                 nE(n_edges()), 
                 nH(2*n_edges()), 
                 nF(n_faces());

  std::vector<VertexHandle>    vh_map;
  std::vector<HalfedgeHandle>  hh_map;
  std::vector<FaceHandle>      fh_map;

  // setup handle mapping:
  // on 1st pos is an invalid handle, all others are hence shifted by 1

  vh_map.reserve(nV+1);
  for (i=-1; i<nV; ++i) vh_map.push_back(VertexHandle(i));

  hh_map.reserve(nH+1);
  for (i=-1; i<nH; ++i) hh_map.push_back(HalfedgeHandle(i));

  fh_map.reserve(nF+1);
  for (i=-1; i<nF; ++i) fh_map.push_back(FaceHandle(i));



  // remove deleted vertices
  if (_v && n_vertices() > 0)
  {
    i0=0;  i1=nV-1;
    
    while (1)
    {
      // find 1st deleted and last un-deleted
      while (!status(VertexHandle(i0)).deleted() && i0 < i1)  ++i0;
      while ( status(VertexHandle(i1)).deleted() && i0 < i1)  --i1;
      if (i0 >= i1) break;

      // swap
      std::swap(vertices_[i0], vertices_[i1]);
      std::swap(vh_map[i0+1],  vh_map[i1+1]);
      vprops_swap(i0, i1);
    };

    vertices_.resize(status(VertexHandle(i0)).deleted() ? i0 : i0+1);
    vprops_resize(n_vertices());
  }


  // remove deleted edges
  if (_e && n_edges() > 0)
  {
    i0=0;  i1=nE-1;
    
    while (1)
    {
      // find 1st deleted and last un-deleted
      while (!status(EdgeHandle(i0)).deleted() && i0 < i1)  ++i0;
      while ( status(EdgeHandle(i1)).deleted() && i0 < i1)  --i1;
      if (i0 >= i1) break;

      // swap
      std::swap(edges_[i0], edges_[i1]);
      std::swap(hh_map[2*i0+1], hh_map[2*i1+1]);
      std::swap(hh_map[2*i0+2], hh_map[2*i1+2]);
      eprops_swap(i0, i1);
      hprops_swap(2*i0,   2*i1);
      hprops_swap(2*i0+1, 2*i1+1);
    };

    edges_.resize(status(EdgeHandle(i0)).deleted() ? i0 : i0+1);
    eprops_resize(n_edges());
    hprops_resize(n_halfedges());
  }


  // remove deleted faces
  if (_f && n_faces() > 0)
  {
    i0=0;  i1=nF-1;
    
    while (1)
    {
      // find 1st deleted and last un-deleted
      while (!status(FaceHandle(i0)).deleted() && i0 < i1)  ++i0;
      while ( status(FaceHandle(i1)).deleted() && i0 < i1)  --i1;
      if (i0 >= i1) break;

      // swap
      std::swap(faces_[i0],   faces_[i1]);
      std::swap(fh_map[i0+1], fh_map[i1+1]);
      fprops_swap(i0, i1);
    };

    faces_.resize(status(FaceHandle(i0)).deleted() ? i0 : i0+1);
    fprops_resize(n_faces());
  }


  // update handles of vertices
  if (_e)
  {
    KernelVertexIter v_it(vertices_begin()), v_end(vertices_end());
    VertexHandle     vh;

    for (; v_it!=v_end; ++v_it)
    {
      vh = handle(*v_it);
      set_halfedge_handle(vh, hh_map[halfedge_handle(vh).idx()+1]);
    }
  }


  // update handles of halfedges
  KernelEdgeIter  e_it(edges_begin()), e_end(edges_end());
  HalfedgeHandle  hh;

  for (; e_it!=e_end; ++e_it)
  {
    hh = halfedge_handle(handle(*e_it), 0);
    set_next_halfedge_handle(hh, hh_map[next_halfedge_handle(hh).idx()+1]);
    set_vertex_handle(hh, vh_map[to_vertex_handle(hh).idx()+1]);
    set_face_handle(hh, fh_map[face_handle(hh).idx()+1]);

    hh = halfedge_handle(handle(*e_it), 1);
    set_next_halfedge_handle(hh, hh_map[next_halfedge_handle(hh).idx()+1]);
    set_vertex_handle(hh, vh_map[to_vertex_handle(hh).idx()+1]);
    set_face_handle(hh, fh_map[face_handle(hh).idx()+1]);
  }


  // update handles of faces
  if (_e)
  {
    KernelFaceIter  f_it(faces_begin()), f_end(faces_end());
    FaceHandle      fh;

    for (; f_it!=f_end; ++f_it)
    {
      fh = handle(*f_it);
      set_halfedge_handle(fh, hh_map[halfedge_handle(fh).idx()+1]);
    }
  }

}


//=============================================================================
} // namespace OpenMesh
//=============================================================================
#endif // OPENMESH_ARRAY_KERNEL_HH defined
//=============================================================================
