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
protected:
    // orientation
    Fvector 		FPosition;
    Fvector 		FScale;
    Fvector 		FRotate;
	Fmatrix 		FTransform;
protected:
    virtual Fvector& GetPosition	()	{ return FPosition; 	}
    virtual Fvector& GetRotate		()	{ return FRotate; 	}
    virtual Fvector& GetScale		()	{ return FScale; 		}

    virtual void 	SetPosition		(Fvector& pos)	{ FPosition.set(pos);	UpdateTransform();}
	virtual void 	SetRotate		(Fvector& rot)	{ FRotate.set(rot);		UpdateTransform();}
    virtual void 	SetScale		(Fvector& scale){ FScale.set(scale);	UpdateTransform();}
protected:
    int 			m_iGroupIndex;
public:
	IC BOOL 		Visible			(){return m_bVisible; }
	IC BOOL 		Locked			(){return m_bLocked; }
	IC BOOL 		Selected		(){return m_bSelected; }
    IC BOOL			Valid			(){return m_bValid;}

	virtual void 	Select			(BOOL flag);
	virtual void 	Show			(BOOL flag);
	virtual void 	Lock			(BOOL flag);
    void			SetValid		(BOOL flag){m_bValid=flag;}

    IC BOOL 		Group			(int g_idx)	{if (!IsInGroup()){m_iGroupIndex=g_idx;return true;}return false;}
    IC void 		Ungroup			()			{if (IsInGroup()){m_iGroupIndex=-1;}}
    IC BOOL 		IsInGroup		()			{return m_iGroupIndex>=0;}
    IC int 			GetGroupIndex	()			{return m_iGroupIndex;}

	IC virtual bool IsRender		(){return true;}
	virtual void 	Render			(int priority, bool strictB2F){};
	virtual void 	OnFrame			();
    virtual void 	OnUpdateTransform();

	virtual bool 	RayPick			(float& dist, Fvector& start,Fvector& dir, SRayPickInfo* pinf=NULL){ return false; };
    virtual bool 	FrustumPick		(const CFrustum& frustum){ return false; };
    virtual bool 	SpherePick		(const Fvector& center, float radius){ return false; };

    virtual void 	ResetAnimation	(bool upd_t=true){;}
    virtual void 	UpdateTransform	(bool bForced=false){m_bUpdateTransform=TRUE;if(bForced)OnUpdateTransform();}

    // change position/orientation methods
	virtual void 	Move			(Fvector& amount);
	virtual void 	Rotate			(Fvector& center, Fvector& axis, float angle );
	virtual void 	Scale			(Fvector& center, Fvector& amount );
	virtual void 	WorldRotate		(Fvector& axis, float angle );
	virtual void 	WorldScale		(Fvector& amount);
	virtual void 	LocalRotate		(Fvector& axis, float angle );
//	virtual void 	LocalScale		(Fvector& amount);

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
                        m_iGroupIndex= -1;
    					FScale.set	(1,1,1);
    					FRotate.set	(0,0,0);
    					FPosition.set(0,0,0);
						FTransform.identity();
                        m_bUpdateTransform = FALSE;
                    }

					CCustomObject	(CCustomObject* source){
                        ClassID 	= source->ClassID;
                        Name		= source->Name;
                        m_bSelected	= false;
                        m_bVisible 	= true;
                        m_bLocked 	= false;
                        m_bValid	= false;
                        m_iGroupIndex= -1;
					}

	virtual 		~CCustomObject();

    IC const Fmatrix& _Transform			(){return FTransform;}
    IC const Fvector& _Position				(){return FPosition;}
    IC const Fvector& _Rotate				(){return FRotate;}
    IC const Fvector& _Scale				(){return FScale;}

    PropertyGP(GetPosition,SetPosition)		Fvector PPosition;
    PropertyGP(GetRotate,SetRotate)			Fvector PRotate;
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
