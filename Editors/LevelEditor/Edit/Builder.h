//----------------------------------------------------
// file: Builder.h
//----------------------------------------------------

#ifndef _INCDEF_Builder_H_
#define _INCDEF_Builder_H_

#include "communicate.h"
#include "EditMesh.h"
#include "CustomObject.h"
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

class SceneBuilder{
	int						l_vertices_cnt, l_vertices_it;
	int						l_faces_cnt, l_faces_it;
    b_vertex*		        l_vertices;
    b_face*  		        l_faces;
    SVertVec				l_svertices;
    vector<b_light>			l_lights;
    vector<b_texture>       l_textures;
    vector<b_shader>        l_shaders;
    vector<b_shader>        l_shaders_xrlc;
    vector<b_material>      l_materials;
    vector<b_vnormal>       l_vnormals;
    vector<b_glow>          l_glows;
    vector<b_portal>        l_portals;
    vector<Flight>          l_light_keys;

    void    GetBBox         (DWORD st_fid, DWORD cnt, Fbox& box);

    void    BuildGlow       (b_glow* b, CGlow* e);
    void    BuildLight      (b_light* b, CLight* e);
    void    BuildPortal   	(b_portal* b, CPortal* e);
    BOOL    BuildMesh       (const Fmatrix& parent, CEditableObject* object, CEditableMesh* mesh, int sector_num);
    BOOL    BuildObject     (CSceneObject* obj);

    void    ResetStructures ();

    int     FindInShaders   (b_shader* s);
    int     BuildShader     (LPCSTR s);

	int 	FindInShadersXRLC(b_shader* s);
	int 	BuildShaderXRLC	(const char * s);

	int 	FindInTextures	(LPCSTR name);
    int     BuildTexture    (LPCSTR name);

    int     FindInMaterials (b_material* m);
	int 	BuildMaterial	(CEditableMesh* m, CSurface* surf, int sector_num );
protected:
	friend void SaveBuild(b_transfer *info);
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

	bool 	BuildLTX                ();
    bool 	BuildGame				();

	bool 	WriteTextures           ();
	void 	AddUniqueTexName        (const char *name);

    bool 	BuildSkyModel			();
    bool 	BuildHOMModel			();
    BOOL	ParseStaticObjects		(ObjectList& lst);
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

