//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef EditMeshH
#define EditMeshH

//----------------------------------------------------
// refs
class CSurface;
struct SRayPickInfo;
//struct CFrustum;
struct FSChunkDef;
class CExporter;

#include "sceneclasslist.h"

#pragma pack( push,1 )
enum EVMType{
	vmtUV=0,
    vmtWeight,
    vmt_force_byte = 0xff
};

struct ECORE_API st_WB{
	int 	bone;
	float 	weight;
			st_WB	():bone(-1),weight(0){;}
			st_WB	(int b, float w):bone(b),weight(w){;}
	void	set		(int b, float w){bone=b;weight=w;}
};
DEFINE_VECTOR(st_WB,WBVec,WBIt);
struct ECORE_API st_VertexWB:public WBVec{
protected:
	static bool compare_by_weight(const st_WB& a, const st_WB& b)
	{
		return a.weight > b.weight; // отсортировать по убыванию
	}
	static bool compare_by_bone(const st_WB& a, const st_WB& b)
	{
		return a.bone < b.bone; // отсортировать по возрастанию
	}
public:
	void sort_by_bone()
	{
		std::sort(begin(),end(),compare_by_bone);
	}
	void sort_by_weight()
	{
		std::sort(begin(),end(),compare_by_weight);
	}
	void normalize_weights(int max_influence)
	{
		if ((int)size()>max_influence){	
			WBIt it;
			sort_by_weight	();
			erase			(begin()+2,end()); // delete >2 weight
			float sum_weight=0;
			for (it=begin(); it!=end(); it++) sum_weight+=it->weight;
			for (it=begin(); it!=end(); it++) it->weight/=sum_weight;
			sort_by_bone	();
		}
	}
};
DEFINE_VECTOR(st_VertexWB,VWBVec,VWBIt);

struct ECORE_API st_VMapPt{
	int				vmap_index;	// ссылка на мапу
	int				index;		// индекс в мапе на uv
	st_VMapPt(){vmap_index=-1;index=-1;}
};
// uv's
class ECORE_API st_VMap{
    FloatVec    	vm;			// u,v - координаты или weight
public:
	string128		name;		// vertex uv map name
    u8				dim;
    EVMType			type;
	BOOL			polymap;
	IntVec			vindices;
	IntVec			pindices;
public:
	st_VMap			(LPCSTR nm=0, EVMType t=vmtUV, BOOL pm=false){
		type=t;
		polymap=pm;
		if(nm) strcpy(name,nm); else name[0]=0;
		if (t==vmtUV) dim=2; else dim=1;
	}
    IC Fvector2&	getUV		(int idx)				{VERIFY(type==vmtUV);		return (Fvector2&)vm[idx*dim];}
    IC float&		getW		(int idx)				{VERIFY(type==vmtWeight);	return vm[idx];}
    IC FloatVec&	getVM		()						{return vm;}
	IC float*		getVMdata	()						{return &*vm.begin();}
    IC float*		getVMdata	(int start)				{return &*(vm.begin()+start*dim);}
    IC int			VMdatasize	()						{return vm.size()*sizeof(float);}
    IC int*			getVIdata	()						{return &*vindices.begin();}
    IC int			VIdatasize	()						{return vindices.size()*sizeof(int);}
    IC int*			getPIdata	()						{return &*pindices.begin();}
    IC int			PIdatasize	()						{return pindices.size()*sizeof(int);}
    IC int			size		()						{return vm.size()/dim;}
    IC void			resize		(int cnt)				{vm.resize(cnt*dim);vindices.resize(cnt);if (polymap) pindices.resize(cnt); }
	IC void			appendUV	(float u, float v)		{vm.push_back(u);vm.push_back(v);}
	IC void			appendUV	(Fvector2& uv)			{appendUV(uv.x,uv.y);}
	IC void			appendW		(float w)				{vm.push_back(w);}
	IC void			appendVI	(int vi)				{vindices.push_back(vi);}
	IC void			appendPI	(int pi)				{VERIFY(polymap); pindices.push_back(pi);}
    IC void			copyfrom	(float* src, int cnt)	{resize(cnt); CopyMemory(&*vm.begin(),src,cnt*dim*4);}
};
DEFINE_SVECTOR		(st_VMapPt,8,VMapPtSVec,VMapPtIt);
DEFINE_VECTOR		(VMapPtSVec,VMRefsVec,VMRefsIt);

