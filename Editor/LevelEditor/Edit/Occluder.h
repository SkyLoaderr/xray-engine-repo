//----------------------------------------------------
// file: Occluder.h
//----------------------------------------------------

#ifndef _INCDEF_Occluder_H_
#define _INCDEF_Occluder_H_

#include "CustomObject.h"

#define MIN_OCCLUDER_POINTS 3
#define MAX_OCCLUDER_POINTS 6

class CFrustum;


class COccluder: public CCustomObject {
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

	virtual void    Render      (int priority, bool strictB2F);

	virtual bool    RayPick		(float& distance, Fvector& start, Fvector& direction, SRayPickInfo* pinf = NULL);

    virtual bool    FrustumPick (const CFrustum& frustum);

	virtual void    Move        (Fvector& amount);
	virtual void    Rotate      (Fvector& center, Fvector& axis, float angle);
	virtual void    Scale       (Fvector& center, Fvector& amount);
	virtual void    ParentRotate(Fvector& axis, float angle);
	virtual void    ParentScale (Fvector& amount);

  	virtual bool 	Load		(CStream&);
	virtual void 	Save		(CFS_Base&);

	virtual bool    GetBox      (Fbox& box);

	void            Construct   ();
    void            SetPlane    (int p_num);

    BOOL            Valid       (){return (m_Points.size()>=MIN_OCCLUDER_POINTS)&&(m_Points.size()<=MAX_OCCLUDER_POINTS);}

    void 			UpdatePoints3D();
    bool			ResizePoints(int new_size);
	bool 			SelectPoint	(Fvector& start, Fvector& direction, bool bLeaveSel);
    bool    		SelectPoint	(const CFrustum& frustum, bool bLeaveSel);
    void			MovePoints	(Fvector& amount);
    void			Optimize	();

    FvectorVec&		Get3DPoints	(){return m_3DPoints.m_Points;}
};

#endif /*_INCDEF_Occluder_H_*/

