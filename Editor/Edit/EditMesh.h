//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef _INCDEF_StaticCEditMesh_H_
#define _INCDEF_StaticCEditMesh_H_

//----------------------------------------------------
#include "cl_rapid.h"
#include "sceneobject.h"

// refs
struct st_Surface;
struct SRayPickInfo;
struct CFrustum;
struct FSChunkDef;

#pragma pack( push,1 )
enum EVMType{
	vmtUV=0,
    vmtWeight,
    vmt_force_byte = 0xff
};

struct st_VMapPt{
	int				vmap_index;	// ссылка на мапу
	int				index;		// индекс в мапе на uv
};
// uv's
class st_VMap{
    FloatVec    	vm;			// u,v - координаты или weight
public:
	char			name[MAX_OBJ_NAME];	// vertex uv map name
    BYTE			dim;
    EVMType			type;
public:
				    st_VMap		()						{ name[0]=0; type=vmtUV; dim=2; }
				    st_VMap		(EVMType t)				{ name[0]=0; type=t; if (t==vmtUV) dim=2; else dim=1;}
    IC Fvector2&	getUV		(int idx)				{VERIFY(type==vmtUV);		return (Fvector2&)vm[idx*dim];}
    IC float&		getW		(int idx)				{VERIFY(type==vmtWeight);	return vm[idx];}
    IC FloatVec&	getvm		()						{return vm;}
    IC float*		getdata		()						{return vm.begin();}
    IC float*		getdata		(int start)				{return vm.begin()+start*dim;}
    IC int			datasize	()						{return vm.size()*sizeof(float);}
    IC int			size		()						{return vm.size()/dim;}
    IC void			resize		(int cnt)				{vm.resize(cnt*dim);}
	IC void			appendUV	(float u, float v)		{vm.push_back(u);vm.push_back(v);}
	IC void			appendUV	(Fvector2& uv)			{appendUV(uv.x,uv.y);}
	IC void			appendW		(float w)				{vm.push_back(w);}
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
DEFINE_VECTOR		(st_VMap,VMapVec,VMapIt);
DEFINE_VECTOR		(st_Face,FaceVec,FaceIt);
DEFINE_MAP			(st_Surface*,INTVec,SurfFaces,SurfFacesPairIt);
DEFINE_VECTOR		(st_SVert,SVertVec,SVertIt);

//refs
struct st_RenderBuffer;
struct st_Surface;

struct st_RenderBuffer{
    DWORD			dwStartVertex;
    DWORD			dwNumVertex;
	IDirect3DVertexBuffer7* buffer;
    void*			rv;
    st_RenderBuffer	(DWORD sv, DWORD nv):dwStartVertex(sv),dwNumVertex(nv),buffer(0),rv(0){;}
};
DEFINE_VECTOR(st_RenderBuffer,RBVector,RBVecIt);
DEFINE_MAP(st_Surface*,RBVector,RBMap,RBMapPairIt);

#define EMESH_LS_CF_MODEL	0x0001
#define EMESH_LS_FNORMALS 	0x0002
#define EMESH_LS_PNORMALS 	0x0004
#define EMESH_LS_SVERTICES 	0x0008

class CEditMesh {
	friend class CEditObject;
    friend class CSectorItem;
    friend class CSector;
    friend class CPortalUtils;
    friend class SceneBuilder;
    friend class CDetail;

	char m_Name[MAX_OBJ_NAME];

    CEditObject*	m_Parent;

    RAPID::Model*	m_CFModel;

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

    RBMap			m_RenderBuffers;
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

    void			UpdateRenderBuffers		(LPDIRECT3D7 pD3D,DWORD dwCaps);
	void 			FillRenderBuffer		(INTVec& face_lst, int start_face, int num_face, const st_Surface* surf, LPVOID& data);
    void 			ClearRenderBuffers		();

	void 			RecurseTri				(int id);

    void			RecomputeBBox			();

	// mesh optimize routine
	bool 			OptimizeFace			(st_Face& face);
	void 			Optimize				();
	bool 			UpdateAdjacency			();
public:
	                CEditMesh				(CEditObject* parent){m_Parent=parent;Construct();}
	                CEditMesh				(CEditMesh* source,CEditObject* parent){m_Parent=parent;Construct();CloneFrom(source);}
	virtual         ~CEditMesh				();
	void			Construct				();
    void			Clear					();

	IC char*		GetName					(){return m_Name;}
	void            GetBox					(Fbox& box){box.set(m_Box);}
	st_Surface*		GetSurfaceByFaceID		(int fid);
	st_Surface*		GetFaceTC				(int fid, const Fvector2* tc[3]);
	st_Surface*		GetFacePT				(int fid, const Fvector* pt[3]);
	IC BOOL 		Visible					(){return m_Visible; }
	IC void 		Show					(bool bVisible){m_Visible=bVisible;}

    // mesh modify routine
	void            CloneFrom				(CEditMesh *source);
	void            Transform				(Fmatrix& parent);

    // pick routine
	bool            RayPick					(float& dist, Fvector& start, Fvector& dir, Fmatrix& parent, SRayPickInfo* pinf = NULL);
    void 			BoxPick					(const Fbox& box, Fmatrix& parent, SBoxPickInfoVec& pinf);
	bool            FrustumPick				(const CFrustum& frustum, const Fmatrix& parent);
    void            FrustumPickFaces		(const CFrustum& frustum, Fmatrix& parent, DWORDVec& fl);
    void			CHullPickFaces			(PlaneVec& pl, Fmatrix& parent, DWORDVec& fl);
	void 			GetTiesFaces			(int start_id, DWORDVec& fl, float fSoftAngle, bool bRecursive);

    // render routine
	void 			Render					(const Fmatrix& parent, st_Surface* S);
	void            RenderList				(const Fmatrix& parent, DWORD color, bool bEdge, DWORDVec& fl);
	void 			RenderEdge				(Fmatrix& parent, DWORD color);
	void 			RenderSelection			(Fmatrix& parent, DWORD color);

    // statistics methods
    int 			GetFaceCount			(bool bMatch2Sided=true);
	int 			GetVertexCount			(){return m_Points.size();}
    int 			GetSurfFaceCount		(st_Surface* surf, bool bMatch2Sided=true);

    // IO - routine
    void			SaveMesh				(CFS_Base&);
	bool 			LoadMesh				(CStream&);

    // debug
    void			DumpAdjacency			();
};
//----------------------------------------------------
#endif /*_INCDEF_StaticMesh_H_*/

