//----------------------------------------------------
// file: WayPoint.h
//----------------------------------------------------

#ifndef _INCDEF_WayPoint_H_
#define _INCDEF_WayPoint_H_                                

#include "CustomObject.h"

class CFrustum;

class CWayPoint: public CCustomObject {
	friend class 	SceneBuilder;

    ObjectList		m_Links;
    // temporary storage (load only)
    AStringVec		m_NameLinks;
    AnsiString		m_PathName;

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

    bool			AddLink			(CWayPoint* P);
    bool			RemoveLink		(CWayPoint* P);
    LPCSTR			GetPathName		(){return m_PathName.c_str();}
    void			SetPathName     (LPCSTR name, bool bOnlyThis=false);

	virtual void 	RotateParent	(Fvector& axis, float angle ){;}
	virtual void 	RotateLocal		(Fvector& axis, float angle ){;}
	virtual void 	Scale			(Fvector& amount){;}
};
#endif /*_INCDEF_WayPoint_H_*/

