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

// OpenMesh
#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/System/omstream.hh>
#include <OpenMesh/Core/IO/reader/OFFReader.hh>
#include <OpenMesh/Core/IO/importer/BaseImporter.hh>
#include <OpenMesh/Core/IO/IOManager.hh>
#include <OpenMesh/Core/IO/BinaryHelper.hh>

//STL
#include <fstream>
#include <memory>

//=== NAMESPACES ==============================================================


namespace OpenMesh {
namespace IO {


//=============================================================================

#if !defined(OM_CC_MIPS)
using std::memset;
#endif

//=== INSTANCIATE =============================================================


// register the OFFReader singleton with MeshReader
static BaseReader* OFFReaderInstance = &OFFReader::Instance();


//=== IMPLEMENTATION ==========================================================



_OFFReader_::_OFFReader_() { IOManager::Instance().register_module(this); }


//-----------------------------------------------------------------------------


bool 
_OFFReader_::read(const std::string& _filename, BaseImporter& _bi, 
                  Options& _opt)
{
  FILE* in = fopen(_filename.c_str(), (options_.is_binary ? "rb" : "r"));
  if (!in)
  {
    omerr << "[OFFReader] : cannot not open file " 
	  << _filename
	  << std::endl;
    return false;
  }


  bool result = read(in, _bi, _opt);


  fclose(in);
  return result;
}


//-----------------------------------------------------------------------------


bool 
_OFFReader_::read(FILE* _in, BaseImporter& _bi, Options& _opt ) const
{
   // filter relevant options for reading
   bool swap = _opt.check( Options::Swap );
  

   // build options to be returned
   _opt.clear();
   if (options_.vertex_has_normal)   _opt += Options::VertexNormal;
   if (options_.vertex_has_texcoord) _opt += Options::VertexTexCoord;
   if (options_.vertex_has_color)    _opt += Options::VertexColor;
   if (options_.is_binary)           _opt += Options::Binary;     


   return (options_.is_binary ?
	   read_binary(_in, _bi, swap) :
	   read_ascii(_in, _bi));
}



//-----------------------------------------------------------------------------

bool 
_OFFReader_::read_ascii(FILE* _in, BaseImporter& _bi) const
{
  omlog << "[OFFReader] : read ascii file\n";
   
  unsigned int            i, j, k, l, idx;
  unsigned int            nV, nF, dummy;
  OpenMesh::Vec3f         v, n;
  OpenMesh::Vec2f         t;
  BaseImporter::VHandles  vhandles;
  VertexHandle            vh;


  // read header line
  char line[100];
  fgets(line, 100, _in);


  // + optional space dimension
  if (options_.vertex_dim)
  {
    fscanf(_in, "%d", &i);
    if (i != 3) // cannot process more 3 dimensions!
      return false;
  }


  // + #Vertice, #Faces, #Edges
  fscanf(_in, "%d %d %d", &nV, &nF, &dummy);
  _bi.reserve(nV, 3*nV, nF);


  // read vertices: coord [hcoord] [normal] [color] [texcoord] 
  if (options_.vertex_has_normal && options_.vertex_has_texcoord)
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      fscanf(_in, "%f %f %f %f %f %f %f %f", 
	     &v[0], &v[1], &v[2], &n[0], &n[1], &n[2], &t[0], &t[1]);
      vh = _bi.add_vertex(v);
      _bi.set_normal(vh, n);
      _bi.set_texcoord(vh, t);
    }
  }

  else if (options_.vertex_has_normal)
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      fscanf(_in, "%f %f %f %f %f %f", 
	     &v[0], &v[1], &v[2], &n[0], &n[1], &n[2]);
      vh = _bi.add_vertex(v);
      _bi.set_normal(vh, n);
    }
  }

  else if (options_.vertex_has_texcoord)
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      fscanf(_in, "%f %f %f %f %f", 
	     &v[0], &v[1], &v[2], &t[0], &t[1]);
      vh = _bi.add_vertex(v);
      _bi.set_texcoord(vh, t);
    }
  }

  else
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      fscanf(_in, "%f %f %f", &v[0], &v[1], &v[2]);
      _bi.add_vertex(v);
    }
  }


  
  // faces
  // #N <v1> <v2> .. <v(n-1)> [color spec]
  // So far color spec is unsupported!
  for (i=0; i<nF; ++i)
  {
    fscanf(_in, "%d", &nV);


    if (nV == 3)
    {
      vhandles.resize(3);
      fscanf(_in, "%d %d %d", &j, &k, &l);
      vhandles[0] = VertexHandle(j);
      vhandles[1] = VertexHandle(k);
      vhandles[2] = VertexHandle(l);
    }
    else 
    {
      vhandles.clear();
      for (j=0; j<nV; ++j)
      {
	fscanf(_in, "%d", &idx);
	vhandles.push_back(VertexHandle(idx));
      }
    }
    
    _bi.add_face(vhandles);
  }


  // File was successfully parsed.
  return true;
}


//-----------------------------------------------------------------------------


