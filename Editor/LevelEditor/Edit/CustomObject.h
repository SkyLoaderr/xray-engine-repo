//----------------------------------------------------
// file: CustomObject.h
//----------------------------------------------------
#ifndef _INCDEF_CustomObject_H_
#define _INCDEF_CustomObject_H_

#include "SceneClassList.h"

#define CHUNK_OBJECT_BODY   0x7777
//----------------------------------------------------
struct SRayPickInfo;
struct FSChunkDef;
class CFrustum;
class CStream;
class CFS_Base;
class CGroupObject;

class CCustomObject {
	EObjClass 		FClassID;
	string64		FName;

    BOOL			m_bUpdateTransform;

	BOOL 			m_bSelected;
	BOOL 			m_bVisible;
    BOOL 			m_bLocked;
    BOOL 			m_bValid;

	LPSTR			GetName			(){return FName; }
	void			SetName			(LPCSTR N){strcpy(FName,N); strlwr(FName); }
public:
    // orientation
    Fvector 		FPosition;
    Fvector 		FScale;
    Fvector 		FRotation;
    Fmatrix			FTransformRP;
	Fmatrix 		FTransform;
    Fmatrix			FITransformRP;
	Fmatrix 		FITransform;

    CGroupObject*	m_pGroupObject;
protected:
    virtual Fvector& GetPosition	()	{ return FPosition; 	}
    virtual Fvector& GetRotation	()	{ return FRotation;		}
    virtual Fvector& GetScale		()	{ return FScale; 		}

    virtual void 	SetPosition		(Fvector& pos)	{ FPosition.set(pos);	UpdateTransform();}
	virtual void 	SetRotation		(Fvector& rot)	{ FRotation.set(rot);	UpdateTransform();}
    virtual void 	SetScale		(Fvector& scale){ FScale.set(scale);	UpdateTransform();}
public:
	IC BOOL 		Visible			(){return m_bVisible; }
	IC BOOL 		Locked			(){return m_bLocked; }
	IC BOOL 		Selected		(){return m_bSelected; }
    IC BOOL			Valid			(){return m_bValid;}

	virtual void 	Select			(int  flag);
	virtual void 	Show			(BOOL flag);
	virtual void 	Lock			(BOOL flag);
    void			SetValid		(BOOL flag){m_bValid=flag;}
                                                                              
	virtual bool 	IsRender		(){return true;}
	virtual void 	Render			(int priority, bool strictB2F);
	virtual void 	OnFrame			();
    virtual void 	OnUpdateTransform();

	virtual bool 	RaySelect		(int flag, Fvector& start,Fvector& dir, bool bRayTest=false); // flag 1,0,-1 (-1 invert)
    virtual bool 	FrustumSelect	(int flag, const CFrustum& frustum);
	virtual bool 	RayPick			(float& dist, Fvector& start,Fvector& dir, SRayPickInfo* pinf=NULL){ return false; };
    virtual bool 	FrustumPick		(const CFrustum& frustum){ return false; };
    virtual bool 	SpherePick		(const Fvector& center, float radius){ return false; };

    void			ResetTransform	(){
    					FScale.set	(1,1,1);
    					FRotation.set	(0,0,0);
    					FPosition.set(0,0,0);
						FTransform.identity();
					}
    virtual void 	ResetAnimation	(bool upd_t=true){;}
    virtual void 	UpdateTransform	(bool bForced=false){m_bUpdateTransform=TRUE;if(bForced)OnUpdateTransform();}

    // grouping methods
    void			RemoveFromGroup	();
    void            AppendToGroup	(CGroupObject* group);
    CGroupObject* 	GetGroup		(){return m_pGroupObject;}

    // change position/orientation methods
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle );
	virtual void 	RotateLocal		(Fvector& axis, float angle );
	virtual void 	Scale			(Fvector& amount);
	virtual void 	PivotRotateParent(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& axis, float angle );
	virtual void 	PivotRotateLocal(const Fmatrix& parent, Fvector& pivot, Fvector& axis, float angle );
	virtual void 	PivotScale		(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount );

	virtual bool 	Load			(CStream&);
	virtual void 	Save			(CFS_Base&);

	virtual bool 	GetBox			(Fbox& box){return false; }
	virtual void 	OnSceneUpdate	(){;}
	virtual void 	OnDestroy		(){;}

	virtual void 	OnDeviceCreate	(){;}
	virtual void 	OnDeviceDestroy	(){;}

	virtual void 	OnSynchronize	(){;}
    virtual void    OnShowHint      (AStringVec& dest);

					CCustomObject	(){
                        ClassID 	= OBJCLASS_DUMMY;
                        FName[0] 	= 0;
                        m_bSelected = false;
                        m_bVisible 	= true;
                        m_bLocked 	= false;
                        m_bValid	= false;
                        m_pGroupObject= 0;
                        ResetTransform();
                        m_bUpdateTransform = FALSE;
                    }

					CCustomObject	(CCustomObject* source){
                        ClassID 	= source->ClassID;
                        Name		= source->Name;
                        m_bSelected	= false;
                        m_bVisible 	= true;
                        m_bLocked 	= false;
                        m_bValid	= false;
                        m_pGroupObject= 0;
					}

	virtual 		~CCustomObject();

    IC const Fmatrix& _ITransform			(){return FITransform;}
    IC const Fmatrix& _Transform			(){return FTransform;}
    IC const Fvector& _Position				(){return FPosition;}
    IC const Fvector& _Rotation				(){return FRotation;}
    IC const Fvector& _Scale				(){return FScale;}

    PropertyGP(GetPosition,SetPosition)		Fvector PPosition;
    PropertyGP(GetRotation,SetRotation)		Fvector PRotation;
    PropertyGP(GetScale,SetScale)			Fvector PScale;

    PropertyGP(FClassID,FClassID)			EObjClass ClassID;
    PropertyGP(GetName,SetName) 			LPSTR  	Name;
};

typedef std::list<CCustomObject*> ObjectList;
typedef ObjectList::iterator ObjectIt;
typedef std::map<EObjClass,ObjectList> ObjectMap;
typedef ObjectMap::iterator ObjectPairIt;

//----------------------------------------------------
#endif /* _INCDEF_CustomObject_H_ */
