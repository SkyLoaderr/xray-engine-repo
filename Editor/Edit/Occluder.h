//----------------------------------------------------
// file: Occluder.h
//----------------------------------------------------

#ifndef _INCDEF_Occluder_H_
#define _INCDEF_Occluder_H_

#include "SceneObject.h"

#define MIN_OCCLUDER_POINTS 3
#define MAX_OCCLUDER_POINTS 6

class CFrustum;


class COccluder: public SceneObject {
    void CreatePlane();
	struct S3DPoints{
		DWORD		dwFrame;
	    FvectorVec 	m_Points;
    }m_3DPoints;
public:
    Fvector         m_vRotate;
    Fvector         m_vCenter;
    Fvector2        m_vPlaneSize;
	Fvector2Vec     m_Points;
    INTVec			m_SelPoints;
public:
	                COccluder   ();
	                COccluder   (char *name);
	virtual         ~COccluder  ();

	virtual void    Render      (Fmatrix& parent, ERenderPriority flag);

	virtual bool    RayPick		(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf = NULL);

    virtual bool    FrustumPick (const CFrustum& frustum, const Fmatrix& parent);

	virtual void    Move        (Fvector& amount);
	virtual void    Rotate      (Fvector& center, Fvector& axis, float angle);
	virtual void    Scale       (Fvector& center, Fvector& amount);
	virtual void    LocalRotate (Fvector& axis, float angle);
	virtual void    LocalScale  (Fvector& amount);

  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);

	virtual bool    GetBox      (Fbox& box);

	void            Construct   ();
    void            SetPlane    (int p_num);

    bool            Valid       (){return (m_Points.size()>=MIN_OCCLUDER_POINTS)&&(m_Points.size()<=MAX_OCCLUDER_POINTS);}

    void 			UpdatePoints3D();
    bool			ResizePoints(int new_size);
	bool 			SelectPoint	(Fvector& start, Fvector& direction, bool bLeaveSel);
    bool    		SelectPoint	(const CFrustum& frustum, bool bLeaveSel);
    void			MovePoints	(Fvector& amount);
    void			Optimize	();

    FvectorVec&		Get3DPoints	(){return m_3DPoints.m_Points;}

    virtual bool 	GetPosition	(Fvector& pos){pos.set(m_vCenter); return true; }
    virtual bool 	GetRotate	(Fvector& rot){rot.set(m_vRotate); return true;}
    virtual bool 	GetScale	(Fvector& scale){scale.set(m_vPlaneSize.x,0,m_vPlaneSize.y); return true; }

    virtual void 	SetPosition	(Fvector& pos){m_vCenter.set(pos);}
    virtual void 	SetRotate	(Fvector& rot){m_vRotate.set(rot.x,rot.y,rot.z);}
    virtual void 	SetScale	(Fvector& scale){m_vPlaneSize.set(scale.x,scale.z);}
};

#endif /*_INCDEF_Occluder_H_*/

