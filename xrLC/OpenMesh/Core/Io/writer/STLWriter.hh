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
//  Implements a writer module for STL ascii files
//
//=============================================================================
// $Id: STLWriter.hh,v 1.8 2004/01/08 15:48:35 openmeshadm Exp $

#ifndef __STLWRITER_HH__
#define __STLWRITER_HH__


//=== INCLUDES ================================================================

// -------------------- STL
#if defined( OM_CC_MIPS )
#  include <stdio.h>
#else
#  include <cstdio>
#endif
#include <string>
// -------------------- OpenMesh
#include <OpenMesh/Core/System/config.h>
#include <OpenMesh/Core/Utils/SingletonT.hh>
#include <OpenMesh/Core/IO/exporter/BaseExporter.hh>
#include <OpenMesh/Core/IO/writer/BaseWriter.hh>


//== NAMESPACES ===============================================================


namespace OpenMesh {
namespace IO {


//=== IMPLEMENTATION ==========================================================


/** 
    Implementation of the STL format writer. This class is singleton'ed by 
    SingletonT to STLWriter.
*/
class _STLWriter_ : public BaseWriter
{
public:
  
  _STLWriter_();
  
  std::string get_description() const { return "Stereolithography Format"; }
  std::string get_extensions()  const { return "stla stlb"; }
  
  bool write(const std::string&, BaseExporter&, Options) const;
  
  size_t binary_size(BaseExporter&, Options) const;

private:
  bool write_stla(FILE*, BaseExporter&, Options) const;
  bool write_stlb(FILE*, BaseExporter&, Options) const;
};


//== TYPE DEFINITION ==========================================================


// Declare the single entity of STL writer.
typedef SingletonT<_STLWriter_> STLWriter;


//=============================================================================
} // namespace IO
} // namespace OpenMesh
//=============================================================================
#endif
//=============================================================================
