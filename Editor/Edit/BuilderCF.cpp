//----------------------------------------------------
// file: BuilderFakeCF.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "xrLevel.h"
#include "cl_collector.h"

bool SceneBuilder::SaveObjectVCF(const char* name, CEditObject* obj){
	CFS_Memory F;
    bool bRes = BuildObjectVCF(F, (obj->IsReference())?obj->GetRef():obj);
    if (bRes) F.SaveTo(name,0);
    return bRes;
}

struct st_VCFFace{
	int p[3];
    st_VCFFace(int _0, int _1, int _2){p[0]=_0;p[1]=_1;p[2]=_2;}
};
DEFINE_VECTOR(st_VCFFace,VCFFaceVec,VCFFaceIt);

bool SceneBuilder::BuildObjectVCF(CFS_Base& FM, CEditObject* obj){
    FvectorVec		V;
    VCFFaceVec		F;
    
    Fbox          	BB;
    DWORD           offs;
	Fvector         C;
    float           R;
    bool			bFirstInit=true;

    obj->GetBox		(BB);

    
    for(EditMeshIt m=obj->FirstMesh();m!=obj->LastMesh();m++){
        if (bFirstInit){
            bFirstInit = false;
            BB.set((*m)->m_Box);
        }else{
            BB.merge((*m)->m_Box);
        }
        offs = V.size();
            
        for(FvectorIt V_it=(*m)->m_Points.begin(); V_it!=(*m)->m_Points.end(); V_it++) V.push_back(*V_it);
        for(FaceIt f_it=(*m)->m_Faces.begin();f_it!=(*m)->m_Faces.end();f_it++){ 
            F.push_back(st_VCFFace(	f_it->pv[0].pindex+offs,
                                    f_it->pv[1].pindex+offs,
                                    f_it->pv[2].pindex+offs));
        }
    }
    if (F.empty()||V.empty()){
    	Log->DlgMsg(mtError,"Can't create collision model. Model is empty.");
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

bool SceneBuilder::BuildVCFModels( ){
	int objcount = Scene->ObjCount(OBJCLASS_EDITOBJECT);
	if( objcount <= 0 ) return true;

    char temp[MAX_OBJ_NAME];
  	int handle;
	// -- mobile objects --
	ObjectIt i = Scene->FirstObj(OBJCLASS_EDITOBJECT);
	ObjectIt _E = Scene->LastObj(OBJCLASS_EDITOBJECT);
	for(;i!=_E;i++){
        CEditObject *obj = (CEditObject*)(*i);
        if(obj->IsDynamic() && !obj->IsReference()){
            sprintf(temp,"%s.vcf",obj->GetName());
            m_LevelPath.Update(temp);
            CFS_Memory F;
            BuildObjectVCF(F, obj);
            F.SaveTo(temp,0);
        }
	}
	return true;
}

