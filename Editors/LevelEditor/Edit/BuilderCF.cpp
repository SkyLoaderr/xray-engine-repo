//----------------------------------------------------
// file: BuilderFakeCF.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "SceneObject.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "xrLevel.h"
#include "cl_collector.h"
/*
bool SceneBuilder::SaveObjectVCF(LPCSTR name, CEditableObject* obj){
	CFS_Memory F;
    bool bRes = BuildObjectVCF(F, obj);
    if (bRes) F.SaveTo(name,0);
    return bRes;
}

struct st_VCFFace{
	int p[3];
    st_VCFFace(int _0, int _1, int _2){p[0]=_0;p[1]=_1;p[2]=_2;}
};
DEFINE_VECTOR(st_VCFFace,VCFFaceVec,VCFFaceIt);

bool SceneBuilder::BuildObjectVCF(CFS_Base& FM, CEditableObject* obj){
	ELog.DlgMsg(mtError,"Error BuildObjectVCF");
    FvectorVec		V;
    VCFFaceVec		F;

    DWORD           offs;
	Fvector         C;
    float           R;

    Fbox& BB		= obj->GetBox();

    for(EditMeshIt m=obj->FirstMesh();m!=obj->LastMesh();m++){
        offs = V.size();

        for(FvectorIt V_it=(*m)->m_Points.begin(); V_it!=(*m)->m_Points.end(); V_it++) V.push_back(*V_it);
        for(FaceIt f_it=(*m)->m_Faces.begin();f_it!=(*m)->m_Faces.end();f_it++){
            F.push_back(st_VCFFace(	f_it->pv[0].pindex+offs,
                                    f_it->pv[1].pindex+offs,
                                    f_it->pv[2].pindex+offs));
        }
    }
    if (F.empty()||V.empty()){
    	ELog.DlgMsg(mtError,"Can't create collision model. Model is empty.");
        return false;
    }
//------------------------------------------------------------------------------
	// Collect faces
	RAPID::CollectorPacked CLP(BB);
	for (DWORD k=0; k<F.size(); k++){
		st_VCFFace&	T = F[k];
//        SFaceTie& A = FT[k];
		CLP.add_face(
        	V[T.p[0]], V[T.p[1]], V[T.p[2]],
			-1,-1,-1,
//			A.t0,A.t1,A.t2,
			0,0,0);
	}

	// Header
	hdrCFORM hdr;
	hdr.version		= CFORM_CURRENT_VERSION;
	hdr.vertcount	= CLP.getVS();
	hdr.facecount	= CLP.getTS();
	hdr.aabb		= BB;
    FM.write		(&hdr,sizeof(hdr));

	// Data
	FM.write		(CLP.getV(),CLP.getVS()*sizeof(Fvector));
	FM.write		(CLP.getT(),CLP.getTS()*sizeof(RAPID::tri));
    return true;
}
//----------------------------------------------------
*/

