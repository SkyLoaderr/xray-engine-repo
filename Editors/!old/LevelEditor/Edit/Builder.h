//----------------------------------------------------
// file: Builder.h
//----------------------------------------------------

#ifndef BuilderH
#define BuilderH

#include "communicate.h"
#include "EditMesh.h"
//----------------------------------------------------

// refs
class CSceneObject;
class CEditableMesh;
class EScene;
class CLight;
class CGlow;
class CPortal;
struct st_SPData;
class CSurface;
struct st_DPSurface;
class CSceneStat;
//----------------------------------------------------
// some types
typedef Fvector b_vnormal;

struct sb_light_control						// controller or "layer", 30fps
{
	string64			name;				// empty for base layer
    U32Vec				data;
};

struct e_b_lod{
	b_lod				lod;
    ref_str				tex_name;
};

class SceneBuilder{
	int							l_vert_cnt, l_vert_it;
	int							l_face_cnt, l_face_it;
    b_vertex*		        	l_verts;
    b_face*  		        	l_faces;
    SVertVec					l_svertices;
    xr_vector<b_mu_model>		l_mu_models;
    xr_vector<b_mu_reference>	l_mu_refs;
    xr_vector<e_b_lod>			l_lods;
    xr_vector<sb_light_control>	l_light_control;
    xr_vector<b_light_static>	l_light_static;
    xr_vector<b_light_dynamic>	l_light_dynamic;
    xr_vector<b_texture>       	l_textures;
    xr_vector<b_shader>        	l_shaders;
    xr_vector<b_shader>        	l_shaders_xrlc;
    xr_vector<b_material>      	l_materials;
    xr_vector<b_vnormal>       	l_vnormals;
    xr_vector<b_glow>          	l_glows;
    xr_vector<b_portal>        	l_portals;
    xr_vector<Flight>          	l_light_keys;

    CSceneStat*					l_scene_stat;

    void    GetBBox         (u32 st_fid, u32 cnt, Fbox& box);

    BOOL    BuildGlow       (CGlow* e);
    void    BuildPortal   	(b_portal* b, CPortal* e);
    BOOL    BuildMesh       (const Fmatrix& parent, CEditableObject* object, CEditableMesh* mesh, int sector_num,
    						b_vertex* verts, int& vert_cnt, int& vert_it,
                            b_face* faces, int& face_cnt, int& face_it);
    BOOL    BuildObject     (CSceneObject* obj);
    BOOL    BuildMUObject   (CSceneObject* obj);

    void    Clear 			();

    int		BuildLightControl(LPCSTR name);
    void 	BuildHemiLights	(u8 quality, LPCSTR lcontrol);
	void	AppendLight		();
    BOOL 	BuildSun		(u8 quality, Fvector2 dir);
    BOOL 	BuildPointLight	(b_light* b, const Flags32& usage, svector<WORD,16>* sectors, FvectorVec* soft_points, const Fmatrix* soft_transform=0);
    BOOL    BuildLight		(CLight* e);

    int     FindInLODs   	(b_lod* s);
    int		BuildObjectLOD  (const Fmatrix& parent, CEditableObject* e, int sector_num);

    int     FindInShaders   (b_shader* s);
    int     BuildShader     (LPCSTR s);

	int 	FindInShadersXRLC(b_shader* s);
	int 	BuildShaderXRLC	(const char * s);

	int 	FindInTextures	(LPCSTR name);
    int     BuildTexture    (LPCSTR name);

    int     FindInMaterials (b_material* m);
	int 	BuildMaterial	(CSurface* surf, int sector_num);

    void 	SaveBuild		();
protected:
	friend void SaveBuild	();
    friend class TfrmBuildProgress;

	Fbox 	m_LevelBox;
public:
	std::string m_LevelPath;
    std::string	MakeLevelPath		(LPCSTR nm){return m_LevelPath+nm;}
protected:
	bool 	EvictResource			();
	bool 	PrepareFolders          ();
    bool 	PreparePath				();

	bool 	GetBounding            	();

    BOOL	ParseLTX				(CInifile* pIni, ObjectList& lst, LPCSTR prefix=0);
	BOOL 	BuildLTX                ();
    BOOL	ParseGAME				(IWriter& game, IWriter& spawn, ObjectList& lst, LPCSTR prefix=0);
    BOOL 	BuildGame				();

    BOOL	BuildSceneStat			();
    bool 	BuildHOMModel			();
    bool	BuildAIMap				();
    bool	BuildWallmarks			();
    BOOL	ParseStaticObjects		(ObjectList& lst, LPCSTR prefix=0);
    BOOL 	CompileStatic		   	();

	int 	m_iDefaultSectorNum;
	bool 	RenumerateSectors		();
	int 	CalculateSector			(const Fvector& P, float R);
public:
			SceneBuilder            ();
	virtual ~SceneBuilder           ();

	BOOL	Compile            		();
	BOOL 	MakeGame				();
    BOOL 	MakeDetails				();
    BOOL 	MakeHOM					();
    BOOL	MakeAIMap				();
    BOOL	MakeWallmarks			();
};

extern SceneBuilder Builder;

//----------------------------------------------------
#endif /*_INCDEF_Builder_H_*/

