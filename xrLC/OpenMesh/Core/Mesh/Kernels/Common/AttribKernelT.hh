/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *      Copyright (C) 2001-2003 by Computer Graphics Group, RWTH Aachen      *
 *                           www.openmesh.org                                *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *                                                                           *
 *                                License                                    *
 *                                                                           *
 *  This library is free software; you can redistribute it and/or modify it  *
 *  under the terms of the GNU Library General Public License as published   *
 *  by the Free Software Foundation, version 2.                              *
 *                                                                           *
 *  This library is distributed in the hope that it will be useful, but      *
 *  WITHOUT ANY WARRANTY; without even the implied warranty of               *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU        *
 *  Library General Public License for more details.                         *
 *                                                                           *
 *  You should have received a copy of the GNU Library General Public        *
 *  License along with this library; if not, write to the Free Software      *
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                *
 *                                                                           *
\*===========================================================================*/

#ifndef OPENMESH_ATTRIBKERNEL_HH
#define OPENMESH_ATTRIBKERNEL_HH


//== INCLUDES =================================================================

#include <OpenMesh/Core/Attributes/Attributes.hh>
#include <OpenMesh/Core/Mesh/Kernels/Common/BaseKernel.hh>
#include <OpenMesh/Core/Utils/GenProg.hh>
#include <OpenMesh/Core/Utils/vector_traits.hh>
#include <vector>


//== NAMESPACES ===============================================================

namespace OpenMesh {


//== CLASS DEFINITION =========================================================
  
/// This class adds the standard properties to the mesh type.
///
/// The attribute kernel adds all standard properties to the kernel. Therefore
/// the functions/types defined here provide a subset of the kernel
/// interface as described in Concepts::KernelT.
///
/// \see Concepts::KernelT
template <class MeshItems>
class AttribKernelT : public BaseKernel
{
public:

  //---------------------------------------------------------------- item types

  typedef typename MeshItems::Vertex             Vertex;
  typedef typename MeshItems::Halfedge           Halfedge;
  typedef typename MeshItems::Edge               Edge;
  typedef typename MeshItems::Face               Face;

  typedef typename MeshItems::Point              Point;
  typedef typename MeshItems::Normal             Normal;
  typedef typename MeshItems::Color              Color;
  typedef typename MeshItems::TexCoord           TexCoord;

  typedef typename MeshItems::Scalar             Scalar;

  typedef Attributes::StatusInfo                 StatusInfo;


  enum Attribs {
    VAttribs = MeshItems::VAttribs,
    HAttribs = MeshItems::HAttribs,
    EAttribs = MeshItems::EAttribs,
    FAttribs = MeshItems::FAttribs,
  };

  typedef GenProg::Bool2Type<(bool)(HAttribs & Attributes::PrevHalfedge)> 
    HasPrevHalfedge;



  //-------------------------------------------------- constructor / destructor

  AttribKernelT() :

    refcount_vnormals_(0),
    refcount_vcolors_(0),
    refcount_vtexcoords_(0),
    refcount_vstatus_(0),
    refcount_hstatus_(0),
    refcount_estatus_(0),
    refcount_fnormals_(0),
    refcount_fcolors_(0),
    refcount_fstatus_(0)

  {

    add_property( points_, "v:points" );

    if (VAttribs & Attributes::Normal)
      request_vertex_normals();

    if (VAttribs & Attributes::Color)
      request_vertex_colors();

    if (VAttribs & Attributes::TexCoord)
      request_vertex_texcoords();

    if (VAttribs & Attributes::Status)
      request_vertex_status();

    if (HAttribs & Attributes::Status)
      request_halfedge_status();

    if (EAttribs & Attributes::Status)
      request_edge_status();

    if (FAttribs & Attributes::Normal)
      request_face_normals();

    if (FAttribs & Attributes::Color)
      request_face_colors();

    if (FAttribs & Attributes::Status)
      request_face_status();
  }

   
  ~AttribKernelT() 
  {
    // should remove properties, but this will be done in
    // BaseKernel's destructor anyway...
  }



  // -------------------------------------------------------- copy & assignment

  AttribKernelT(const AttribKernelT& _rhs) 
    : BaseKernel(_rhs)
  { operator=(_rhs); }

