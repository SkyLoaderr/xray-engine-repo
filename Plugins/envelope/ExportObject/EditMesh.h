//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef _INCDEF_StaticCEditMesh_H_
#define _INCDEF_StaticCEditMesh_H_

//----------------------------------------------------
// refs
struct st_Surface;
struct SPickInfo;
struct FSChunkDef;

// refs
class CFS_Base;
class CStream;

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
	int		 		m_Collision;
    int				m_Reserved0;
    st_MeshOptions	(){m_Collision=1;}
};
#pragma pack( pop )

DEFINE_VECTOR(INTVec,AdjVec,AdjIt);
DEFINE_VECTOR(st_VMap,VMapVec,VMapIt);
DEFINE_VECTOR(st_Face,FaceVec,FaceIt);
DEFINE_MAP(st_Surface*,INTVec,SurfFaces,SurfFacesPairIt);

//refs
struct st_RenderBuffer;
struct st_Surface;
class INode;

class CEditMesh {
	friend class CEditObject;
    friend class CSectorItem;
    friend class CSector;
    friend class CPortalUtils;
    friend class SceneBuilder;
    
	char m_Name[MAX_OBJ_NAME];

    CEditObject*	m_Parent;

    // internal variables
    int				m_Visible;
    int				m_Locked;
public:
	st_MeshOptions	m_Ops;
    DWORD 			m_LoadState;
protected:
	Fbox			m_Box;
    FvectorVec		m_Points;	// |
    AdjVec			m_Adjs;     // + some array size!!!
    SurfFaces		m_SurfFaces;
    FvectorVec		m_PNormals;	// |*3
    FvectorVec		m_FNormals;	// |
    FaceVec			m_Faces;    // + some array size!!!
    VMapVec			m_VMaps;
    VMRefsVec		m_VMRefs;

    void 			PrepareMesh				();

	void 			RecurseTri				(int id);

	// mesh optimize routine
	bool 			OptimizeFace			(int face_id, bool bTestNormal);
	bool 			UpdateAdjacency			();
public:
	                CEditMesh				(CEditObject* parent){m_Parent=parent;Construct();}
//	                CEditMesh				(CEditMesh* source,CEditObject* parent){m_Parent=parent;Construct();CloneFrom(source);}
	virtual         ~CEditMesh				();
	void			Construct				(){m_Box.set(0,0,0,0,0,0);m_Visible=1;m_Locked=0;m_Name[0]=0;}
    void			Clear					();

    // mesh modify routine
//	void            CloneFrom				(CEditMesh *source);
//	void            Transform				(Fmatrix& parent);


	IC char*		GetName					(){return m_Name;}
	void            GetBox					(Fbox& box){box.set(m_Box);}

    // statistics methods
    int 			GetFaceCount			(){return m_Faces.size();}
	int 			GetVertexCount			(){return m_Points.size();}

    // IO - routine
    void			SaveMesh				(CFS_Base&);
	bool 			LoadMesh				(CStream&);

	// convert
	void 			Optimize				(BOOL NoOpt);
    void			RecomputeBBox			();
	void			FlipFaces				();

	st_VMap*		FindVMapByName			(const char* name, EVMType t);
};
//----------------------------------------------------
#endif /*_INCDEF_StaticMesh_H_*/

