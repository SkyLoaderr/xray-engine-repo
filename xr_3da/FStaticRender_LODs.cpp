#include "stdafx.h"
#include "fstaticrender.h"
#include "flod.h"

void CRender::flush_LODs()
{
	mapLOD.getLR				(lstLODs);
	if (lstLODs.empty())		return;

	// *** Fill VB and generate groups
	CVisual*					firstV		= lstLODs[0].pVisual;
	Shader*						cur_S		= firstV->hShader;
	int							cur_count	= 0;
	DWORD						vOffset;
	FVF::LIT*					V = (FVF::LIT*)Device.Streams.Vertex.Lock	(lstLODs.size()*4,firstV->hVS->dwStride, vOffset);
	for (DWORD i=0; i<lstLODs.size(); i++)
	{
		// sort out redundancy
		SceneGraph::_LodItem		&P = lstLODs[i];
		if (P.pVisual->hShader==cur_S)	cur_count++;
		else {
			vecGroups.push_back	(cur_count);
			cur_S				= P.pVisual->hShader;
			cur_count			= 1;
		}
        
		// gen geometry
		FLOD*							lodV		= (FLOD*)P.pVisual;
		Fvector							Ldir;
		Ldir.sub						(lodV->bv_Position,Device.vCameraPosition);
		Ldir.normalize					();

		FLOD::_face*					facets		= lodV->facets;
		int								best_id		= 0;
		float							best_dot	= Ldir.dotproduct(facets[0].N);
		float							dot;

		dot	= Ldir.dotproduct			(facets[1].N); if (dot>best_dot) { best_id=1; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[2].N); if (dot>best_dot) { best_id=2; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[3].N); if (dot>best_dot) { best_id=3; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[4].N); if (dot>best_dot) { best_id=4; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[5].N); if (dot>best_dot) { best_id=5; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[6].N); if (dot>best_dot) { best_id=6; best_dot=dot; }
		dot	= Ldir.dotproduct			(facets[7].N); if (dot>best_dot) { best_id=7; best_dot=dot; }

		// Fill VB
		FLOD::_face&	F				= facets[best_id];
		V->set							(F.v[0].v,F.v[0].c,F.v[0].t.x,F.v[0].t.y); V++;
		V->set							(F.v[1].v,F.v[1].c,F.v[1].t.x,F.v[1].t.y); V++;
		V->set							(F.v[2].v,F.v[2].c,F.v[2].t.x,F.v[2].t.y); V++;
		V->set							(F.v[3].v,F.v[3].c,F.v[3].t.x,F.v[3].t.y); V++;
	}
	vecGroups.push_back				(cur_count);
	Device.Streams.Vertex.Unlock	(lstLODs.size()*4,firstV->hVS->dwStride);

	// *** Render
	int current=0;
	for (DWORD g=0; g<vecGroups.size(); g++)
	{
		int p_count						= vecGroups[g];
		Device.Shader.set_Shader		(lstLODs[current].pVisual->hShader);

		Device.Primitive.setVertices	(firstV->hVS->dwHandle,firstV->hVS->dwStride,Device.Streams.Vertex.Buffer());
		Device.Primitive.setIndices		(vOffset,Device.Streams.QuadIB);;
		Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,4*p_count,0,2*p_count);
		current	+=	p_count;
		vOffset	+=	4*p_count;
	}

	mapLOD.clear	();
	lstLODs.clear	();
	vecGroups.clear	();
}
