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
typedef		TriMesh_ArrayKernelT	< MyTraits >			_Mesh;			// Mesh type
typedef		Decimater::DecimaterT	< _Mesh >				_Decimater;		// Decimater type
typedef		Decimater::ModQuadricT	< _Decimater >::Handle	_HModQuadric;	// Decimation Module Handle type

//main
void SimplifyCFORM		(CDB::CollectorPacked& CL)
{
	Phase		("CFORM: simplification...");
	Status		("Building base mesh...");

	_Mesh        mesh;             // a mesh object
	_Decimater   decimater(mesh);  // a decimater object, connected to a mesh
	_HModQuadric hModQuadric;      // use a quadric module
	decimater.add( hModQuadric ); // register module at the decimater

	decimater.initialize();       // let the decimater initialize the mesh and the modules

	float	nv_before		= float	(mesh.n_vertices());
	int		nc				= decimater.decimate	(nv_before);	// do decimation, as large, as possible
	float	nv_after		= float	(mesh.n_vertices());

	// ---- 6 - throw away all tagged edges
	mesh.garbage_collection();
}
