/*===========================================================================*\
 *                                                                           *
 *                               OpenMesh                                    *
 *        Copyright (C) 2003 by Computer Graphics Group, RWTH Aachen         *
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


//=============================================================================
//
//  Implements an importer module for arbitrary OpenMesh meshes
//
//=============================================================================


#ifndef __IMPORTERT_HH__
#define __IMPORTERT_HH__


//=== INCLUDES ================================================================


#include <OpenMesh/Core/IO/importer/BaseImporter.hh>
#include <OpenMesh/Core/Utils/vector_cast.hh>
#include <OpenMesh/Core/Attributes/Attributes.hh>


//== NAMESPACES ===============================================================


namespace OpenMesh {
namespace IO {


//=== IMPLEMENTATION ==========================================================


/**
 *  This class template provides an importer module for OpenMesh meshes.
 */
template <class Mesh>
class ImporterT : public BaseImporter
{
public:

  typedef typename Mesh::Point     Point;
  typedef typename Mesh::Normal    Normal;
  typedef typename Mesh::Color     Color;
  typedef typename Mesh::TexCoord  TexCoord;


  ImporterT(Mesh& _mesh) : mesh_(_mesh) {}


  virtual VertexHandle add_vertex(const Vec3f& _point) 
  {
    return mesh_.add_vertex(vector_cast<Point>(_point));
  }
   

  virtual FaceHandle add_face(const std::vector<VertexHandle>& _indices) 
  {
    return mesh_.add_face(_indices);
  }


  // vertex attributes

  virtual void set_normal(VertexHandle _vh, const Vec3f& _normal)
  {
    if (mesh_.has_vertex_normals())
      mesh_.set_normal(_vh, vector_cast<Normal>(_normal));
  }


  virtual void set_color(VertexHandle _vh, const Vec3uc& _color)
  {
    if (mesh_.has_vertex_colors())
      mesh_.set_color(_vh, vector_cast<Color>(_color));
  }


  virtual void set_texcoord(VertexHandle _vh, const Vec2f& _texcoord)
  {
    if (mesh_.has_vertex_texcoords())
    mesh_.set_texcoord(_vh, vector_cast<TexCoord>(_texcoord));
  }


  // face attributes

  virtual void set_normal(FaceHandle _fh, const Vec3f& _normal)
  {
    if (mesh_.has_face_normals())
      mesh_.set_normal(_fh, vector_cast<Normal>(_normal));
  }

  virtual void set_color(FaceHandle _fh, const Vec3uc& _color)
  {
    if (mesh_.has_face_colors())
      mesh_.set_color(_fh, vector_cast<Color>(_color));
  }


  // low-level access to mesh

  virtual BaseKernel& kernel() { return mesh_; }

  bool is_triangle_mesh() const
  {
    typedef typename Mesh::Face Face; // must use typedef for gcc 2.95.x
    return Face::is_triangle();
  }

  void reserve(unsigned int nV, unsigned int nE, unsigned int nF)
  {
    mesh_.reserve(nV, nE, nF);
  }

  // query number of faces, vertices, normals, texcoords
  size_t n_vertices()  const { return mesh_.n_vertices(); }   
  size_t n_faces()     const { return mesh_.n_faces(); }
  size_t n_edges()     const { return mesh_.n_edges(); }


private:

  Mesh& mesh_;
};


//=============================================================================
} // namespace IO
} // namespace OpenMesh
//=============================================================================
#endif
//=============================================================================
