//---------------------------------------------------------------------------

#ifndef ExportSkeletonH
#define ExportSkeletonH

#include "PropSlimTools.h"
#include "SkeletonCustom.h"
//---------------------------------------------------------------------------
const int clpSMX = 28, clpSMY=16, clpSMZ=28;
//---------------------------------------------------------------------------
// refs                                           
class CEditableObject;
class CSurface;
class CInifile;

struct ECORE_API SSkelVert{
	Fvector		O;
	Fvector		N;
    Fvector		T;
    Fvector		B;
    Fvector2	UV;
	u16			B0;
	u16			B1;
    float 		w;
	SSkelVert(){
        UV.set	(0.f,0.f);
        O.set	(0,0,0);
		N.set	(0,1,0);
        T.set	(1,0,0);
		B.set	(0,0,1);
		B0		= BI_NONE;
		B1		= BI_NONE;
        w		= 0;
	}
    void set(const Fvector& o, const Fvector& n, const Fvector2& uv, float _w, u16 b0, u16 b1=BI_NONE)
    {
        O.set   (o);
        N.set	(n);
        UV.set	(uv);
        w		= _w;
        B0		= b0;
        B1		= b1;
    }
	BOOL	similar_pos(SSkelVert& V)
    {
        return O.similar(V.O,EPS_L);
    }
	BOOL	similar(SSkelVert& V)
    {
		if (B0!=V.B0)					return FALSE;
		if (B1!=V.B1)					return FALSE;
        if (!UV.similar	(V.UV,EPS_S))	return FALSE;
		if (!O.similar	(V.O,EPS_L))	return FALSE;
		if (!N.similar	(V.N,EPS_L))	return FALSE;
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
	void add_face	(SSkelVert& v0, SSkelVert& v1, SSkelVert& v2)
    {
		if (v0.O.similar(v1.O,EPS) || v0.O.similar(v2.O,EPS) || v1.O.similar(v2.O,EPS)){
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
    	shared_str		m_Shader;
        shared_str		m_Texture;
        Fbox			m_Box;

        // Progressive
		ArbitraryList<VIPM_SWR>	m_SWR;// The records of the collapses.
	    BOOL			m_b2Link;
    public:
        SSplit			(CSurface* surf, const Fbox& bb);

        bool			valid()
        {
        	if (m_Verts.empty()) return false;
        	if (m_Faces.empty()) return false;
            return true;
        }
		void 			MakeProgressive	();
		void 			CalculateTB		();

        void 			Save			(IWriter& F);

        void			ComputeBounding	()
        {
            // calculate BBox
            m_Box.invalidate	();
            for (SkelVertIt v_it=m_Verts.begin(); v_it!=m_Verts.end(); v_it++){
                SSkelVert& pV 	= *v_it;
                m_Box.modify(pV.O);
            }
        }
    };
	DEFINE_VECTOR		(SSplit,SplitVec,SplitIt);
	SplitVec			m_Splits;
    Fbox 				m_Box;
//----------------------------------------------------
    int     			FindSplit			(LPCSTR shader, LPCSTR texture);
    void				ComputeBounding	()
    {
        m_Box.invalidate();
        for (SplitIt it=m_Splits.begin(); it!=m_Splits.end(); it++){
            it->ComputeBounding	();
            m_Box.merge			(it->m_Box);
        }
    }
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

void ECORE_API 			ComputeOBB			(Fobb &B, FvectorVec& V);

#endif
