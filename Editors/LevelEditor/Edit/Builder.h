//----------------------------------------------------
// file: Builder.h
//----------------------------------------------------

#ifndef _INCDEF_Builder_H_
#define _INCDEF_Builder_H_

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
//----------------------------------------------------
// some types
typedef Fvector b_vnormal;

struct sb_light_control						// controller or "layer", 30fps
{
	string64			name;				// empty for base layer
    DWORDVec			data;
};
class SceneBuilder{
	int						l_vertices_cnt, l_vertices_it;
	int						l_faces_cnt, l_faces_it;
    b_vertex*		        l_vertices;
    b_face*  		        l_faces;
    SVertVec				l_svertices;
    vector<b_lod>			l_lods;
    vector<sb_light_control>l_light_control;
    vector<b_light_static>	l_light_static;
    vector<b_light_dynamic>	l_light_dynamic;
    vector<b_texture>       l_textures;
    vector<b_shader>        l_shaders;
    vector<b_shader>        l_shaders_xrlc;
    vector<b_material>      l_materials;
    vector<b_vnormal>       l_vnormals;
    vector<b_glow>          l_glows;
    vector<b_portal>        l_portals;
    vector<Flight>          l_light_keys;

    void    GetBBox         (DWORD st_fid, DWORD cnt, Fbox& box);

    void    BuildGlow       (CGlow* e);
    void    BuildPortal   	(b_portal* b, CPortal* e);
    BOOL    BuildMesh       (const Fmatrix& parent, CEditableObject* object, CEditableMesh* mesh, int sector_num, int lod_id);
    BOOL    BuildObject     (CSceneObject* obj);

    void    ResetStructures ();

    int		BuildLightControl(CLight* e);
    void 	BuildHemiLights	();
	void	AppendLight		();    
    BOOL 	BuildSun		(b_light* b, const Flags32& usage, svector<WORD,16>* sectors);
    BOOL 	BuildPointLight	(b_light* b, const Flags32& usage, svector<WORD,16>* sectors, FvectorVec* soft_points);
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
	int 	BuildMaterial	(CSurface* surf, int sector_num, int lod_id);

    void 	SaveBuild		();

	void 	PF_ProcessOne	(_finddata_t& F, const char* path);
	void 	PF_Recurse		(const char* path);
protected:
	friend void SaveBuild	();
    friend class TfrmBuildProgress;

	Fbox 	m_LevelBox;

	AStringVec m_TexNames;
public:
	FSPath 	m_LevelPath;
protected:
	bool 	EvictResource			();
	bool 	PrepareFolders          ();
    bool 	PreparePath				();

	bool 	GetBounding            	();

    BOOL	ParseLTX				(CInifile* pIni, ObjectList& lst, LPCSTR prefix=0);
	BOOL 	BuildLTX                ();
    BOOL	ParseGAME				(CFS_Base& game, CFS_Base& spawn, ObjectList& lst, LPCSTR prefix=0);
    BOOL 	BuildGame				();

	bool 	WriteTextures           ();
	void 	AddUniqueTexName        (const char *name);

    bool 	BuildSkyModel			();
    bool 	BuildHOMModel			();
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
};

extern SceneBuilder Builder;

//----------------------------------------------------
#endif /*_INCDEF_Builder_H_*/

