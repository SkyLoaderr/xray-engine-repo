//----------------------------------------------------
// file: WayPoint.h
//----------------------------------------------------

#ifndef _INCDEF_WayPoint_H_
#define _INCDEF_WayPoint_H_                                

#include "CustomObject.h"

class CFrustum;
class CWayPoint;

DEFINE_VECTOR(CWayPoint*,WPVec,WPIt);
class CWayPoint{
    friend class CWayObject;
    Fvector		m_vPosition;
    DWORD		m_dwFlag;
    bool		m_bSelected;
    WPVec		m_Links;
    void		AppendLink		(CWayPoint* P);
    bool		DeleteLink		(CWayPoint* P);
public:
                CWayPoint		();
    void    	Render      	();
    bool    	RayPick	    	(float& distance, Fvector& S, Fvector& D);
    bool 		FrustumPick		(const CFrustum& frustum);
    bool 		FrustumSelect	(int flag, const CFrustum& frustum);
    void		Select			(int flag);
    void 		MoveTo			(const Fvector& pos){m_vPosition.set(pos);}
    bool		AddSingleLink	(CWayPoint* P);
    bool		AddDoubleLink	(CWayPoint* P);
    bool		RemoveLink		(CWayPoint* P);
    void		InvertLink		(CWayPoint* P);
    WPIt		HasLink			(CWayPoint* P);
};

class CWayObject: public CCustomObject{
protected:
    WPVec			m_WayPoints;

    typedef CCustomObject inherited;
public:
					CWayObject		();
					CWayObject		(LPSTR name);
    void            Construct   	();
	virtual			~CWayObject		();

	virtual void 	Select			(int flag);
	virtual bool 	RaySelect		(int flag, Fvector& start,Fvector& dir, bool bRayTest=false); // flag 1,0,-1 (-1 invert)
    virtual bool 	FrustumSelect	(int flag, const CFrustum& frustum);

    CWayPoint*		AppendWayPoint	();
    CWayPoint*		GetFirstSelected();
    int 			GetSelectedPoints(WPVec& lst);
    void			RemoveSelectedPoints();
    // change position/orientation methods
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount){;}
	virtual void 	RotateParent	(Fvector& axis, float angle ){;}
	virtual void 	RotateLocal		(Fvector& axis, float angle ){;}
	virtual void 	Scale			(Fvector& amount){;}
	virtual void 	PivotRotateParent(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& axis, float angle ){;}
	virtual void 	PivotRotateLocal(const Fmatrix& parent, Fvector& pivot, Fvector& axis, float angle ){;}
	virtual void 	PivotScale		(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount ){;}

	virtual void    Render      	(int priority, bool strictB2F);
	virtual bool    RayPick	    	(float& distance, Fvector& S, Fvector& D, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);
  	virtual bool 	Load			(CStream&){;}
	virtual void 	Save			(CFS_Base&){;}
	virtual bool    GetBox      	(Fbox& box);
};
/*
class CWayPoint{
	friend class 	SceneBuilder;

    AnsiString		m_PathName;
    ObjectList		m_Links[2];
    // temporary storage (load only)
    AStringVec		m_NameLinks[2];

    void            DrawPoint		(Fcolor& c);
    void			DrawLinks		(Fcolor& c);

    void			AppendLink		(CWayPoint* P);
    bool			DeleteLink		(CWayPoint* P);

    typedef CCustomObject inherited;
public:
    void			GetLinkedObjects(ObjectList& lst);
public:
	                CWayPoint		();
	                CWayPoint   	(char *name);
    void            Construct   	();
	virtual         ~CWayPoint  	();

	virtual void    Render      	(int priority, bool strictB2F);
	virtual bool    RayPick	    	(float& distance, Fvector& S, Fvector& D, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);
  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
	virtual bool    GetBox      	(Fbox& box);
	virtual void 	OnDestroy		();
	virtual void 	OnSynchronize	();

    bool			AddSingleLink	(CWayPoint* P);
    bool			AddDoubleLink	(CWayPoint* P);
    bool			RemoveLink		(CWayPoint* P);
    LPCSTR			GetPathName		(){return m_PathName.c_str();}
    void			SetPathName     (LPCSTR name, bool bOnlyThis=false);

	virtual void 	RotateParent	(Fvector& axis, float angle ){;}
	virtual void 	RotateLocal		(Fvector& axis, float angle ){;}
	virtual void 	Scale			(Fvector& amount){;}
};
*/
#endif /*_INCDEF_WayPoint_H_*/

