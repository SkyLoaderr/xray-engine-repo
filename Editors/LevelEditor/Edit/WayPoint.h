//----------------------------------------------------
// file: WayPoint.h
//----------------------------------------------------

#ifndef _INCDEF_WayPoint_H_
#define _INCDEF_WayPoint_H_                                

#include "LevelGameDef.h"

class CFrustum;
class CWayPoint;

DEFINE_VECTOR(CWayPoint*,WPVec,WPIt);
class CWayPoint{
    friend class CWayObject;
    friend class TfrmPropertiesWayPoint;
    Fvector		m_vPosition;
    DWORD		m_dwFlags;
    BOOL		m_bSelected;
    WPVec		m_Links;
    void		AppendLink		(CWayPoint* P);
    bool		DeleteLink		(CWayPoint* P);
public:
                CWayPoint		();
    void    	Render      	(bool bParentSelect);
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
    void		GetBox			(Fbox& bb);
};

class CWayObject: public CCustomObject{
protected:
    friend class TfrmPropertiesWayPoint;
	EWayType		m_Type;
    WPVec			m_WayPoints;
    typedef CCustomObject inherited;
public:
					CWayObject		(LPVOID data, LPCSTR name);
    void            Construct   	(LPVOID data);
	virtual			~CWayObject		();
    void			Clear			();

    EWayType		GetType			(){return m_Type;}

	virtual void 	Select			(int flag);
	virtual bool 	RaySelect		(int flag, Fvector& start,Fvector& dir, bool bRayTest=false); // flag 1,0,-1 (-1 invert)
    virtual bool 	FrustumSelect	(int flag, const CFrustum& frustum);

    CWayPoint*		AppendWayPoint	();
    CWayPoint*		GetFirstSelected();
    int 			GetSelectedPoints(WPVec& lst);
    void			RemoveSelectedPoints();
    void			RemoveLink		();
    void			InvertLink		();
    void			Add1Link		();
    void			Add2Link		();
    // change position/orientation methods
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle ){;}
	virtual void 	RotateLocal		(Fvector& axis, float angle ){;}
	virtual void 	Scale			(Fvector& amount){;}
	virtual void 	PivotRotateParent(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& axis, float angle ){;}
	virtual void 	PivotRotateLocal(const Fmatrix& parent, Fvector& pivot, Fvector& axis, float angle ){;}
	virtual void 	PivotScale		(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount ){;}

	virtual bool    GetBox      	(Fbox& box);
	virtual void    Render      	(int priority, bool strictB2F);
	virtual bool    RayPick	    	(float& distance, Fvector& S, Fvector& D, SRayPickInfo* pinf = NULL);
    virtual bool 	FrustumPick		(const CFrustum& frustum);
  	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);
    virtual bool	ExportGame		(SExportStreams& data);

    static bool		IsPointMode		();
};
#endif /*_INCDEF_WayPoint_H_*/

