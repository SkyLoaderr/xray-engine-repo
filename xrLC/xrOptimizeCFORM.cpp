#include "stdafx.h"
#include "build.h"
#include "cl_collector.h"

#pragma warning(disable:4267)
#pragma warning(disable:4995)
#pragma warning(disable:4244)

#define FLT_MIN	flt_min
#define FLT_MAX	flt_max

// OpenMesh
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/Types/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModProgMeshT.hh>
#include <OpenMesh/Tools/Decimater/ModIndependentSetsT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>

using namespace		CDB;
using namespace		OpenMesh;

//t-defs
struct		MyTraits : public OpenMesh::DefaultTraits	
{
	FaceTraits	{
	private:
		u32		props_;
	public:
		FaceT() : props_(0)					{ }

		const	u32 props() const			{ return props_;	}
		void	set_props(const u32 _p)		{ props_ = _p;		}
	};

	HalfedgeAttributes( OpenMesh::Attributes::None );
};
typedef		TriMesh_ArrayKernelT	< MyTraits >			_mesh;			// Mesh type
typedef		Decimater::DecimaterT	< _mesh >				_decimater;		// Decimater type
typedef		Decimater::ModQuadricT	< _decimater >::Handle	_HModQuadric;	// Decimation Module Handle type

//main
struct	FailFace
{
	Fvector	P[3];
	u32		props;
};

void SimplifyCFORM		(CDB::CollectorPacked& CL)
{
	Phase		("CFORM: simplification...");
	//Status		("Building base mesh : vertices...");

	_mesh        mesh;						// a mesh object
	_decimater   decimater(mesh);			// a decimater object, connected to a mesh
	_HModQuadric hModQuadric;				// use a quadric module
	decimater.add( hModQuadric );			// register module at the decimater
	decimater.module(hModQuadric).set_max_err(0.00001,false);	// error-limit

	// Initializing mesh
	Status		("Building base mesh : vertices[%d]...",CL.getVS());
	xr_vector<_mesh::VertexHandle>		vhandles;
	vhandles.resize	(CL.getVS());
	for (u32 v_it=0; v_it<CL.getVS(); v_it++)		{
		Fvector3&	v	= CL.getV()[v_it];
		vhandles[v_it]	= mesh.add_vertex	(_mesh::Point(v.x,v.y,v.z));
	}
	Status		("Building base mesh : faces[%d]...",CL.getTS());
	std::vector <_mesh::VertexHandle>	fhandles;
	xr_vector	<FailFace>				failedfaces;
	for (u32 f_it=0; f_it<CL.getTS(); f_it++)		{
		CDB::TRI&	f		= CL.getT()[f_it];
		fhandles.clear		();
		fhandles.push_back	(vhandles[f.IDvert(0)]);
		fhandles.push_back	(vhandles[f.IDvert(1)]);
		fhandles.push_back	(vhandles[f.IDvert(2)]);
		_mesh::FaceHandle	hface	= mesh.add_face		(fhandles);
		if (hface == _mesh::InvalidFaceHandle)	{
			failedfaces.push_back(FailFace());
			failedfaces.back().P[0]		= CL.getV()[f.IDvert(0)];
			failedfaces.back().P[1]		= CL.getV()[f.IDvert(1)];
			failedfaces.back().P[2]		= CL.getV()[f.IDvert(2)];
			failedfaces.back().props	= f.dummy;
		}
		else				mesh.face(hface).set_props	(f.dummy);
	}
	clMsg		("%d faces failed topology check",failedfaces.size());
	clMsg		("%f%% geometry/artist quality",100.f * (1-float(failedfaces.size())/float(CL.getTS())));
	Status		("Building base mesh : normals...");
	mesh.garbage_collection		();
	mesh.request_vertex_normals	();
	mesh.update_vertex_normals	();
	vhandles.clear_and_free		();
	fhandles.clear				();
	CL.clear					();

	// Decimate
	Status		("Reconstructing mesh-topology...");
	decimater.initialize	();      // let the decimater initialize the mesh and the modules

	int		nf_before		= int	(mesh.n_faces());
	int		nv_before		= int	(mesh.n_vertices());
	int		nc				= decimater.decimate	(nv_before);	// do decimation, as large, as possible
							mesh.garbage_collection	();
	int		nf_after		= int	(mesh.n_faces());
	int		nv_after		= int	(mesh.n_vertices());
	clMsg					("vertices: was[%d], now[%d] => %f %% left",nv_before,nv_after, 100.f*float(nv_after)/float(nv_before) );
	clMsg					("   faces: was[%d], now[%d] => %f %% left",nf_before,nf_after, 100.f*float(nf_after)/float(nf_before) );

	// Decimate
	Status		("Refactoring CFORM...");
	_mesh::FaceIter		fit=mesh.faces_begin(),fend=mesh.faces_end();
	for (; fit!=fend; fit++)
	{
		// get vertex-handles
		fhandles.clear	();
		for (_mesh::CFVIter fv_it=mesh.cfv_iter(fit); fv_it; ++fv_it)
			fhandles.push_back	(fv_it.handle());

		CL.add_face_D	(
			reinterpret_cast<Fvector&>(mesh.point	(fhandles[0])),
			reinterpret_cast<Fvector&>(mesh.point	(fhandles[1])),
			reinterpret_cast<Fvector&>(mesh.point	(fhandles[2])),
			fit->props	()
			);
	}
	Status		("Restoring fail-faces...");
	for (u32 it=0; it<failedfaces.size(); it++)
	{
		FailFace&	F	= failedfaces[it];
		CL.add_face_D	( F.P[0], F.P[1], F.P[2], F.props );
	}
}
