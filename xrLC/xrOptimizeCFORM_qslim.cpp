#include "stdafx.h"
#include "build.h"
#include <MxStdModel.h>
#include <MxQSlim.h>

//#pragma comment (lib,"x:/xrQSlim.lib")

#define MAX_DECIMATE_ERROR 0.05f
#define COMPACTNESS_RATIO  0.5f

void SaveAsSMF			(LPCSTR fname, CDB::CollectorPacked& CL)
{
	IWriter* W			= FS.w_open(fname);
	string256 tmp;
	// vertices
	for (u32 v_idx=0; v_idx<CL.getVS(); v_idx++){
		Fvector* v		= CL.getV()+v_idx;
		sprintf			(tmp,"v %f %f %f",v->x,v->y,-v->z);
		W->w_string		(tmp);
	}
	// transfer faces
	for (u32 f_idx=0; f_idx<CL.getTS(); f_idx++){
		CDB::TRI* t		= CL.getT()+f_idx;
		sprintf			(tmp,"f %d %d %d",t->verts[0]+1,t->verts[2]+1,t->verts[1]+1);
		W->w_string		(tmp);
	}
	FS.w_close	(W);
}

struct face_props	{
	u16		material;
	u16		sector;
	void	set		(u16 mtl, u16 sect){material=mtl;sector=sect;}
};
DEFINE_VECTOR(face_props,FPVec,FPVecIt);

void SimplifyCFORM		(CDB::CollectorPacked& CL)
{
	FPVec FPs;

	u32 base_verts_cnt		= CL.getVS();
	u32 base_faces_cnt		= CL.getTS();

	// save source SMF
	string_path				fn;
	SaveAsSMF				(strconcat(fn,pBuild->path,"cform_source.smf"),CL);

	// prepare model
	MxStdModel* mdl			= xr_new<MxStdModel>(base_verts_cnt,base_faces_cnt);

	// transfer vertices
	for (u32 v_idx=0; v_idx<base_verts_cnt; v_idx++){
		Fvector* v			= CL.getV()+v_idx;
		mdl->add_vertex		(v->x,v->y,v->z);
	}
	// transfer faces
	FPs.resize				(base_faces_cnt);
	for (u32 f_idx=0; f_idx<base_faces_cnt; f_idx++){
		CDB::TRI* t			= CL.getT()+f_idx;
		mdl->add_face		(t->verts[0],t->verts[1],t->verts[2]);
		FPs[f_idx].set		(t->material,t->sector);
	}
	CL.clear				();

	// create and initialize qslim
	MxEdgeQSlim* slim		= xr_new<MxEdgeQSlim>(mdl);
	slim->boundary_weight	= 1000.f;
	slim->compactness_ratio	= COMPACTNESS_RATIO;
	slim->meshing_penalty	= 1000.f;
	slim->placement_policy	= MX_PLACE_OPTIMAL;
	slim->weighting_policy	= MX_WEIGHT_AREA_AVG;
	slim->initialize		();

	// constraint material&sector vertex
	Ivector2 f_rm[3]={{0,1}, {1,2}, {2,0}};
	for (f_idx=0; f_idx<slim->valid_faces; f_idx++){
		if (mdl->face_is_valid(f_idx)){
			MxFace& F					= mdl->face(f_idx);
			for (u32 edge_idx=0; edge_idx<3; edge_idx++){
				int K;
				u32 I					= f_rm[edge_idx].x;
				u32 J					= f_rm[edge_idx].y;
				const MxFaceList& N0	= mdl->neighbors(F[I]);
				const MxFaceList& N1	= mdl->neighbors(F[J]);
				for(K=0; K<N1.length(); K++) mdl->face_mark(N1[K], 0);
				for(K=0; K<N0.length(); K++) mdl->face_mark(N0[K], 1);
				for(K=0; K<N1.length(); K++) mdl->face_mark(N1[K], mdl->face_mark(N1[K])+1);
				const MxFaceList& N		= (N0.size()<N1.size())?N0:N1;
				face_props& base_t		= FPs[f_idx];
				if (N.size()){
					for(K=0; K<N.length(); K++){
						u32 fff			= N[K];
						unsigned char mk= mdl->face_mark(fff);
						if((f_idx!=N[K])&&(mdl->face_mark(N[K])==2)){
							face_props& cur_t	= FPs[N[K]];
							if ((cur_t.material!=base_t.material)||(cur_t.sector!=base_t.sector)){
								slim->constraint_manual	(F[I],F[J],f_idx);
								break;
							}
						}
					}
				}
			}
		}
	}
	// collect edges
	slim->collect_edges		();

	// decimate
	slim->decimate			(0,MAX_DECIMATE_ERROR);
	mdl->compact_vertices	();

	// rebuild CDB
	for (f_idx=0; f_idx<mdl->face_count(); f_idx++){
		if (mdl->face_is_valid(f_idx)){
			MxFace& F		= mdl->face(f_idx);
			face_props& FP	= FPs[f_idx];
			CL.add_face		(*((Fvector*)&mdl->vertex(F[0])),
							*((Fvector*)&mdl->vertex(F[1])),
							*((Fvector*)&mdl->vertex(F[2])),
							FP.material, FP.sector);
		}
	}


	// save source CDB
	SaveAsSMF				(strconcat(fn,pBuild->path,"cform_optimized.smf"),CL);

	xr_delete				(slim);
	xr_delete				(mdl);
}
