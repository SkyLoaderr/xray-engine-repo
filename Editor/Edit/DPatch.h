//----------------------------------------------------
// file: DPatch.h
//----------------------------------------------------

#ifndef _INCDEF_DPatch_H_
#define _INCDEF_DPatch_H_

#include "SceneObject.h"
#include "tlsprite.h"
class Shader;

struct st_SPData{
    BYTE        	m_Selected;
    float       	m_Range;
    Fvector     	m_Position;
    Fvector     	m_Normal;
    st_SPData      	():m_Selected(1),m_Range(0){;}
};

struct st_DPSurface{
	AStringVec		m_Textures;
    Shader*			m_Shader;
    ~st_DPSurface	();
};

DEFINE_VECTOR		(st_SPData,PatchVec,PatchIt);
DEFINE_MAP			(st_DPSurface*,PatchVec,PatchMap,PatchMapIt);

class CDPatchSystem{
    CTLSprite       m_RenderSprite;
    void            AddDPatch    	(PatchVec& lst, float r, Fvector& p, Fvector& n);
    bool            LoadPatches    	(CStream&);
public:
    PatchMap        m_Patches;
public:
	                CDPatchSystem	();
	virtual         ~CDPatchSystem	();

    st_DPSurface*	CreateSurface	(AStringVec& tex_names, const char* sh_name);
	int             Select			(bool flag);
    void            AddDPatch    	(st_DPSurface* surf, float r, Fvector& p, Fvector& n);
    void            MoveDPatches  	(Fvector& amount);
	int             InvertSelection	();
	int             SelectionCount 	(bool testflag);
	int             RemoveSelection	();
    int 			ObjCount	   	();
    int 			Lock	   		(bool bFlag){return 0;}

    void            Clear       	();

	void            Render      	();
	bool            RayPickSelect	(float& distance, Fvector& start, Fvector& direction);
	void            FrustumSelect	(bool flag);
	void 			Save			(CFS_Base&);
	bool 			Load			(CStream&);
	bool            GetBox      	(Fbox& box);
};

#endif /*_INCDEF_DPatch_H_*/

