//----------------------------------------------------
// file: Gizmo.h
//----------------------------------------------------

#ifndef _INCDEF_Texturizer_H_
#define _INCDEF_Texturizer_H_

#include "gizmo.h"
//refs
class SObject2;
class SObject2Mesh;
class CTextureLayer;

//----------------------------------------------------

class CTexturizer{
    friend class CGizmo;
    bool            m_bActive;
    CGizmo          m_Gizmo;
protected:
    SObject2*       m_EditObject;
    SObject2Mesh*   m_EditMesh;
    CTextureLayer*  m_EditLayer;
public:
	                CTexturizer     ();
	virtual         ~CTexturizer    ();

    bool            Valid           (){return m_bActive&&!!m_EditObject&&!!m_EditMesh;}
    bool            ValidLayer      (){return m_bActive&&!!m_EditObject&&!!m_EditMesh&&!!m_EditLayer;}
    bool            Active          (){return m_bActive;}
    void            Reset           ();
    void            Render          ();

    void            Activate        ();
    void            Deactivate      ();

    void            SetEditObject   (SObject2* obj);
    void            SetEditMesh     (SObject2Mesh* m);
    void            SetEditLayer    (CTextureLayer* l);

    const char*     GetObjectName   ();
    const char*     GetMeshName     ();
    const char*     GetLayerName    ();

    const SObject2* GetObject       (){return m_EditObject;}
    SObject2Mesh*   GetMesh         (){return m_EditMesh;}
    CTextureLayer*  GetLayer        (){return m_EditLayer;}

    void            AddNewLayer     ();
    void            RemoveEditLayer ();
    void            Edit            ();

    void            Fit             ();

    void            UpdateSelection ();
};

extern CTexturizer Texturizer;

#endif /*_INCDEF_Texturizer_H_*/

