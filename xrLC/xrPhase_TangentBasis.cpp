#include "stdafx.h"

void CBuild::xrPhase_TangentBasis()
{
	// ************************************* Declare inputs
	Status						("Declarator...");
	u32 v_count_reserve			= iFloor(float(g_vertices.size())*1.5f);
	u32 i_count_reserve			= 3*g_faces.size();

	std::vector<NVMeshMender::VertexAttribute> input;
	input.push_back(NVMeshMender::VertexAttribute());
	input.push_back(NVMeshMender::VertexAttribute());
	input.push_back(NVMeshMender::VertexAttribute());
	input.push_back(NVMeshMender::VertexAttribute());
	
	input[0].Name_= "position";	vector<float>&	v_position	= input[0].floatVector_;	v_position.reserve	(v_count_reserve);
	input[1].Name_= "normal";	vector<float>&	v_normal	= input[1].floatVector_;	v_normal.reserve	(v_count_reserve);
	input[2].Name_= "tex0";		vector<float>&	v_tc		= input[2].floatVector_;	v_tc.reserve		(v_count_reserve);
	input[3].Name_= "indices";	vector<int>&	v_indices	= input[3].intVector_;		v_indices.reserve	(i_count_reserve);

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
			Fvector2	Ftc = F->tc.front()
			u32 ID			= lower_bound(g_vertices.begin(),g_vertices.end(),V)-g_vertices.begin();
			vector<u32>& m	= remap[ID];

			BOOL bFound		= FALSE;
			for (u32 it=0; it<m.size(); it++)
			{
				u32		m_id	= m[it];
				float*	tc		= v_tc.begin()+m_id*3;
				if (!fsimilar(tc[0],F))
			}
		}
	}

	// ************************************* Perform mungle
	{
		unsigned int i;

		// Retrieve vertices and texture coordinates from the mesh vertex buffer
		LPD3DXMESH gMesh	= Mesh.GetSysMemMesh();
		u32 numVertices		= gMesh->GetNumVertices();
		meshVERTEX*			vertexBuffer;
		std::vector<float>	position;
		std::vector<float>	texCoord;
		std::vector<float>	normals;
		gMesh->LockVertexBuffer	(D3DLOCK_READONLY, (VOID**)&vertexBuffer);
		for (i = 0; i < numVertices; ++i) {
			position.push_back	(vertexBuffer[i].p.x);
			position.push_back	(vertexBuffer[i].p.y);
			position.push_back	(vertexBuffer[i].p.z);
			normals.push_back	(vertexBuffer[i].n.x);
			normals.push_back	(vertexBuffer[i].n.y);
			normals.push_back	(vertexBuffer[i].n.z);
			texCoord.push_back	(vertexBuffer[i].tu);
			texCoord.push_back	(vertexBuffer[i].tv);
			texCoord.push_back	(0);
		}
		gMesh->UnlockVertexBuffer();

		// Retrieve triangle indices from the index buffer
		u32 numTriangles	= gMesh->GetNumFaces();
		WORD (*indexBuffer)[3];
		std::vector<int> index;
		HRESULT hr = gMesh->LockIndexBuffer(D3DLOCK_READONLY, (VOID**)&indexBuffer);
		for (i = 0; i < numTriangles; ++i) {
			index.push_back(indexBuffer[i][0]);
			index.push_back(indexBuffer[i][1]);
			index.push_back(indexBuffer[i][2]);
		}
		gMesh->UnlockIndexBuffer();

		// gMesh->Release();

		// Prepare the parameters to the mesh mender

		// Fill in the input to the mesh mender

		// Specify the requested output
		// Tangents
		NVMeshMender::VertexAttribute tangentAtt;
		tangentAtt.Name_ = "tangent";
		// Binormals
		NVMeshMender::VertexAttribute binormalAtt;
		binormalAtt.Name_ = "binormal";
		// Fill in output list
		std::vector<NVMeshMender::VertexAttribute> outputAtts;
		unsigned int n = 0;
		outputAtts.push_back(positionAtt); ++n;
		outputAtts.push_back(indexAtt); ++n;
		outputAtts.push_back(texCoordAtt); ++n;
		outputAtts.push_back(tangentAtt); ++n;
		outputAtts.push_back(binormalAtt); ++n;
		outputAtts.push_back(normalAtt); ++n;

		// Mender!!!!
		NVMeshMender mender;
		if (!mender.Munge(
			inputAtts,									// input attributes
			outputAtts,									// outputs attributes
			3.141592654f / 3.0f,						// tangent space smooth angle
			0,											// no texture matrix applied to my texture coordinates
			NVMeshMender::FixTangents,					// fix degenerate bases & texture mirroring
			NVMeshMender::DontFixCylindricalTexGen,		// handle cylindrically mapped textures via vertex duplication
			NVMeshMender::DontWeightNormalsByFaceSize	// weigh vertex normals by the triangle's size
			))
		{
			fprintf(stderr, "NVMeshMender failed\n");
			exit(-1);
		}

		// Retrieve outputs
		--n; std::vector<float> normal = outputAtts[n].floatVector_;
		--n; std::vector<float> binormal = outputAtts[n].floatVector_;
		--n; std::vector<float> tangent = outputAtts[n].floatVector_;
		--n; texCoord = outputAtts[n].floatVector_;
		--n; index = outputAtts[n].intVector_;
		--n; position = outputAtts[n].floatVector_;

		// Create the new vertex buffer
		m_dwModelNumVerts = position.size() / 3;
		D3DXComputeBoundingSphere((D3DXVECTOR3*)&position.front(), m_dwModelNumVerts, 3*sizeof(float), &vecModelCenter, &fModelRad);
		m_fModelSize = fModelRad * 2.0f;

		u32 size = m_dwModelNumVerts * sizeof(VERTEX);
		m_pd3dDevice->CreateVertexBuffer(size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pModelVB, NULL);
		VERTEX* vertexBufferNew;
		m_pModelVB->Lock	(0, 0, (void**)&vertexBufferNew, 0);
		for (i = 0; i < m_dwModelNumVerts; ++i) {
			vertexBufferNew[i].p.x			= position[3 * i + 0];
			vertexBufferNew[i].p.y			= position[3 * i + 1];
			vertexBufferNew[i].p.z			= position[3 * i + 2];
			vertexBufferNew[i].tu			= texCoord[3 * i + 0];
			vertexBufferNew[i].tv			= texCoord[3 * i + 1];

			D3DXVECTOR3	N,T,B;
			N.x								= normal[3 * i + 0];
			N.y								= normal[3 * i + 1];
			N.z								= normal[3 * i + 2];
			D3DXVec3Normalize				(&N,&N);
			T.x								= tangent[3 * i + 0];
			T.y								= tangent[3 * i + 1];
			T.z								= tangent[3 * i + 2];
			D3DXVec3Normalize				(&T,&T);
			B.x								= binormal[3 * i + 0];
			B.y								= binormal[3 * i + 1];
			B.z								= binormal[3 * i + 2];
			D3DXVec3Normalize				(&B,&B);

			/*
			// ortho-normalize
			//			D3DXVec3Cross					(&B,&T,&N);
			//			D3DXVec3Normalize				(&B,&B);
			//			D3DXVec3Cross					(&T,&N,&B);
			//			D3DXVec3Normalize				(&T,&T);
			B = -B;
			D3DXVec3Cross					(&T,&B,&N);
			D3DXVec3Normalize				(&T,&T);
			D3DXVec3Cross					(&B,&N,&T);
			D3DXVec3Normalize				(&B,&B);
			*/

			vertexBufferNew[i].n.x			= N.x;
			vertexBufferNew[i].n.y			= N.y;
			vertexBufferNew[i].n.z			= N.z;
			vertexBufferNew[i].tangent.x	= T.x;
			vertexBufferNew[i].tangent.y	= T.y;
			vertexBufferNew[i].tangent.z	= T.z;
			vertexBufferNew[i].binormal.x	= B.x;
			vertexBufferNew[i].binormal.y	= B.y;
			vertexBufferNew[i].binormal.z	= B.z;
		}
		m_pModelVB->Unlock();

		// Create the new index buffer
		m_dwModelNumFaces	= index.size() / 3;
		size				= m_dwModelNumFaces * 3 * sizeof(WORD);
		m_pd3dDevice->CreateIndexBuffer(size, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pModelIB, NULL);
		m_pModelIB->Lock	(0, 0, (void**)&indexBuffer, 0);
		for (i = 0; i < m_dwModelNumFaces; ++i) {
			indexBuffer[i][0] = index[3 * i + 0];
			indexBuffer[i][1] = index[3 * i + 1];
			indexBuffer[i][2] = index[3 * i + 2];
		}
		m_pModelIB->Unlock	();
	}
}