bool 
_OFFReader_::read_binary(FILE* _in, BaseImporter& _bi, bool _swap) const
{
  omlog << "[OFFReader] : read binary file\n";

  unsigned long           i, j;
  unsigned long           nV, nF;
  unsigned long           space_dim;
  OpenMesh::Vec3f         v, n;
  OpenMesh::Vec2f         t;
  BaseImporter::VHandles  vhandles;
  VertexHandle            vh;


  // header line
  char line[100]; 
  fgets(line, 100, _in);


  // vertex dimension
  if (options_.vertex_dim)
  {
    space_dim = read_int(_in, _swap);
    if (space_dim != 3) // cannot process more 3 dimensions!
      return false;
  }

  
  // #Vertice, #Faces, #Edges
  nV = read_int(_in, _swap);
  nF = read_int(_in, _swap);
       read_int(_in, _swap);
  _bi.reserve(nV, 3*nV, nF);


  // read vertices [hcoord] [normal] [color] [texcoord] 
  if (options_.vertex_has_normal && options_.vertex_has_texcoord)
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      v[0] = read_float(_in, _swap);
      v[1] = read_float(_in, _swap);
      v[2] = read_float(_in, _swap);
      n[0] = read_float(_in, _swap);
      n[1] = read_float(_in, _swap);
      n[2] = read_float(_in, _swap);
      t[0] = read_float(_in, _swap);
      t[1] = read_float(_in, _swap);

      vh = _bi.add_vertex(v);
      _bi.set_normal(vh, n);
      _bi.set_texcoord(vh, t);
    }
  }

  else if (options_.vertex_has_normal)
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      v[0] = read_float(_in, _swap);
      v[1] = read_float(_in, _swap);
      v[2] = read_float(_in, _swap);
      n[0] = read_float(_in, _swap);
      n[1] = read_float(_in, _swap);
      n[2] = read_float(_in, _swap);

      vh = _bi.add_vertex(v);
      _bi.set_normal(vh, n);
    }
  }

  else if (options_.vertex_has_texcoord)
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      v[0] = read_float(_in, _swap);
      v[1] = read_float(_in, _swap);
      v[2] = read_float(_in, _swap);
      t[0] = read_float(_in, _swap);
      t[1] = read_float(_in, _swap);

      vh = _bi.add_vertex(v);
      _bi.set_texcoord(vh, t);
    }
  }

  else
  {
    for (i=0; i<nV && !feof(_in); ++i)
    {
      v[0] = read_float(_in, _swap);
      v[1] = read_float(_in, _swap);
      v[2] = read_float(_in, _swap);

      _bi.add_vertex(v);
    }
  }


  
  // faces
  // #N <v1> <v2> .. <v(n-1)> [color spec]
  // So far color spec is unsupported!
  for (i=0; i<nF; ++i)
  {
    nV = read_int(_in, _swap);

    if (nV == 3)
    {
      vhandles.resize(3);
      vhandles[0] = VertexHandle(read_int(_in, _swap));
      vhandles[1] = VertexHandle(read_int(_in, _swap));
      vhandles[2] = VertexHandle(read_int(_in, _swap));
    }
    else 
    {
      vhandles.clear();
      for (j=0; j<nV; ++j)
	vhandles.push_back(VertexHandle(read_int(_in, _swap)));
    }
    
    _bi.add_face(vhandles);
  }


  // File was successfully parsed.
  return true;
}


//-----------------------------------------------------------------------------


bool _OFFReader_::can_u_read(const std::string& _filename) const
{
  // !!! Assuming BaseReader::can_u_parse( std::string& )
  // does not call BaseReader::read_magic()!!!
  if (BaseReader::can_u_read(_filename))
  {
    std::ifstream ifs(_filename.c_str());
    if (ifs.is_open() && can_u_read(ifs))
    {
      ifs.close();
      return true;
    }      
  }
  return false;
}


//-----------------------------------------------------------------------------

  
bool _OFFReader_::can_u_read(std::istream& _is) const
{
  // read 1st line
  char line[100], *p;
  _is.getline(line, 100);
  p = line;

  

  // check header: [ST][C][N][4][n]OFF BINARY
  memset( &options_, 0, sizeof(options_) );

  if (p[0] == 'S' && p[1] == 'T')
  { options_.vertex_has_texcoord = true; p += 2; }

  if (p[0] == 'C')
  { options_.vertex_has_color = true; ++p; }

  if (p[0] == 'N')
  { options_.vertex_has_normal = true; ++p; }

  if (p[0] == '4')
  { options_.vertex_has_hcoord = true; ++p; }

  if (p[0] == 'n')
  { options_.vertex_dim = true; ++p; }
  
  if (!(p[0] == 'O' && p[1] == 'F' && p[2] == 'F'))
    return false;

  if (strncmp(p+4, "BINARY", 6) == 0)
    options_.is_binary = true;



  // currently these options are not supported!!!
  if (options_.vertex_dim  || options_.vertex_has_hcoord)
    return false;
   

  return true;
}


//=============================================================================
} // namespace IO
} // namespace OpenMesh
//=============================================================================
