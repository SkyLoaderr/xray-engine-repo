//---------------------------------------------------------------------------

#ifndef ExportSkeletonH
#define ExportSkeletonH

#include "progmesh.h"
//---------------------------------------------------------------------------
const DWORD BONE_NONE 	= 0xffffffff;
const float KEY_Quant	= 32767.f;
const int clpSMX = 28, clpSMY=16, clpSMZ=28;
//---------------------------------------------------------------------------
// refs
class CEditableObject;
class CSurface;
class CInifile;

struct SSkelVert{
	Fvector		P;
	Fvector		O;
	Fvector		N;
    Fvector2	UV;
	DWORD		B;
	SSkelVert(){
		P.set	(0,0,0);
        O.set   (0,0,0);
		N.set	(0,1,0);
        UV.set	(0.f,0.f);
		B		= BONE_NONE;
	}
	void set(Fvector& p, Fvector& o, Fvector& n, Fvector2& uv, DWORD b){
		P.set	(p);
        O.set   (o);
		N.set	(n);
        UV.set	(uv);
		B		= b;
	}
	void	setbone(DWORD b){
		if (B==BONE_NONE){
			B = b;
		} else R_ASSERT(B==b);
	}
	BOOL	similar_pos(SSkelVert& V){
        return P.similar(V.P,EPS_L);
    }
	BOOL	similar(SSkelVert& V){
		if (B!=V.B)						return FALSE;
        if (!P.similar	(V.P,EPS_L))	return FALSE;
        if (!UV.similar	(V.UV,EPS_S))	return FALSE;
		if (!O.similar	(V.O,EPS_L))	return FALSE;
		if (!N.similar	(V.N,EPS_L))	return FALSE;
		return TRUE;
	}
};

struct SSkelFace{
	WORD		v[3];
};

DEFINE_VECTOR(SSkelVert,SkelVertVec,SkelVertIt);
DEFINE_VECTOR(SSkelFace,SkelFaceVec,SkelFaceIt);

class CSkeletonCollectorPacked
{
protected:
    SkelVertVec		m_Verts;
    SkelFaceVec		m_Faces;

    Fvector			m_VMmin, m_VMscale;
    DWORDVec		m_VM[clpSMX+1][clpSMY+1][clpSMZ+1];
    Fvector			m_VMeps;

    DWORD			VPack(SSkelVert& V);
public:
    CSkeletonCollectorPacked	(const Fbox &bb, int apx_vertices=5000, int apx_faces=5000);
    bool 			check      	(SSkelFace& F){
		if ((F.v[0]==F.v[1]) || (F.v[0]==F.v[2]) || (F.v[1]==F.v[2])) return false;
        for (SkelFaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
            // Test for 6 variations
            if ((f_it->v[0]==F.v[0]) && (f_it->v[1]==F.v[1]) && (f_it->v[2]==F.v[2])) return false;
            if ((f_it->v[0]==F.v[0]) && (f_it->v[2]==F.v[1]) && (f_it->v[1]==F.v[2])) return false;
            if ((f_it->v[2]==F.v[0]) && (f_it->v[0]==F.v[1]) && (f_it->v[1]==F.v[2])) return false;
            if ((f_it->v[2]==F.v[0]) && (f_it->v[1]==F.v[1]) && (f_it->v[0]==F.v[2])) return false;
            if ((f_it->v[1]==F.v[0]) && (f_it->v[0]==F.v[1]) && (f_it->v[2]==F.v[2])) return false;
            if ((f_it->v[1]==F.v[0]) && (f_it->v[2]==F.v[1]) && (f_it->v[0]==F.v[2])) return false;
        }
        return true;
    }
	void add_face	(SSkelVert& v0, SSkelVert& v1, SSkelVert& v2){
		if (v0.P.similar(v1.P,EPS_L) || v0.P.similar(v2.P,EPS_L) || v1.P.similar(v2.P,EPS_L)){
			ELog.Msg(mtError,"Degenerate face found. Removed.");
            return;
        }
        SSkelFace F;
        F.v[0]	= VPack(v0);
        F.v[1]	= VPack(v1);
        F.v[2]	= VPack(v2);
        if (check(F)) 	m_Faces.push_back(F);
        else			ELog.Msg(mtError,"Dublicate(degenerate) face found. Removed.");
    }
    SkelVertVec& 	getV_Verts()	{return m_Verts;}
    SkelFaceVec& 	getV_Faces()	{return m_Faces;}
    SSkelVert*		getVert()		{ return m_Verts.begin();	}
    int				getVS()			{ return m_Verts.size();	}
//    SSkelFace&		getFace(int id)	{ return VERIFY(id<m_Faces.size()); m_Faces[id];	}
    int				getTS() 		{ return m_Faces.size();	}
};
//----------------------------------------------------


class CExportSkeleton{
    struct SSplit: public CSkeletonCollectorPacked{
        string128		m_Shader;
        string128		m_Texture;
        Fbox			m_Box;

        // Progressive
        int				I_Current;
        int				V_Minimal;
        std::vector<Vsplit>	pmap_vsplit;
        std::vector<WORD>	pmap_faces;

        void 			Save	(CFS_Base& F);

		void 			MakeProgressive();
        SSplit			(CSurface* surf, const Fbox& bb);
    };
	DEFINE_VECTOR		(SSplit,SplitVec,SplitIt);
	SplitVec			m_Splits;
	CEditableObject*	m_Source;
//----------------------------------------------------
	void 	ComputeOBB			(Fobb &B, FvectorVec& V);
    int     FindSplit			(LPCSTR shader, LPCSTR texture);
public:
			CExportSkeleton		(CEditableObject* object);
    bool    Export				(CFS_Base& F);
    bool    ExportGeometry		(CFS_Base& F);
    bool    ExportMotions		(CFS_Base& F);
};

#endif
