//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef _INCDEF_EditableMesh_H_
#define _INCDEF_EditableMesh_H_

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

struct st_VMapPt{
	int				vmap_index;	// ссылка на мапу
	int				index;		// индекс в мапе на uv
	st_VMapPt(){vmap_index=-1;index=-1;}
};
// uv's
class st_VMap{
    FloatVec    	vm;			// u,v - координаты или weight
public:
	string128		name;		// vertex uv map name
    BYTE			dim;
    EVMType			type;
	BOOL			polymap;
	INTVec			vindices;
	INTVec			pindices;
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
    IC float*		getVMdata	()						{return vm.begin();}
    IC float*		getVMdata	(int start)				{return vm.begin()+start*dim;}
    IC int			VMdatasize	()						{return vm.size()*sizeof(float);}
    IC int*			getVIdata	()						{return vindices.begin();}
    IC int			VIdatasize	()						{return vindices.size()*sizeof(int);}
    IC int*			getPIdata	()						{return pindices.begin();}
    IC int			PIdatasize	()						{return pindices.size()*sizeof(int);}
    IC int			size		()						{return vm.size()/dim;}
    IC void			resize		(int cnt)				{vm.resize(cnt*dim);vindices.resize(cnt);if (polymap) pindices.resize(cnt); }
	IC void			appendUV	(float u, float v)		{vm.push_back(u);vm.push_back(v);}
	IC void			appendUV	(Fvector2& uv)			{appendUV(uv.x,uv.y);}
	IC void			appendW		(float w)				{vm.push_back(w);}
	IC void			appendVI	(int vi)				{vindices.push_back(vi);}
	IC void			appendPI	(int pi)				{VERIFY(polymap); pindices.push_back(pi);}
    IC void			copyfrom	(float* src, int cnt)	{resize(cnt); CopyMemory(vm.begin(),src,cnt*dim*4);}
};
DEFINE_SVECTOR		(st_VMapPt,8,VMapPtSVec,VMapPtIt);
DEFINE_VECTOR		(VMapPtSVec,VMRefsVec,VMRefsIt);

struct st_SVert{
	Fvector			offs;
    Fvector			norm;
    int				bone;
};
// faces
struct st_FaceVert{
	int 			pindex;		// point index in PointList
    int				vmref;		// vm index
};
struct st_Face{
    st_FaceVert		pv[3];		// face vertices (P->P...)
};

// mesh options
struct st_MeshOptions{
	int 			m_Reserved0;
	int 			m_Reserved1;
    st_MeshOptions	(){m_Reserved0=0;m_Reserved1=0;}
};
#pragma pack( pop )

DEFINE_VECTOR		(INTVec,AdjVec,AdjIt);
DEFINE_VECTOR		(st_VMap*,VMapVec,VMapIt);
DEFINE_VECTOR		(st_Face,FaceVec,FaceIt);
DEFINE_MAP			(CSurface*,INTVec,SurfFaces,SurfFacesPairIt);
DEFINE_VECTOR		(st_SVert,SVertVec,SVertIt);

//refs
struct st_RenderBuffer;
class CSurface;
class CSector;

#ifdef _EDITOR
	struct st_RenderBuffer{
		DWORD			dwStartVertex;
	    DWORD			dwNumVertex;
        CVS*			pVS;
		IDirect3DVertexBuffer8*	pVB;
//		IDirect3DIndexBuffer8*	pIB;
		st_RenderBuffer	(DWORD sv, DWORD nv):dwStartVertex(sv),dwNumVertex(nv),pVS(0),pVB(0){;}
	};
	DEFINE_VECTOR(st_RenderBuffer,RBVector,RBVecIt);
	DEFINE_MAP(CSurface*,RBVector,RBMap,RBMapPairIt);
#endif

#define EMESH_LS_CF_MODEL	0x0001
#define EMESH_LS_FNORMALS 	0x0002
#define EMESH_LS_PNORMALS 	0x0004
#define EMESH_LS_SVERTICES 	0x0008

