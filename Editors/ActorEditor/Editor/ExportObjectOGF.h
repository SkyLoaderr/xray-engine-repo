//---------------------------------------------------------------------------

#ifndef ExportObjectOGFH
#define ExportObjectOGFH

#include "progmesh.h"
//---------------------------------------------------------------------------
const int clpMX = 28, clpMY=16, clpMZ=28;
//---------------------------------------------------------------------------
// refs
class CEditableObject;
class CSurface;
class CInifile;

struct SOGFVert{
	Fvector		P;
	Fvector		N;
    Fvector2	UV;
	SOGFVert(){
		P.set	(0,0,0);
		N.set	(0,1,0);
        UV.set	(0.f,0.f);
	}
	void set(Fvector& p, Fvector& n, Fvector2& uv){
		P.set	(p);
  		N.set	(n);
        UV.set	(uv);
	}
	BOOL	similar_pos(SOGFVert& V){
        return P.similar(V.P,EPS_L);
    }
	BOOL	similar(SOGFVert& V){
        if (!P.similar	(V.P,EPS_L))	return FALSE;
        if (!UV.similar	(V.UV,EPS_S))	return FALSE;
		if (!N.similar	(V.N,EPS_L))	return FALSE;
		return TRUE;
	}
};

struct SOGFFace{
	WORD		v[3];
};

DEFINE_VECTOR(SOGFVert,OGFVertVec,OGFVertIt);
DEFINE_VECTOR(SOGFFace,OGFFaceVec,OGFFaceIt);

class CObjectOGFCollectorPacked
{
protected:
    OGFVertVec		m_Verts;
    OGFFaceVec		m_Faces;

    Fvector			m_VMmin, m_VMscale;
    DWORDVec		m_VM[clpMX+1][clpMY+1][clpMZ+1];
    Fvector			m_VMeps;

    DWORD			VPack(SOGFVert& V);
public:
    CObjectOGFCollectorPacked	(const Fbox &bb, int apx_vertices=5000, int apx_faces=5000);
    bool 			check      	(SOGFFace& F){
		if ((F.v[0]==F.v[1]) || (F.v[0]==F.v[2]) || (F.v[1]==F.v[2])) return false;
        for (OGFFaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
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
	void add_face	(SOGFVert& v0, SOGFVert& v1, SOGFVert& v2){
		if (v0.P.similar(v1.P,EPS_L) || v0.P.similar(v2.P,EPS_L) || v1.P.similar(v2.P,EPS_L)){
			ELog.Msg(mtError,"Degenerate face found. Removed.");
            return;
        }
        SOGFFace F;
        F.v[0]	= VPack(v0);
        F.v[1]	= VPack(v1);
        F.v[2]	= VPack(v2);
        if (check(F)) 	m_Faces.push_back(F);
        else			ELog.Msg(mtError,"Dublicate(degenerate) face found. Removed.");
    }
    OGFVertVec& 	getV_Verts()	{return m_Verts;}
    OGFFaceVec& 	getV_Faces()	{return m_Faces;}
    SOGFVert*		getVert()		{ return m_Verts.begin();	}
    int				getVS()			{ return m_Verts.size();	}
    int				getTS() 		{ return m_Faces.size();	}
};
//----------------------------------------------------


class CExportObjectOGF{
    struct SSplit: public CObjectOGFCollectorPacked{
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
			CExportObjectOGF	(CEditableObject* object);
    bool    ExportGeometry		(CFS_Base& F);
};

#endif
