//----------------------------------------------------
// file: Builder.h
//----------------------------------------------------

#ifndef _INCDEF_Builder_H_
#define _INCDEF_Builder_H_

#include "communicate.h"
#include "FileSystem.h"
#include "EditMesh.h"
//----------------------------------------------------

// refs
class CSceneObject;
class CEditableMesh;
class EScene;
class CLight;
class ETextureCore;
class CGlow;
class COccluder;
class CDPatch;
class CPortal;
struct st_SPData;
struct st_Surface;
struct st_DPSurface;
//----------------------------------------------------
// some types
// some types
class SSTR{
public:
	char filename[MAX_PATH];
	SSTR(){filename[0]=0;};
	SSTR(const SSTR& source){strcpy(filename,source.filename);};
	SSTR(const char *source){strcpy(filename,source);};
	~SSTR(){}; };

typedef std::vector<SSTR> SSTRLIST;
typedef Fvector b_vnormal;

class SceneBuilder{
	int						l_vertices_cnt, l_vertices_it;
	int						l_faces_cnt, l_faces_it;
    b_vertex*		        l_vertices;
    b_face*  		        l_faces;
    SVertVec				l_svertices;
    vector<b_texture>       l_textures;
    vector<b_shader>        l_shaders;
    vector<b_material>      l_materials;
    vector<b_vnormal>       l_vnormals;
    vector<b_glow>          l_glows;
    vector<b_particle>      l_dpatches;
    vector<b_occluder>      l_occluders;
    vector<b_portal>        l_portals;
    vector<Flight>          l_light_keys;

    void    GetBBox         (DWORD st_fid, DWORD cnt, Fbox& box);

    void    BuildGlow       (b_glow* b, CGlow* e);
    void    BuildDPatch   	(b_particle* b, st_SPData* e, st_DPSurface* surf);
    void    BuildLight      (b_light* b, CLight* e);
    void    BuildOccluder   (b_occluder* b, COccluder* e);
    void    BuildPortal   	(b_portal* b, CPortal* e);
    void    BuildMesh       (const Fmatrix& parent, CEditableObject* object, CEditableMesh* mesh, int sector_num);
    void    BuildObject     (CSceneObject* obj);
    void    BuildObjectDynamic(CEditableObject* obj);

    void    ResetStructures ();

    int     FindInShaders   (b_shader* s);
    int     BuildShader     (const char* s);

	int 	FindInTextures	(const char* name);
    int     BuildTexture    (ETextureCore* e_tex, const char* name);

    int     FindInMaterials (b_material* m);
	int 	BuildMaterial	(CEditableMesh* m, st_Surface* surf, int sector_num );
protected:
	friend void SaveBuild(b_transfer *info);
    friend class TfrmBuildProgress;
    friend struct st_SPLIT;

	bool m_InProgress;

	FSPath m_LevelPath;
	Fvector m_LevelShift;
	Fbox m_LevelBox;

	SSTRLIST m_TexNames;

    bool bNeedAbort;
protected:
	bool LightenObjects				();
	bool PrepareFolders             ();
    bool PreparePath				();
    bool UngroupAndUnlockObjects	();

	bool GetShift                   ();
	bool ShiftLevel                 ();

	bool BuildLTX                   ();

	bool WriteTextures              ();
	void AddUniqueTexName           (const char *name);
    void AddUniqueTexName			(CSceneObject* O);

    // OGF
	bool BuildObjectOGF             (CFS_Base& F, CEditableObject *obj, const char* base_name, FSPath& path);
	bool BuildSingleOGF             (CFS_Base& F, DWORD fid_start, DWORD cnt, int mid);
    bool BuildSkyModel				();

    void AssembleSkeletonObject		(CEditableObject* obj);
    void AssembleSkeletonMesh		(CEditableMesh* mesh);

    bool BuildObjectVCF             (CFS_Base& F, CEditableObject* obj);

    bool RemoteStaticBuild          ();

	int m_iDefaultSectorNum;
	bool RenumerateSectors			();
	int CalculateSector				(const Fvector& P, float R);
public:
	virtual DWORD Thread            ();
	DWORD ThreadMakeLTX             ();
public:
	SceneBuilder                    ();
	virtual ~SceneBuilder           ();

	virtual bool Execute            ();
	bool MakeLTX  			        ();
    bool MakeDetails				(bool bOkMessage);

    bool    NeedAbort               (){Application->ProcessMessages(); return bNeedAbort;}
    void 	BreakBuild				(){bNeedAbort = true;}

	__inline bool InProgress        (){	return m_InProgress; }

    bool SaveObjectVCF              (const char* name, CEditableObject* obj);
    bool SaveObjectOGF              (const char* name, CEditableObject* obj);
    bool SaveObjectSkeletonOGF      (const char* name, CEditableObject* obj);
    bool SaveObjectSkeletonLTX      (const char* name, CEditableObject* obj);
};

extern SceneBuilder* Builder;

//----------------------------------------------------
#endif /*_INCDEF_Builder_H_*/

