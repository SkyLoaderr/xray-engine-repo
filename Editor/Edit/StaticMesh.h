//----------------------------------------------------
// file: StaticMesh.h
//----------------------------------------------------
#ifndef _INCDEF_StaticMesh_H_
#define _INCDEF_StaticMesh_H_

//----------------------------------------------------

// refs
struct SPickInfo;
struct FSChunkDef;
class ETexture;

#define SCRIPT_MAX_SIZE 255

typedef unsigned __int64	CLASS_ID;
#define MK_CLSID(a,b,c,d,e,f,g,h) \
	CLASS_ID(	((CLASS_ID(a)<<24)|(CLASS_ID(b)<<16)|(CLASS_ID(c)<<8)|(CLASS_ID(d)))<<32 | \
				((CLASS_ID(e)<<24)|(CLASS_ID(f)<<16)|(CLASS_ID(g)<<8)|(CLASS_ID(h))) )

#pragma pack(push,1)
enum EShaderOP{
    sopBlend=0,
    sopMul,
    sopAdd,
    sop_forcedword = (-1)
};

struct SPoint {
	Fvector m_Point;
	Fvector m_Normal;
};

struct SFace {
	union{
		WORD p[3];
		struct { WORD p0,p1,p2; };
	};
};

struct SUV{
    float tu, tv;
};

struct SUV_tri{
    SUV uv[3];
    SUV_tri(){
        uv[0].tu=1.f; uv[0].tv=0.f;
        uv[1].tu=0.f; uv[1].tv=1.f;
        uv[2].tu=1.f; uv[2].tv=1.f;
    }
    SUV_tri(SUV t[3]){
        CopyMemory(uv,t,sizeof(SUV)*3);
    }
};

struct STFace :public SFace{
    int GFaceID;
};
#pragma pack(pop)

typedef vector<DWORD>  DWORDList;
typedef DWORDList::iterator DWORDIt;

typedef vector<SUV_tri> UVFaceList;
typedef UVFaceList::iterator UVFaceIt;

#define MAX_LAYER_NAME 32

class CTextureLayer{
    friend class Mesh;
    friend class TfrmPropertiesSubObject;
    friend class TfrmPropertiesObject;
    friend class TfraSelectTexturing;

	Mesh*			    m_Parent;
    char                m_LayerName	[MAX_LAYER_NAME];
    char				m_Script	[SCRIPT_MAX_SIZE];
	ETexture*           m_Texture;  // texture
    CLASS_ID            m_CLS;      // texture class
    Fmaterial           m_Mat;      // d3d material
    EShaderOP           m_SOP;		// shader OP

// layer texture geometry
    UVFaceList          m_UVMaps;
    DWORDList           m_GFaceIDs;

// layer renderer
    vector<Fvertex>     m_RenderBuffer;
public:
                        CTextureLayer   (Mesh* parent);
    virtual             ~CTextureLayer  ();
	__inline const char*GetName         (){return m_LayerName; }
    bool                AddFace         (DWORD face_id, SUV_tri* uv=NULL);
    bool                DelFace         (DWORD face_id);
    bool                ContainsFace    (DWORD face_id);
	void		        Copy			(CTextureLayer* source, DWORD face_offs=0);
    bool                Load            (FSChunkDef *chunk);
    void                UpdateRenderBuffer();
};

typedef vector<CTextureLayer*> TLayerList;
typedef TLayerList::iterator TLayerIt;

typedef vector<SPoint> PointList;
typedef PointList::iterator PointIt;

typedef vector<SFace> FaceList;
typedef FaceList::iterator FaceIt;

class RAPID_model;

class Mesh {
protected:
	friend class SceneBuilder;
    friend class TfrmPropertiesObject;
    friend class TfrmPropertiesSubObject;
    friend class CTextureLayer;

    PointList       m_Vertices;
    FaceList        m_GFaces;
    TLayerList      m_Layers;

    RAPID_model*    cf_model;
    void            GenerateCFModel();
    void            Clear();
public:
	                Mesh();
	                Mesh(Mesh* source);
	virtual         ~Mesh();

	void            Render( Fmatrix& parent, bool use_material=true );
	void            RenderEdge( Fmatrix& parent, FPcolor& color, CTextureLayer* layer=0 );
	void            CloneFrom( Mesh *source );
	void            Transform( Fmatrix& parent );
	void            GetBox( IAABox& box );

	bool            Pick( float *distance,	Fvector& start,	Fvector& direction,
                          Fmatrix& parent, SPickInfo* pinf = NULL );
	bool            BoxPick( ICullPlane *planes,Fmatrix& parent );

	bool            Load( char *filename, bool bRealLoad=false );
	bool            LoadMesh( FSChunkDef *chunk );
	void            SaveMesh( int handle );

    int             GetFaceCount() { return m_GFaces.size();}
    int             GetVertexCount() { return m_Vertices.size();}

    CTextureLayer*  AddTextureLayer(ETexture* t=NULL);
    void            RemoveTextureLayer(CTextureLayer** l);
    bool            ContainsLayer(CTextureLayer* l);
    int             GetLayerCount() { return m_Layers.size();}
    CTextureLayer*  GetLayer(DWORD i){VERIFY((i>=0)&&(i<m_Layers.size())); return m_Layers[i];}

	void		    Append( Mesh *source );

    void            Update();
};

typedef list<Mesh*> MeshList;
typedef list<Mesh*>::iterator MeshIt;

extern void CLSID2TEXT(CLASS_ID id, char *text);
extern CLASS_ID TEXT2CLSID(char *text);
//----------------------------------------------------
#endif /*_INCDEF_StaticMesh_H_*/

