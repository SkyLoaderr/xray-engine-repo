#include "stdafx.h"
#include "cl_collector.h"
#include "build.h"
#include "communicate.h"

CDB::MODEL*	RCAST_Model	= 0;

IC bool				FaceEqual(Face& F1, Face& F2)
{
	// Test for 6 variations
	if ((F1.v[0]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[0]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[1]==F2.v[2])) return true;
	if ((F1.v[2]==F2.v[0]) && (F1.v[1]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[0]==F2.v[1]) && (F1.v[2]==F2.v[2])) return true;
	if ((F1.v[1]==F2.v[0]) && (F1.v[2]==F2.v[1]) && (F1.v[0]==F2.v[2])) return true;
	return false;
}

void CBuild::BuildRapid		(BOOL bSaveForOtherCompilers)
{
	float	p_total			= 0;
	float	p_cost			= 1.f/(g_faces.size());

	xr_delete		(RCAST_Model);

	Status			("Converting faces...");
	for				(u32 fit=0; fit<g_faces.size(); fit++)	g_faces[fit]->flags.bProcessed = false;

	xr_vector<Face*>	adjacent;	adjacent.reserve(6*2*3);
	CDB::CollectorPacked	CL	(scene_bb,g_vertices.size(),g_faces.size());
	for (vecFaceIt it=g_faces.begin(); it!=g_faces.end(); it++)
	{
		Face*	F	= (*it);
		Progress	(float(it-g_faces.begin())/float(g_faces.size()));

		// Collect
		adjacent.clear	();
		for (int vit=0; vit<3; vit++)
		{
			Vertex* V = F->v[vit];
			for (u32 adj=0; adj<V->adjacent.size(); adj++)
			{
				adjacent.push_back(V->adjacent[adj]);
			}
		}

		// Unique
		std::sort		(adjacent.begin(),adjacent.end());
		adjacent.erase	(std::unique(adjacent.begin(),adjacent.end()),adjacent.end());
		BOOL			bAlready	= FALSE;
		for (u32 ait=0; ait<adjacent.size(); ait++)
		{
			Face*	Test					= adjacent[ait];
			if (Test==F)					continue;
			if (!Test->flags.bProcessed)	continue;
			if (FaceEqual(*F,*Test)){
				bAlready					= TRUE;
				break;
			}
		}

		//
		if (!bAlready) 
		{
			F->flags.bProcessed	= true;
			CL.add_face_D		( F->v[0]->P,F->v[1]->P,F->v[2]->P, u32(F) );
		}
	}

	clMsg					("Faces: original(%d), model(%d), ratio(%f)",
		g_faces.size(),CL.getTS(),float(CL.getTS())/float(g_faces.size()));

	// Export references
	Status					("Models..");
	for (u32 ref=0; ref<mu_refs.size(); ref++)
		mu_refs[ref]->export_cform_rcast	(CL);

	Status					("Building OBB tree..");
	RCAST_Model				= xr_new<CDB::MODEL> ();
	RCAST_Model->build		(CL.getV(),(int)CL.getVS(),CL.getT(),(int)CL.getTS());

	// Saving for AI/DO usage
	if (bSaveForOtherCompilers)
	{
		Status					("Saving...");
		string256				fn;

		IWriter*		MFS		= FS.w_open	(strconcat(fn,pBuild->path,"build.cform"));
		xr_vector<b_rc_face>	rc_faces;
		rc_faces.resize			(CL.getTS());
		// Prepare faces
		for (u32 k=0; k<CL.getTS(); k++){
			CDB::TRI* T			= CL.getT()+k;
			base_Face* F		= (base_Face*)T->dummy;
			b_rc_face& cf		= rc_faces[k];
			cf.dwMaterial		= F->dwMaterial;
			cf.dwMaterialGame	= F->dwMaterialGame;
			Fvector2*	cuv		= F->getTC0	();
			cf.t[0].set			(cuv[0]);
			cf.t[1].set			(cuv[1]);
			cf.t[2].set			(cuv[2]);
		}

		MFS->open_chunk			(0);

		// Header
		hdrCFORM hdr;
		hdr.version				= CFORM_CURRENT_VERSION;
		hdr.vertcount			= (u32)CL.getVS();
		hdr.facecount			= (u32)CL.getTS();
		hdr.aabb				= scene_bb;
		MFS->w					(&hdr,sizeof(hdr));

		// Data
		MFS->w					(CL.getV(),(u32)CL.getVS()*sizeof(Fvector));
		MFS->w					(CL.getT(),(u32)CL.getTS()*sizeof(CDB::TRI));
		MFS->close_chunk		();

		MFS->open_chunk			(1);
		MFS->w					(&*rc_faces.begin(),(u32)rc_faces.size()*sizeof(b_rc_face));
		MFS->close_chunk		();
	}
}
