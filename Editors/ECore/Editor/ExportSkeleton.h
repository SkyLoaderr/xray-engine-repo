//---------------------------------------------------------------------------

#ifndef ExportSkeletonH
#define ExportSkeletonH

#include "progmesh.h"
#include "SkeletonCustom.h"
//---------------------------------------------------------------------------
const int clpSMX = 28, clpSMY=16, clpSMZ=28;
//---------------------------------------------------------------------------
// refs
class CEditableObject;
class CSurface;
class CInifile;

struct ECORE_API SSkelVert{
	Fvector		P;
	Fvector		O0;
	Fvector		O1;
	Fvector		N0;
	Fvector		N1;
    Fvector2	UV;
	u16			B0;
	u16			B1;
    float 		w;
	SSkelVert(){
		P.set	(0,0,0);
        UV.set	(0.f,0.f);
        O0.set	(0,0,0);
		N0.set	(0,1,0);
		B0		= BI_NONE;
        O1.set	(0,0,0);
		N1.set	(0,1,0);
		B1		= BI_NONE;
        w		= 0;
	}
    void set(Fvector& p, Fvector2& uv, float _w)
    {
        P.set   (p);
        UV.set	(uv);
        w		= _w;
    }
	void set0(Fvector& o, Fvector& n, u32 b)
    {
        O0.set	(o);
		N0.set	(n);
		B0		= b;
	}
	void set1(Fvector& o, Fvector& n, u32 b)
    {
        O1.set   (o);
		N1.set	(n);
		B1		= b;
	}
	BOOL	similar_pos(SSkelVert& V){
        return P.similar(V.P,EPS_L);
    }
	BOOL	similar(SSkelVert& V){
		if (B0!=V.B0)					return FALSE;
		if (B1!=V.B1)					return FALSE;
        if (!P.similar	(V.P,EPS_L))	return FALSE;
        if (!UV.similar	(V.UV,EPS_S))	return FALSE;
		if (!O0.similar	(V.O0,EPS_L))	return FALSE;
		if (!N0.similar	(V.N0,EPS_L))	return FALSE;
		if (!O1.similar	(V.O1,EPS_L))	return FALSE;
		if (!N1.similar	(V.N1,EPS_L))	return FALSE;
		return TRUE;
	}
};

struct ECORE_API SSkelFace{
	WORD		v[3];
};

DEFINE_VECTOR(SSkelVert,SkelVertVec,SkelVertIt);
DEFINE_VECTOR(SSkelFace,SkelFaceVec,SkelFaceIt);

class ECORE_API CSkeletonCollectorPacked
{
protected:
    SkelVertVec		m_Verts;
    SkelFaceVec		m_Faces;

    Fvector			m_VMmin, m_VMscale;
    U32Vec			m_VM[clpSMX+1][clpSMY+1][clpSMZ+1];
    Fvector			m_VMeps;

    u32			VPack(SSkelVert& V);
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
		if (v0.P.similar(v1.P,EPS) || v0.P.similar(v2.P,EPS) || v1.P.similar(v2.P,EPS)){
			ELog.Msg(mtError,"Degenerate face found. Removed.");
            return;
        }
        SSkelFace F;
        F.v[0]	= VPack(v0);
        F.v[1]	= VPack(v1);
        F.v[2]	= VPack(v2);
        if (check(F)) 	m_Faces.push_back(F);
        else			ELog.Msg(mtError,"Duplicate(degenerate) face found. Removed.");
    }
    SkelVertVec& 	getV_Verts()	{return m_Verts;}
    SkelFaceVec& 	getV_Faces()	{return m_Faces;}
    SSkelVert*		getVert()		{ return m_Verts.begin();	}
    int				getVS()			{ return m_Verts.size();	}
//    SSkelFace&		getFace(int id)	{ return VERIFY(id<m_Faces.size()); m_Faces[id];	}
    int				getTS() 		{ return m_Faces.size();	}
};
//----------------------------------------------------

class ECORE_API CExportSkeletonCustom{
protected:
    struct ECORE_API SSplit: public CSkeletonCollectorPacked{
        ref_str			m_Shader;
        ref_str			m_Texture;
        Fbox			m_Box;

        // Progressive
        int				I_Current;
        int				V_Minimal;
        xr_vector<Vsplit>	pmap_vsplit;
        xr_vector<WORD>		pmap_faces;

        void 			Save	(IWriter& F, BOOL b2Link);

		void 			MakeProgressive();
        SSplit			(CSurface* surf, const Fbox& bb);
        bool			valid()
        {
        	if (m_Verts.empty()) return false;
        	if (m_Faces.empty()) return false;
            return true;
        }
    };
	DEFINE_VECTOR		(SSplit,SplitVec,SplitIt);
	SplitVec			m_Splits;
//----------------------------------------------------
	void 				ComputeOBB			(Fobb &B, FvectorVec& V);
    int     			FindSplit			(LPCSTR shader, LPCSTR texture);
public:
    virtual bool    	Export				(IWriter& F)=0;
};


class ECORE_API CExportSkeleton: public CExportSkeletonCustom{
	CEditableObject*	m_Source;
public:
						CExportSkeleton		(CEditableObject* object);
    virtual bool    	Export				(IWriter& F);
    virtual bool    	ExportGeometry		(IWriter& F);
    virtual bool    	ExportMotions		(IWriter& F);

    virtual bool    	ExportMotionKeys	(IWriter& F);
    virtual bool    	ExportMotionDefs	(IWriter& F);
};

#endif
