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

//=============================================================================
//
//  OpenMesh streams: omlog, omout, omerr
//
//=============================================================================

#ifndef OPENMESH_OMSTREAMS_HH
#define OPENMESH_OMSTREAMS_HH


//== INCLUDES =================================================================

#include <OpenMesh/Core/System/mostream.hh>


//== CLASS DEFINITION =========================================================


// do not put these into namespace OpenMesh (for convenience)
extern OpenMesh::mostream  omlog, omout, omerr;

#ifndef DOXY_IGNORE_THIS

namespace OpenMesh 
{
  struct __InitMultiplexStreams
  {
    struct Initializer
    {
      Initializer()
      {
	omlog.connect(std::clog);
	omout.connect(std::cout);
	omerr.connect(std::cerr);
#ifdef NDEBUG
	omlog.disable();
#endif
      }
    };
    static Initializer init_multiplex_streams;
  };
}

#endif

//=============================================================================
#endif // OPENMESH_OMSTREAMS_HH defined
//=============================================================================