class CEditableMesh {
	friend class MeshExpUtility;
	friend class CEditableObject;
    friend class CSectorItem;
    friend class CSector;
    friend class CPortalUtils;
    friend class SceneBuilder;
    friend class CDetail;
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
    BYTE			m_Visible;
    BYTE			m_Locked;
public:
	st_MeshOptions	m_Ops;
    DWORD 			m_LoadState;

protected:
	Fbox			m_Box;
    FvectorVec		m_Points;	// |
    SVertVec		m_SVertices;// |
    AdjVec			m_Adjs;     // + some array size!!!
    SurfFaces		m_SurfFaces;
    FvectorVec		m_PNormals;	// |*3
    FvectorVec		m_FNormals;	// |
    FaceVec			m_Faces;    // + some array size!!!
    VMapVec			m_VMaps;
    VMRefsVec		m_VMRefs;

	void 			FillRenderBuffer		(INTVec& face_lst, int start_face, int num_face, const CSurface* surf, LPBYTE& data);

	void 			RecurseTri				(int id);

    void			RecomputeBBox			();

	// mesh optimize routine
	bool 			OptimizeFace			(st_Face& face);
	void 			Optimize				(BOOL NoOpt);
	bool 			UpdateAdjacency			();
public:
	                CEditableMesh				(CEditableObject* parent){m_Parent=parent;Construct();}
	                CEditableMesh				(CEditableMesh* source,CEditableObject* parent){m_Parent=parent;Construct();CloneFrom(source);}
	virtual         ~CEditableMesh				();
	void			Construct				();
    void			Clear					();

	IC char*		GetName					(){return m_Name;}
	void            GetBox					(Fbox& box){box.set(m_Box);}
	CSurface*		GetSurfaceByFaceID		(int fid);
	CSurface*		GetFaceTC				(int fid, const Fvector2* tc[3]);
	CSurface*		GetFacePT				(int fid, const Fvector* pt[3]);
	IC BOOL 		Visible					(){return m_Visible; }
	IC void 		Show					(bool bVisible){m_Visible=bVisible;}

    // mesh modify routine
	void            CloneFrom				(CEditableMesh *source);
	void            Transform				(const Fmatrix& parent);

    IC FaceVec&		GetFaces				(){ return m_Faces;}
    IC FvectorVec&	GetPoints				(){ return m_Points;}
    IC FvectorVec&	GetFNormals				(){ return m_FNormals;}

    // pick routine
	bool            RayPick					(float& dist, const Fvector& start, const Fvector& dir, const Fmatrix& inv_parent, SRayPickInfo* pinf = NULL);
#ifdef _LEVEL_EDITOR
    bool 			BoxPick					(const Fbox& box, const Fmatrix& parent, SBoxPickInfoVec& pinf);
	bool            FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    void            FrustumPickFaces		(const CFrustum& frustum, const Fmatrix& parent, DWORDVec& fl);
    bool			CHullPickMesh			(PlaneVec& pl, const Fmatrix& parent);
	void 			GetTiesFaces			(int start_id, DWORDVec& fl, float fSoftAngle, bool bRecursive);
#endif

    // render routine
	void 			Render					(const Fmatrix& parent, CSurface* S);
	void            RenderList				(const Fmatrix& parent, DWORD color, bool bEdge, DWORDVec& fl);
	void 			RenderEdge				(const Fmatrix& parent, DWORD color);
	void 			RenderSelection			(const Fmatrix& parent, DWORD color);

    // statistics methods
    int 			GetFaceCount			(bool bMatch2Sided=true);
	int 			GetVertexCount			(){return m_Points.size();}
    int 			GetSurfFaceCount		(CSurface* surf, bool bMatch2Sided=true);

    // IO - routine
    void			SaveMesh				(CFS_Base&);
	bool 			LoadMesh				(CStream&);

    // debug
    void			DumpAdjacency			();

#ifdef _MAX_EXPORT
	// convert
    void			FlipFaces				();
	TriObject*		ExtractTriObject		(INode *node, int &deleteIt);
	bool			Convert					(INode *node);
	bool			Convert					(CExporter* exporter);
#endif
	int				FindVMapByName			(VMapVec& vmaps, const char* name, EVMType t, BOOL polymap);
	void			RebuildVMaps			();

    // device dependent routine
	void 			OnDeviceCreate 			();
	void 			OnDeviceDestroy			();
};
//----------------------------------------------------
#endif /*_INCDEF_EditableMesh_H_*/