  AttribKernelT& operator=(const AttribKernelT& _rhs)
  {
    // remove old properties
    remove_property(points_);
    remove_property(vertex_normals_);
    remove_property(vertex_colors_);
    remove_property(vertex_texcoords_);
    remove_property(vertex_status_);
    remove_property(halfedge_status_);
    remove_property(edge_status_);
    remove_property(face_normals_);
    remove_property(face_colors_);
    remove_property(face_status_);

    // parent deep-copies properties
    BaseKernel::operator=(_rhs);

    // copy property handles
    points_            = _rhs.points_;
    vertex_normals_    = _rhs.vertex_normals_;
    vertex_colors_     = _rhs.vertex_colors_;
    vertex_texcoords_  = _rhs.vertex_texcoords_;
    vertex_status_     = _rhs.vertex_status_;
    halfedge_status_   = _rhs.halfedge_status_;
    edge_status_       = _rhs.edge_status_;
    face_normals_      = _rhs.face_normals_;
    face_colors_       = _rhs.face_colors_;
    face_status_       = _rhs.face_status_;
    
    // copy ref-counts
    refcount_vnormals_   = _rhs.refcount_vnormals_;
    refcount_vcolors_    = _rhs.refcount_vcolors_;
    refcount_vtexcoords_ = _rhs.refcount_vtexcoords_;
    refcount_vstatus_    = _rhs.refcount_vstatus_;
    refcount_hstatus_    = _rhs.refcount_hstatus_;
    refcount_estatus_    = _rhs.refcount_estatus_;
    refcount_fnormals_   = _rhs.refcount_fnormals_;
    refcount_fcolors_    = _rhs.refcount_fcolors_;
    refcount_fstatus_    = _rhs.refcount_fstatus_;

    return *this;
  }


  //-------------------------------------------------------------------- points
 
  const Point* points() const { 
    return property(points_).data(); 
  }

  const Point& point(VertexHandle _vh) const {
    return property(points_, _vh);
  }

  Point& point(VertexHandle _vh) {
    return property(points_, _vh);
  }

  void set_point(VertexHandle _vh, const Point& _p) {
    property(points_, _vh) = _p;
  }


  //------------------------------------------------------------ vertex normals

  const Normal* vertex_normals() const {
    return property(vertex_normals_).data(); 
  }

  const Normal& normal(VertexHandle _vh) const {
    return property(vertex_normals_, _vh);
  }

  void set_normal(VertexHandle _vh, const Normal& _n) {
    property(vertex_normals_, _vh) = _n;
  }


  //------------------------------------------------------------- vertex colors

  const Color* vertex_colors() const {
    return property(vertex_colors_).data(); 
  }

  const Color& color(VertexHandle _vh) const {
    return property(vertex_colors_, _vh);
  }

  void set_color(VertexHandle _vh, const Color& _c) {
    property(vertex_colors_, _vh) = _c;
  }


  //---------------------------------------------------------- vertex texcoords

  const TexCoord* texcoords() const {
    return property(vertex_texcoords_).data(); 
  }

  const TexCoord& texcoord(VertexHandle _vh) const {
    return property(vertex_texcoords_, _vh);
  }

  void set_texcoord(VertexHandle _vh, const TexCoord& _t) {
    property(vertex_texcoords_, _vh) = _t;
  }


  //------------------------------------------------------------ vertex status

  const StatusInfo& status(VertexHandle _vh) const {
    return property(vertex_status_, _vh);
  }

  StatusInfo& status(VertexHandle _vh) {
    return property(vertex_status_, _vh);
  }


  //----------------------------------------------------------- halfedge status

  const StatusInfo& status(HalfedgeHandle _hh) const {
    return property(halfedge_status_, _hh);
  }

  StatusInfo& status(HalfedgeHandle _hh) {
    return property(halfedge_status_, _hh);
  }



  //--------------------------------------------------------------- edge status

  const StatusInfo& status(EdgeHandle _eh) const {
    return property(edge_status_, _eh);
  }

  StatusInfo& status(EdgeHandle _eh) {
    return property(edge_status_, _eh);
  }



  //--------------------------------------------------------------- face status

  const StatusInfo& status(FaceHandle _fh) const {
    return property(face_status_, _fh);
  }

  StatusInfo& status(FaceHandle _fh) {
    return property(face_status_, _fh);
  }



  //-------------------------------------------------------------- face normals

  const Normal& normal(FaceHandle _fh) const {
    return property(face_normals_, _fh);
  }

  void set_normal(FaceHandle _fh, const Normal& _n) {
    property(face_normals_, _fh) = _n;
  }



  //--------------------------------------------------------------- face colors

  const Color& color(FaceHandle _fh) const {
    return property(face_colors_, _fh);
  }

  void set_color(FaceHandle _fh, const Color& _c) {
    property(face_colors_, _fh) = _c;
  }



  //------------------------------------------------ request / alloc properties

