//---------------------------------------------------------------------------

#ifndef ExportObjectOGFH
#define ExportObjectOGFH

#include "progmesh.h"
//---------------------------------------------------------------------------
const int clpOGFMX = 48, clpOGFMY=16, clpOGFMZ=48;
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
public:
    OGFVertVec		m_Verts;
    OGFFaceVec		m_Faces;

    Fvector			m_VMmin, m_VMscale;
    U32Vec			m_VM[clpOGFMX+1][clpOGFMY+1][clpOGFMZ+1];
    Fvector			m_VMeps;

    u32				VPack(SOGFVert& V);
public:
    CObjectOGFCollectorPacked	(const Fbox &bb, int apx_vertices, int apx_faces);
    IC bool 		check      	(SOGFFace& F){
		if ((F.v[0]==F.v[1]) || (F.v[0]==F.v[2]) || (F.v[1]==F.v[2])) return false;
/*        for (OGFFaceIt f_it=m_Faces.begin(); f_it!=m_Faces.end(); f_it++){
            // Test for 6 variations
            if ((f_it->v[0]==F.v[0]) && (f_it->v[1]==F.v[1]) && (f_it->v[2]==F.v[2])) return false;
            if ((f_it->v[0]==F.v[0]) && (f_it->v[2]==F.v[1]) && (f_it->v[1]==F.v[2])) return false;
            if ((f_it->v[2]==F.v[0]) && (f_it->v[0]==F.v[1]) && (f_it->v[1]==F.v[2])) return false;
            if ((f_it->v[2]==F.v[0]) && (f_it->v[1]==F.v[1]) && (f_it->v[0]==F.v[2])) return false;
            if ((f_it->v[1]==F.v[0]) && (f_it->v[0]==F.v[1]) && (f_it->v[2]==F.v[2])) return false;
            if ((f_it->v[1]==F.v[0]) && (f_it->v[2]==F.v[1]) && (f_it->v[0]==F.v[2])) return false;
        }
*/        
		return true;
    }
	IC bool			add_face	(SOGFVert& v0, SOGFVert& v1, SOGFVert& v2){
		if (v0.P.similar(v1.P,EPS) || v0.P.similar(v2.P,EPS) || v1.P.similar(v2.P,EPS)){
			ELog.Msg(mtError,"Degenerate face found. Removed.");
            return true;
        }
        SOGFFace F;
        u32 v;
        v	= VPack(v0); if (0xffffffff==v) return false; F.v[0] = v;
        v	= VPack(v1); if (0xffffffff==v) return false; F.v[1] = v;
        v	= VPack(v2); if (0xffffffff==v) return false; F.v[2] = v;
        
        if (check(F)) 	m_Faces.push_back(F);
        else{	
			ELog.Msg(mtError,"Duplicate(degenerate) face found. Removed.");
            return true;
        }
        return true;
    }
    IC OGFVertVec& 	getV_Verts	()	{return m_Verts;}
   	IC OGFFaceVec& 	getV_Faces	()	{return m_Faces;}
    IC SOGFVert*	getVert		() 	{return m_Verts.begin();}
    IC int			getVS		() 	{return m_Verts.size();}
    IC int			getTS		() 	{return m_Faces.size();}
};
//----------------------------------------------------
DEFINE_VECTOR(CObjectOGFCollectorPacked*,COGFCPVec,COGFCPIt)


class CExportObjectOGF
{
    struct SSplit
    {
    	Fbox			apx_box;
        
    	COGFCPVec		m_Parts;
        CObjectOGFCollectorPacked* m_CurrentPart;
        
        Fbox			m_Box;
        CSurface*		m_Surf;

        // Progressive
//		int				I_Current;
//		int				V_Minimal;
//		std::vector<Vsplit>	pmap_vsplit;
//		std::vector<u16>	pmap_faces;

		void			AppendPart		(int apx_vertices, int apx_faces);
        void 			Save			(IWriter& F, int& chunk_id);

//		void 			MakeProgressive	();
        				SSplit			(CSurface* surf, const Fbox& bb);
        				~SSplit			();
    };
	DEFINE_VECTOR		(SSplit*,SplitVec,SplitIt);
	SplitVec			m_Splits;
	CEditableObject*	m_Source;
//----------------------------------------------------
//	void 	ComputeOBB			(Fobb &B, FvectorVec& V);
    SSplit*	FindSplit			(CSurface* surf);
public:
			CExportObjectOGF	(CEditableObject* object);
			~CExportObjectOGF	();
    bool    ExportGeometry		(IWriter& F);
};

#endif
