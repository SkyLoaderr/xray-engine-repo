#include "stdafx.h"
#include "build.h"
#include "cl_collector.h"

#define	_USE_MATH_DEFINES
#define FLT_MIN	flt_min
#define FLT_MAX	flt_max

// OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>
//#include <OpenMesh/Core/Utils/vector_cast.hh>
//#include <OpenMesh/Tools/Utils/getopt.h>
//#include <OpenMesh/Tools/Utils/Timer.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModProgMeshT.hh>
#include <OpenMesh/Tools/Decimater/ModIndependentSetsT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>

using namespace		CDB;
using namespace		OpenMesh;

//t-defs
typedef		DefaultTraits			MyTraits;
typedef		TriMesh_ArrayKernelT	< MyTraits >			_mesh;			// Mesh type
typedef		Decimater::DecimaterT	< _mesh >				_decimater;		// Decimater type
typedef		Decimater::ModQuadricT	< _decimater >::Handle	_HModQuadric;	// Decimation Module Handle type

//main
void SimplifyCFORM		(CDB::CollectorPacked& CL)
{
	Phase		("CFORM: simplification...");
	Status		("Building base mesh : vertices...");

	_mesh        mesh;				// a mesh object
	_decimater   decimater(mesh);	// a decimater object, connected to a mesh
	_HModQuadric hModQuadric;		// use a quadric module
	decimater.add( hModQuadric );	// register module at the decimater

	// Initializing mesh
	Status		("Building base mesh : vertices...");
	xr_vector<_mesh::VertexHandle>	vhandles;
	vhandles.resize	(CL.getVS());
	for (u32 v_it=0; v_it<CL.getVS(); v_it++)		{
		Fvector3&	v	= CL.getV()[v_it];
		vhandles[v_it]	= mesh.add_vertex	(_mesh::Point(v.x,v.y,v.z));
	}
	Status		("Building base mesh : faces...");
	std::vector <_mesh::VertexHandle>	fhandles;
	for (u32 f_it=0; f_it<CL.getTS(); f_it++)		{
		CDB::TRI&	f		= CL.getT()[f_it];
		fhandles.clear		();
		fhandles.push_back	(vhandles[f.IDvert(0)]);
		fhandles.push_back	(vhandles[f.IDvert(1)]);
		fhandles.push_back	(vhandles[f.IDvert(2)]);
		mesh.add_face		(fhandles);
	}
	vhandles.clear_and_free	();
	Status		("Building base mesh : normals...");
	mesh.request_vertex_normals	();
	mesh.update_vertex_normals	();

	// Decimate
	Status		("Reconstructing mesh-topology...");
	decimater.initialize();       // let the decimater initialize the mesh and the modules

	float	nv_before		= float	(mesh.n_vertices());
	int		nc				= decimater.decimate	(nv_before);	// do decimation, as large, as possible
	float	nv_after		= float	(mesh.n_vertices());
	clMsg					("was[%d], now[%d]",nv_before,nv_after);

	// ---- 6 - throw away all tagged edges
	mesh.garbage_collection();
}