struct ECORE_API st_SVert{
	Fvector			offs;
    Fvector			norm;
    u16				bone0;
    u16				bone1;
    float			w;
    Fvector2		uv;
};
// faces
struct ECORE_API st_FaceVert{
	int 			pindex;		// point index in PointList
    int				vmref;		// vm index
};
struct ECORE_API st_Face{
    st_FaceVert		pv[3];		// face vertices (P->P...)
};

// mesh options
struct ECORE_API st_MeshOptions{
	int 			m_Reserved0;
	int 			m_Reserved1;
    st_MeshOptions	(){m_Reserved0=0;m_Reserved1=0;}
};
#pragma pack( pop )

DEFINE_VECTOR		(IntVec,AdjVec,AdjIt);
DEFINE_VECTOR		(st_VMap*,VMapVec,VMapIt);
DEFINE_VECTOR		(st_Face,FaceVec,FaceIt);
DEFINE_MAP			(CSurface*,IntVec,SurfFaces,SurfFacesPairIt);
DEFINE_VECTOR		(st_SVert,SVertVec,SVertIt);

//refs
struct st_RenderBuffer;
class CSurface;
class CSector;

#ifdef _EDITOR
	struct ECORE_API st_RenderBuffer{
		u32			dwStartVertex;
	    u32			dwNumVertex;
        ref_geom 	pGeom;
		st_RenderBuffer	(u32 sv, u32 nv):dwStartVertex(sv),dwNumVertex(nv),pGeom(0){;}
	};
	DEFINE_VECTOR(st_RenderBuffer,RBVector,RBVecIt);
	DEFINE_MAP(CSurface*,RBVector,RBMap,RBMapPairIt);
#endif

class ECORE_API CEditableMesh {
	friend class MeshExpUtility;
	friend class CEditableObject;
    friend class CSectorItem;
    friend class CSector;
    friend class CPortalUtils;
    friend class SceneBuilder;
    friend class CExportSkeleton;
    friend class CExportObjectOGF;
    friend class TfrmEditLibrary;
	friend class CExporter;

	string128		m_Name;

    CEditableObject*	m_Parent;

#ifdef _EDITOR
    CDB::MODEL*		m_CFModel;
	RBMap			m_RenderBuffers;
#endif

    void            GenerateCFModel		();
    void 			GenerateFNormals	();
    void 			GeneratePNormals	();
    void            GenerateSVertices	();
    void			UnloadCForm     	();
    void			UnloadFNormals   	();
    void			UnloadPNormals   	();
    void			UnloadSVertices  	();

    // internal variables
	enum{
		flVisible	= (1<<0),
		flLocked	= (1<<1),
		flSGMask	= (1<<2),
	};
	Flags8			m_Flags;
public:
	st_MeshOptions	m_Ops;

    enum{
        LS_CF_MODEL = (1<<0),
        LS_RBUFFERS	= (1<<1),
        LS_FNORMALS = (1<<2),
        LS_PNORMALS = (1<<3),
		LS_SVERTICES= (1<<4),
    };
    Flags32			m_LoadState;
protected:
	Fbox			m_Box;

    FvectorVec	    m_Points;	// |
    AdjVec		    m_Adjs;     // + some array size!!!
    SurfFaces	    m_SurfFaces;
	U32Vec			m_SGs;		// |
    FvectorVec	    m_FNormals;	// |
    FvectorVec	    m_PNormals;	// | *3
    SVertVec	    m_SVertices;// | *3
    FaceVec		    m_Faces;    // + some array size!!!
    VMapVec		    m_VMaps;
    VMRefsVec	    m_VMRefs;

	void 			CreateRenderBuffers		();
	void 			FillRenderBuffer		(IntVec& face_lst, int start_face, int num_face, const CSurface* surf, LPBYTE& data);

	void 			RecurseTri				(int id);


	// mesh optimize routine
	bool 			UpdateAdjacency			();
	bool 			OptimizeFace			(st_Face& face);
public:
	void			RecomputeBBox			();
	void 			Optimize				(BOOL NoOpt);
public:
	                CEditableMesh			(CEditableObject* parent){m_Parent=parent;Construct();}
	                CEditableMesh			(CEditableMesh* source,CEditableObject* parent){m_Parent=parent;Construct();CloneFrom(source);}
	virtual         ~CEditableMesh			();
	void			Construct				();
    void			Clear					();