  void request_vertex_normals() {
    if (!refcount_vnormals_++)  
      add_property( vertex_normals_, "v:normals" );
  }

  void request_vertex_colors() {
    if (!refcount_vcolors_++)
      add_property( vertex_colors_, "v:colors" );
  }

  void request_vertex_texcoords() {
    if (!refcount_vtexcoords_++)
      add_property( vertex_texcoords_, "v:texcoords" );
  }

  void request_vertex_status() {
    if (!refcount_vstatus_++)
      add_property( vertex_status_, "v:status" );
  }

  void request_halfedge_status() {
    if (!refcount_hstatus_++)
      add_property( halfedge_status_, "h:status" );
  }

  void request_edge_status() {
    if (!refcount_estatus_++)
      add_property( edge_status_, "e:status" );
  }

  void request_face_normals() {
    if (!refcount_fnormals_++)
      add_property( face_normals_, "f:normals" );
  }

  void request_face_colors() {
    if (!refcount_fcolors_++)      
      add_property( face_colors_, "f:colors" );
  }

  void request_face_status() {
    if (!refcount_fstatus_++)
      add_property( face_status_, "f:status" );
  }



  //------------------------------------------------- release / free properties

  void release_vertex_normals() {
    if ((refcount_vnormals_ > 0) && (! --refcount_vnormals_))
      remove_property(vertex_normals_);
  }

  void release_vertex_colors() {
    if ((refcount_vcolors_ > 0) && (! --refcount_vcolors_))
      remove_property(vertex_colors_);
  }

  void release_vertex_texcoords() {
    if ((refcount_vtexcoords_ > 0) && (! --refcount_vtexcoords_))
      remove_property(vertex_texcoords_);
  }

  void release_vertex_status() {
    if ((refcount_vstatus_ > 0) && (! --refcount_vstatus_))
      remove_property(vertex_status_);
  }

  void release_halfedge_status() {
    if ((refcount_hstatus_ > 0) && (! --refcount_hstatus_))
      remove_property(halfedge_status_);
  }

  void release_edge_status() {
    if ((refcount_estatus_ > 0) && (! --refcount_estatus_))
      remove_property(edge_status_);
  }

  void release_face_normals() {
    if ((refcount_fnormals_ > 0) && (! --refcount_fnormals_))
      remove_property(face_normals_);
  }

  void release_face_colors() {
    if ((refcount_fcolors_ > 0) && (! --refcount_fcolors_))
      remove_property(face_colors_);
  }

  void release_face_status() {
    if ((refcount_fstatus_ > 0) && (! --refcount_fstatus_))
      remove_property(face_status_);
  }



  //---------------------------------------------- dynamic check for properties

  bool has_vertex_normals()   const { return vertex_normals_.is_valid();   }
  bool has_vertex_colors()    const { return vertex_colors_.is_valid();    }
  bool has_vertex_texcoords() const { return vertex_texcoords_.is_valid(); }
  bool has_vertex_status()    const { return vertex_status_.is_valid();    }
  bool has_halfedge_status()  const { return halfedge_status_.is_valid();  }
  bool has_edge_status()      const { return edge_status_.is_valid();      }
  bool has_face_normals()     const { return face_normals_.is_valid();     }
  bool has_face_colors()      const { return face_colors_.is_valid();      }
  bool has_face_status()      const { return face_status_.is_valid();      }

  static bool has_prev_halfedge() {
    return (HAttribs & Attributes::PrevHalfedge); 
  }


  
protected:
  
  VPropHandleT<Point>         points_;
  VPropHandleT<Normal>        vertex_normals_;
  VPropHandleT<Color>         vertex_colors_;
  VPropHandleT<TexCoord>      vertex_texcoords_;
  VPropHandleT<StatusInfo>    vertex_status_;
  HPropHandleT<StatusInfo>    halfedge_status_;
  EPropHandleT<StatusInfo>    edge_status_;
  FPropHandleT<Normal>        face_normals_;
  FPropHandleT<Color>         face_colors_;
  FPropHandleT<StatusInfo>    face_status_;

  unsigned int  refcount_vnormals_;
  unsigned int  refcount_vcolors_;
  unsigned int  refcount_vtexcoords_;
  unsigned int  refcount_vstatus_;
  unsigned int  refcount_hstatus_;
  unsigned int  refcount_estatus_;
  unsigned int  refcount_fnormals_;
  unsigned int  refcount_fcolors_;
  unsigned int  refcount_fstatus_;
};


//=============================================================================
} // namespace OpenMesh
//=============================================================================
#endif // OPENMESH_ATTRIBKERNEL_HH defined
//=============================================================================
