//----------------------------------------------------
// file: Gizmo.h
//----------------------------------------------------

#ifndef _INCDEF_Gizmo_H_
#define _INCDEF_Gizmo_H_

#include "SceneObject.h"
#include "StaticMesh.h"

class CGizmo{
    float           m_UTile, m_VTile;
    Fvector         m_Normal;
    Fvector         m_Direction;
    Fvector         m_Center;
    Fvector         m_Size;

    vector<FLvertex>m_RenderBuffer;
protected:
    CTextureLayer*  m_SourceLayer;
    CTextureLayer*  m_EditLayer;
    DWORDList       m_UVFaceIDs;
protected:
    void            BuildPlane      ();
    void            UpdateUV        ();
public:
	                CGizmo          ();
	virtual         ~CGizmo         ();

	void            Render          ();

    void            Fit             (Fvector);
    void            NormalAlign     (Fvector& normal);

	void            Move            (Fvector& amount);
	void            Rotate          (Fvector& axis, float angle);
    void            Scale           (Fvector& amount);
};

#endif /*_INCDEF_Gizmo_H_*/