	IC void			SetName					(LPCSTR name){strcpy(m_Name,name);}
	IC char*		GetName					(){return m_Name;}
	void            GetBox					(Fbox& box){box.set(m_Box);}
	CSurface*		GetSurfaceByFaceID		(u32 fid);
	void			GetFaceTC				(u32 fid, const Fvector2* tc[3]);
	void			GetFacePT				(u32 fid, const Fvector* pt[3]);
	IC BOOL 		Visible					(){return m_Flags.is(flVisible); }
	IC void 		Show					(BOOL bVisible){m_Flags.set(flVisible,bVisible);}

    // mesh modify routine
	void            CloneFrom				(CEditableMesh *source);
	void            Transform				(const Fmatrix& parent);

	IC CEditableObject*	Parent				(){ return m_Parent;}
    IC FaceVec&		GetFaces				(){ return m_Faces;}
	IC U32Vec&		GetSGs					(){ return m_SGs;}
    IC FvectorVec&	GetPoints				(){ return m_Points;}
	IC VMapVec&		GetVMaps				(){ return m_VMaps;}
	IC VMRefsVec&	GetVMRefs				(){ return m_VMRefs;}
    IC FvectorVec&	GetFNormals				(){ if (!m_LoadState.is(CEditableMesh::LS_FNORMALS)) GenerateFNormals(); return m_FNormals;}
    IC FvectorVec&	GetPNormals				(){ if (!m_LoadState.is(CEditableMesh::LS_PNORMALS)) GeneratePNormals(); return m_PNormals;}
    IC SVertVec&	GetSVertices			(){ if (!m_LoadState.is(CEditableMesh::LS_SVERTICES))GenerateSVertices();return m_SVertices;}
	IC SurfFaces&	GetSurfFaces			(){ return m_SurfFaces;}
	    
    // pick routine
	bool            RayPick					(float& dist, const Fvector& start, const Fvector& dir, const Fmatrix& inv_parent, SRayPickInfo* pinf = NULL);
#ifdef _EDITOR
	void            RayQuery				(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
	void            BoxQuery				(const Fmatrix& parent, const Fmatrix& inv_parent, SPickQuery& pinf);
    bool 			BoxPick					(const Fbox& box, const Fmatrix& inv_parent, SBoxPickInfoVec& pinf);
	bool            FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    void            FrustumPickFaces		(const CFrustum& frustum, const Fmatrix& parent, U32Vec& fl);
    bool			CHullPickMesh			(PlaneVec& pl, const Fmatrix& parent);
	void 			GetTiesFaces			(int start_id, U32Vec& fl, float fSoftAngle, bool bRecursive);
#endif

    // render routine
	void 			Render					(const Fmatrix& parent, CSurface* S);
	void 			RenderSkeleton			(const Fmatrix& parent, CSurface* S);
	void            RenderList				(const Fmatrix& parent, u32 color, bool bEdge, IntVec& fl);
	void 			RenderSelection			(const Fmatrix& parent, CSurface* s, u32 color);
	void 			RenderEdge				(const Fmatrix& parent, CSurface* s, u32 color);

    // statistics methods
    int 			GetFaceCount			(bool bMatch2Sided=true);
	int 			GetVertexCount			(){return m_Points.size();}
    int 			GetSurfFaceCount		(CSurface* surf, bool bMatch2Sided=true);

    // IO - routine
    void			SaveMesh				(IWriter&);
	bool 			LoadMesh				(IReader&);

    // debug
    void			DumpAdjacency			();

	// convert
#ifdef _MAX_EXPORT
    void			FlipFaces				();
	TriObject*		ExtractTriObject		(INode *node, int &deleteIt);
	bool			Convert					(INode *node);
	bool			Convert					(CExporter* exporter);
#endif

	int				FindSimilarUV			(st_VMap* vmap, Fvector2& _uv);
	int				FindSimilarWeight		(st_VMap* vmap, float _w);
	int				FindVMapByName			(VMapVec& vmaps, const char* name, EVMType t, BOOL polymap);
	void			RebuildVMaps			();

    // device dependent routine
	void 			ClearRenderBuffers		();
};
//----------------------------------------------------
#endif /*_INCDEF_EditableMesh_H_*/

