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


//== INCLUDES =================================================================

#include <OpenMesh/Core/System/config.h>
// -------------------- STL
#if defined( OM_CC_MIPS )
#  include <time.h>
#  include <string.h>
#else
#  include <ctime>
#  include <cstring>
#endif
#include <fstream>
// -------------------- OpenMesh
#include <OpenMesh/Core/IO/OMFormat.hh>
#include <OpenMesh/Core/System/omstream.hh>
#include <OpenMesh/Core/Utils/Endian.hh>
#include <OpenMesh/Core/IO/exporter/BaseExporter.hh>
#include <OpenMesh/Core/IO/writer/OMWriter.hh>

//=== NAMESPACES ==============================================================


namespace OpenMesh {
namespace IO {


//=== INSTANCIATE =============================================================


// register the OMLoader singleton with MeshLoader
static BaseWriter* OMWriterInstance = &OMWriter::Instance();


//=== IMPLEMENTATION ==========================================================


const OMFormat::uchar _OMWriter_::magic_[3] = "OM";
const OMFormat::uint8 _OMWriter_::version_  = OMFormat::mk_version(1,2);


_OMWriter_::
_OMWriter_() 
{ 
  IOManager::Instance().register_module(this); 
}


bool
_OMWriter_::write(const std::string& _filename, BaseExporter& _be,
                   Options _opt) const
{
  // check for om extension in filename, we can only handle OM  
  if (_filename.rfind(".om") == std::string::npos)  
    return false;

  _opt += Options::Binary; // only binary format supported

  std::ofstream ofs(_filename.c_str(), std::ios::binary);

  // check if file is open
  if (!ofs.is_open())
  {
    omerr << "[OMWriter] : cannot open file " << _filename << std::endl;
    return false;
  }

  // call stream save method
  bool rc = write(ofs, _be, _opt);
  
  // close filestream
  ofs.close();

  // return success/failure notice
  return rc;
}


//-----------------------------------------------------------------------------

bool
_OMWriter_::write(std::ostream& _os, BaseExporter& _be, Options _opt) const
{
//   std::clog << "[OMWriter]::write( stream )\n";

  // check exporter features
  if ( !check( _be, _opt ) )
  {
    omerr << "[OMWriter]: exporter does not support wanted feature!\n";
    return false;
  }

  // Maybe an ascii version will be implemented in the future.
  // For now, support only a binary format
  if ( !_opt.check( Options::Binary ) )
    _opt += Options::Binary;

  // Ignore LSB/MSB bit. Always store in LSB (little endian)
  _opt += Options::LSB;
  _opt -= Options::MSB;

//    if ( _opt.check(Options::Binary) )
//    {
  return write_binary(_os, _be, _opt);
//    }
//    else
//    {
//       return write_ascii(_os, _be, _opt);
//    }
}


//-----------------------------------------------------------------------------


// bool _OMWriter_::write_ascii(std::ostream& _os, BaseExporter& _be,
//                               Options _opt) const
// {
//    return false;
// }

//-----------------------------------------------------------------------------


#ifndef DOXY_IGNORE_THIS
template <typename T> struct Enabler
{
  Enabler( T& obj ) : obj_(obj)
  {}

  ~Enabler() { obj_.enable(); }

  T& obj_;
};
#endif


bool _OMWriter_::write_binary(std::ostream& _os, BaseExporter& _be,
                               Options _opt) const
{
  Enabler<mostream> enabler(omlog);

  omlog << "[OMWriter] : write binary file\n";

  size_t bytes = 0;

  bool swap = _opt.check(Options::Swap) || (Endian::local() == Endian::MSB);

  unsigned int i, nV, nF;
  Vec3f v;
  Vec2f t;
  std::vector<VertexHandle> vhandles;


  // -------------------- write header
  OMFormat::Header header;
  
  header.magic_[0]   = 'O';
  header.magic_[1]   = 'M';
  header.mesh_       = _be.is_triangle_mesh() ? 'T' : 'P';
  header.version_    = version_;
  header.n_vertices_ = _be.n_vertices();
  header.n_faces_    = _be.n_faces();
  header.n_edges_    = _be.n_edges();
  
  bytes += store( _os, header, swap );
  
  // ---------------------------------------- write chunks
  
  OMFormat::Chunk::Header chunk_header;
  
  
  // -------------------- write vertex data
  
  // ---------- write vertex position
  if (_be.n_vertices())
  {
    v = _be.point(VertexHandle(0));
    chunk_header.reserved_ = 0;
    chunk_header.name_     = false;
    chunk_header.entity_   = OMFormat::Chunk::Entity_Vertex;
    chunk_header.type_     = OMFormat::Chunk::Type_Pos;
    chunk_header.signed_   = OMFormat::is_signed(v[0]);
    chunk_header.float_    = OMFormat::is_float(v[0]);
    chunk_header.dim_      = OMFormat::dim(v);
    chunk_header.bits_     = OMFormat::bits(v[0]);
  
    bytes += store( _os, chunk_header, swap );
    for (i=0, nV=_be.n_vertices(); i<nV; ++i)
      bytes += vector_store( _os, _be.point(VertexHandle(i)), swap );
  }


  // ---------- write vertex normal
  if (_be.n_vertices() && _opt.check( Options::VertexNormal ))
  {
    Vec3f n = _be.normal(VertexHandle(0));

    chunk_header.name_     = false;
    chunk_header.entity_   = OMFormat::Chunk::Entity_Vertex;
    chunk_header.type_     = OMFormat::Chunk::Type_Normal;
    chunk_header.signed_   = OMFormat::is_signed(n[0]);
    chunk_header.float_    = OMFormat::is_float(n[0]);
    chunk_header.dim_      = OMFormat::dim(n);
    chunk_header.bits_     = OMFormat::bits(n[0]);
  
    bytes += store( _os, chunk_header, swap );
    for (i=0, nV=_be.n_vertices(); i<nV; ++i)
      bytes += vector_store( _os, _be.normal(VertexHandle(i)), swap );
  }

  // ---------- write vertex color
#if 1
  if (_opt.check( Options::VertexColor ) && _be.has_vertex_colors() )
  {  
    Vec3uc c = _be.color(VertexHandle(0));
    
    chunk_header.name_     = false;
    chunk_header.entity_   = OMFormat::Chunk::Entity_Vertex;
    chunk_header.type_     = OMFormat::Chunk::Type_Color;
    chunk_header.signed_   = OMFormat::is_signed( c );
    chunk_header.float_    = OMFormat::is_float( c );
    chunk_header.dim_      = OMFormat::dim( c );
    chunk_header.bits_     = OMFormat::bits( c );    

    bytes += store( _os, chunk_header, swap );
    for (i=0, nV=_be.n_vertices(); i<nV; ++i)
      bytes += vector_store( _os, _be.color(VertexHandle(i)), swap );
  }
#endif

  // ---------- write vertex texture coords
  if (_be.n_vertices() && _opt.check(Options::VertexTexCoord))
  {
    t = _be.texcoord(VertexHandle(0));

    chunk_header.name_     = false;
    chunk_header.entity_   = OMFormat::Chunk::Entity_Vertex;
    chunk_header.type_     = OMFormat::Chunk::Type_Texcoord;
    chunk_header.signed_   = OMFormat::is_signed(t[0]);
    chunk_header.float_    = OMFormat::is_float(t[0]);
    chunk_header.dim_      = OMFormat::dim(t);
    chunk_header.bits_     = OMFormat::bits(t[0]);
  
    // std::clog << chunk_header << std::endl;
    bytes += store( _os, chunk_header, swap );

    for (i=0, nV=_be.n_vertices(); i<nV; ++i)
      bytes += vector_store( _os, _be.texcoord(VertexHandle(i)), swap );
  }

  // -------------------- write face data

  // ---------- write topology
  {
    chunk_header.name_     = false;
    chunk_header.entity_   = OMFormat::Chunk::Entity_Face;
    chunk_header.type_     = OMFormat::Chunk::Type_Topology;
    chunk_header.signed_   = 0;
    chunk_header.float_    = 0;
    chunk_header.dim_      = OMFormat::Chunk::Dim_1D; // ignored
    chunk_header.bits_     = OMFormat::needed_bits(_be.n_vertices());    

    bytes += store( _os, chunk_header, swap );

    for (i=0, nF=_be.n_faces(); i<nF; ++i)
    {
      nV = _be.get_vhandles(FaceHandle(i), vhandles);
      if ( header.mesh_ == 'P' )
	bytes += store( _os, vhandles.size(), 
			OMFormat::Chunk::Integer_16, swap );
      
      for (size_t j=0; j < vhandles.size(); ++j)
      {
	using namespace OMFormat;
	using namespace GenProg;
	
	bytes += store( _os, vhandles[j].idx(),
			Chunk::Integer_Size(chunk_header.bits_), swap );
      }
    }
  }

  // ---------- write face normals

  if ( _be.has_face_normals() && _opt.check(Options::FaceNormal) )
  {
#define NEW_STYLE 0
#if NEW_STYLE
    const BaseProperty *bp = _be.kernel()._get_fprop("f:normals");

    if (bp)
    {
#endif
      Vec3f n = _be.normal(FaceHandle(0));

      chunk_header.name_     = false;
      chunk_header.entity_   = OMFormat::Chunk::Entity_Face;
      chunk_header.type_     = OMFormat::Chunk::Type_Normal;
      chunk_header.signed_   = OMFormat::is_signed(n[0]);
      chunk_header.float_    = OMFormat::is_float(n[0]);
      chunk_header.dim_      = OMFormat::dim(n);
      chunk_header.bits_     = OMFormat::bits(n[0]);
  
      bytes += store( _os, chunk_header, swap );
#if !NEW_STYLE
      for (i=0, nF=_be.n_faces(); i<nF; ++i)
        bytes += vector_store( _os, _be.normal(FaceHandle(i)), swap );
#else
      bytes += bp->store(_os, swap );
    }
    else
      return false;
#endif
#undef NEW_STYLE
  }


  // ---------- write face color

  if (_be.has_face_colors() && _opt.check( Options::FaceColor ))
  {  
#define NEW_STYLE 0
#if NEW_STYLE
    const BaseProperty *bp = _be.kernel()._get_fprop("f:colors");
    
    if (bp)
    {
#endif
      Vec3uc c;
      
      chunk_header.name_     = false;
      chunk_header.entity_   = OMFormat::Chunk::Entity_Face;
      chunk_header.type_     = OMFormat::Chunk::Type_Color;
      chunk_header.signed_   = OMFormat::is_signed( c[0] );
      chunk_header.float_    = OMFormat::is_float( c[0] );
      chunk_header.dim_      = OMFormat::dim( c );
      chunk_header.bits_     = OMFormat::bits( c[0] );
      
      bytes += store( _os, chunk_header, swap );
#if !NEW_STYLE
      for (i=0, nF=_be.n_faces(); i<nF; ++i)
        bytes += vector_store( _os, _be.color(FaceHandle(i)), swap );      
#else
      bytes += bp->store(_os, swap);
    }
    else
      return false;
#endif
  }

  // -------------------- write custom properties

  
  BaseKernel::const_prop_iterator prop;
  
  for (prop  = _be.kernel().vprops_begin();
       prop != _be.kernel().vprops_end(); ++prop)
  {
    if ( !*prop ) continue;
    if ( (*prop)->name()[1]==':') continue;
    bytes += store_binary_custom_chunk(_os, **prop, 
				       OMFormat::Chunk::Entity_Vertex, swap );
  }
  for (prop  = _be.kernel().fprops_begin();
       prop != _be.kernel().fprops_end(); ++prop)
  {
    if ( !*prop ) continue;
    if ( (*prop)->name()[1]==':') continue;
    bytes += store_binary_custom_chunk(_os, **prop, 
				       OMFormat::Chunk::Entity_Face, swap );
  }
  for (prop  = _be.kernel().eprops_begin();
       prop != _be.kernel().eprops_end(); ++prop)
  {
    if ( !*prop ) continue;
    if ( (*prop)->name()[1]==':') continue;
    bytes += store_binary_custom_chunk(_os, **prop, 
				       OMFormat::Chunk::Entity_Edge, swap );
  }
  for (prop  = _be.kernel().hprops_begin();
       prop != _be.kernel().hprops_end(); ++prop)
  {
    if ( !*prop ) continue;
    if ( (*prop)->name()[1]==':') continue;
    bytes += store_binary_custom_chunk(_os, **prop, 
				       OMFormat::Chunk::Entity_Halfedge, swap );
  }
  for (prop  = _be.kernel().mprops_begin();
       prop != _be.kernel().mprops_end(); ++prop)
  {
    if ( !*prop ) continue;
    if ( (*prop)->name()[1]==':') continue;
    bytes += store_binary_custom_chunk(_os, **prop, 
				       OMFormat::Chunk::Entity_Mesh, swap );
  }

// std::clog << "#bytes written: " << bytes << std::endl;

  return true;
}

// ----------------------------------------------------------------------------

size_t _OMWriter_::store_binary_custom_chunk(std::ostream& _os, 
					     const BaseProperty& _bp,
					     OMFormat::Chunk::Entity _entity,
					     bool _swap) const
{
  omlog << "Custom Property " << OMFormat::as_string(_entity) << " property ["
	<< _bp.name() << "]" << std::endl;

  // Don't store if 
  // 1. it is not persistent
  // 2. it's name is empty
  if ( !_bp.persistent() || _bp.name().empty() )
  {
    omlog << "  skipped\n";
    return 0;
  }

  size_t bytes = 0;

  OMFormat::Chunk::esize_t element_size   = _bp.element_size();
  OMFormat::Chunk::Header  chdr;

  // set header
  chdr.name_     = true;
  chdr.entity_   = _entity;
  chdr.type_     = OMFormat::Chunk::Type_Custom;
  chdr.signed_   = 0;
  chdr.float_    = 0;
  chdr.dim_      = OMFormat::Chunk::Dim_1D; // ignored
  chdr.bits_     = element_size;


  // write custom chunk

  // 1. chunk header       
  bytes += store( _os, chdr, _swap );

  // 2. property name
  bytes += store( _os, OMFormat::Chunk::PropertyName(_bp.name()), _swap );

  // 3. block size
  bytes += store( _os, _bp.size_of(), _swap );
  omlog << "  n_bytes = " << _bp.size_of() << std::endl;

  // 4. data
  {
    size_t b;
    bytes += ( b=_bp.store( _os, _swap ) );
    omlog << "  b       = " << b << std::endl;
    assert( b == _bp.size_of() );
  }
  return bytes;
}

// ----------------------------------------------------------------------------

size_t _OMWriter_::binary_size(BaseExporter& _be, Options _opt) const
{
  // std::clog << "[OMWriter]: binary_size()" << std::endl;
  size_t bytes  = sizeof( OMFormat::Header );

  // !!!TODO!!!

  return bytes;
}

//=============================================================================
} // namespace IO
} // namespace OpenMesh
//=============================================================================
