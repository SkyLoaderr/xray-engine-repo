#include "stdafx.h"
#include "nvMeshMender.h"

void CBuild::xrPhase_TangentBasis()
{
	// ************************************* Declare inputs
	Status						("Declarator...");
	u32 v_count_reserve			= iFloor(float(g_vertices.size())*1.33f);
	u32 i_count_reserve			= 3*g_faces.size();

	std::vector<NVMeshMender::VertexAttribute> input,output;
	input.push_back	(NVMeshMender::VertexAttribute());	// pos
	input.push_back	(NVMeshMender::VertexAttribute());	// norm
	input.push_back	(NVMeshMender::VertexAttribute());	// tex0
	input.push_back	(NVMeshMender::VertexAttribute());	// *** faces

	input[0].Name_= "position";	vector<float>&	v_position	= input[0].floatVector_;	v_position.reserve	(v_count_reserve);
	input[1].Name_= "normal";	vector<float>&	v_normal	= input[1].floatVector_;	v_normal.reserve	(v_count_reserve);
	input[2].Name_= "tex0";		vector<float>&	v_tc		= input[2].floatVector_;	v_tc.reserve		(v_count_reserve);
	input[3].Name_= "indices";	vector<int>&	v_indices	= input[3].intVector_;		v_indices.reserve	(i_count_reserve);

	output.push_back(NVMeshMender::VertexAttribute());	// tex0
	output.push_back(NVMeshMender::VertexAttribute());	// tangent
	output.push_back(NVMeshMender::VertexAttribute());	// binormal
	output.push_back(NVMeshMender::VertexAttribute());	// *** faces
	output.push_back(NVMeshMender::VertexAttribute());	// position, needed for mender

	output[0].Name_= "tex0";	
	output[1].Name_= "tangent";	
	output[2].Name_= "binormal";
	output[3].Name_= "indices";	
	output[4].Name_= "position";

	// ************************************* Build vectors + expand TC if nessesary
	Status						("Building inputs...");
	std::sort					(g_vertices.begin(),g_vertices.end());
	vector<vector<u32> >		remap;
	remap.resize				(g_vertices.size());
	for (u32 f=0; f<g_faces.size(); f++)
	{
		Progress	(float(f)/float(g_faces.size()));
		Face*		F	= g_faces[f];

		for (u32 v=0; v<3; v++)
		{
			Vertex*		V	= F->v[v];	
			Fvector2	Ftc = F->tc.front().uv[v];
			u32 ID			= lower_bound(g_vertices.begin(),g_vertices.end(),V)-g_vertices.begin();
			vector<u32>& m	= remap[ID];

			// Search
			BOOL bFound		= FALSE;
			for (u32 it=0; it<m.size(); it++)
			{
				u32		m_id		= m[it];
				float*	tc			= v_tc.begin()+m_id*3;
				if (!fsimilar(tc[0],Ftc.x))	continue;
				if (!fsimilar(tc[1],Ftc.y))	continue;

				bFound				= TRUE;
				v_indices.push_back	(m_id);
				break;
			}

			// Register new if not found
			if (!bFound)
			{
				u32		m_id			= v_position.size()/3;
				v_position.push_back(V->P.x);	v_position.push_back(V->P.y);	v_position.push_back(V->P.z);
				v_normal.push_back(V->N.x);		v_normal.push_back(V->N.y);		v_normal.push_back(V->N.z);
				v_tc.push_back(Ftc.x);			v_tc.push_back(Ftc.y);			v_tc.push_back(0);
				v_indices.push_back(m_id);
				remap[ID].push_back(m_id);
			}
		}
	}
	remap.clear	();
	u32			v_was	= g_vertices.size();
	u32			v_become= v_position.size()/3;
	clMsg		("duplication: was[%d] / become[%d] - %2.1f%%",v_was,v_become,100.f*float(v_become-v_was)/float(v_was));

	// ************************************* Perform mungle
	Status						("Calculating basis...");
	NVMeshMender mender;
	if (!mender.Munge(
		input,										// input attributes
		output,										// outputs attributes
		deg2rad(61.f),								// tangent space smooth angle
		0,											// no texture matrix applied to my texture coordinates
		NVMeshMender::FixTangents,					// fix degenerate bases & texture mirroring
		NVMeshMender::DontFixCylindricalTexGen,		// handle cylindrically mapped textures via vertex duplication
		NVMeshMender::DontWeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
		))
	{
		Debug.fatal	("NVMeshMender failed (%s)",mender.GetLastError().c_str());
	}

	// ************************************* Bind declarators
	// bind
	vector<float>&	o_tc		= output[0].floatVector_;	R_ASSERT(output[0].Name_=="tex0");
	vector<float>&	o_tangent	= output[1].floatVector_;	R_ASSERT(output[1].Name_=="tangent");
	vector<float>&	o_binormal	= output[2].floatVector_;	R_ASSERT(output[2].Name_=="binormal");
	vector<int>&	o_indices	= output[3].intVector_;		R_ASSERT(output[3].Name_=="indices");

	// verify
	R_ASSERT		(3*g_faces.size()	== o_indices.size());
	R_ASSERT		(o_tc.size()		== o_tangent.size());
	R_ASSERT		(o_tangent.size()	== o_binormal.size());
	R_ASSERT		(o_tc.size()		>= v_tc.size());

	// ************************************* Retreive data
	Status						("Retreiving basis...");
	for (f=0; f<g_faces.size(); f++)
	{
		Face*	F				= g_faces[f];
		for (u32 v=0; v<3; v++)
		{
			u32	id							=	o_indices	[f*3+v];	// vertex index
			R_ASSERT						(id*3 < o_tc.size());
			F->tc.front().uv[v].set			(o_tc		[id*3+0],	o_tc		[id*3+1]);
			F->basis_tangent[v].set			(o_tangent	[id*3+0],	o_tangent	[id*3+1],	o_tangent	[id*3+2]);
			F->basis_binormal[v].set		(o_binormal	[id*3+0],	o_binormal	[id*3+1],	o_binormal	[id*3+2]);
			F->basis_tangent[v].normalize	();
			F->basis_binormal[v].normalize	();
		}
	}
}
