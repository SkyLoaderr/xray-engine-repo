//----------------------------------------------------
// file: CustomObject.h
//----------------------------------------------------
#ifndef CustomObjectH
#define CustomObjectH

#include "SceneClassList.h"
#include "SceneSummaryInfo.h"

#define CHUNK_OBJECT_BODY   0x7777
//----------------------------------------------------
struct SRayPickInfo;
struct FSChunkDef;
class CFrustum;
class IReader;
class IWriter;
struct SSceneSummary;

struct SExportStreamItem{
	int					chunk;
	CMemoryWriter		stream;
    SExportStreamItem	():chunk(0){;}
};

struct SExportStreams{
	SExportStreamItem	spawn;
	SExportStreamItem	patrolpath;
	SExportStreamItem	rpoint;
	SExportStreamItem	sound_static;
	SExportStreamItem	sound_env_geom;
	SExportStreamItem	pe_static;
};

class CCustomObject {
	EObjClass 		FClassID;

    BOOL			m_bUpdateTransform;

public:
	BOOL 			m_bSelected;
	BOOL 			m_bVisible;
    BOOL 			m_bLocked;
    BOOL 			m_bValid;
public:
	string64		FName;

    // orientation
    Fvector 		FPosition;
    Fvector 		FScale;
    Fvector 		FRotation;
    Fmatrix			FTransformP;
    Fmatrix			FTransformR;
    Fmatrix			FTransformS;
    Fmatrix			FTransformRP;
	Fmatrix 		FTransform;
    Fmatrix			FITransformRP;
	Fmatrix 		FITransform;

    CCustomObject*	m_pOwnerObject;
	void __fastcall OnObjectNameAfterEdit(PropItem* sender, LPVOID edit_val);
protected:
	LPSTR			GetName			(){return FName; }
	void			SetName			(LPCSTR N){strcpy(FName,N); strlwr(FName); }

    virtual Fvector& GetPosition	()	{ return FPosition; 	}
    virtual Fvector& GetRotation	()	{ return FRotation;		}
    virtual Fvector& GetScale		()	{ return FScale; 		}

    virtual void 	SetPosition		(const Fvector& pos)	{ FPosition.set(pos);	UpdateTransform();}
	virtual void 	SetRotation		(const Fvector& rot)	{ FRotation.set(rot);	UpdateTransform();}
    virtual void 	SetScale		(const Fvector& scale)	{ FScale.set(scale);	UpdateTransform();}
public:
	IC BOOL 		Visible			(){return m_bVisible; }
	IC BOOL 		Locked			(){return m_bLocked; }
	IC BOOL 		Selected		(){return m_bSelected; }
    IC BOOL			Valid			(){return m_bValid;}

	// editor integration
	virtual void	FillProp		(LPCSTR pref, PropItemVec& items);
	virtual bool 	GetSummaryInfo	(SSceneSummary* inf){return false;}

	virtual void 	Select			(int  flag);
	virtual void 	Show			(BOOL flag);
	virtual void 	Lock			(BOOL flag);
    void			SetValid		(BOOL flag){m_bValid=flag;}

	virtual bool 	IsRender		(){return true;}
	virtual void 	Render			(int priority, bool strictB2F);
	virtual void 	OnFrame			();
    virtual void 	OnUpdateTransform();

	virtual bool 	RaySelect		(int flag, const Fvector& start, const Fvector& dir, bool bRayTest=false); // flag 1,0,-1 (-1 invert)
    virtual bool 	FrustumSelect	(int flag, const CFrustum& frustum);
	virtual bool 	RayPick			(float& dist, const Fvector& start, const Fvector& dir, SRayPickInfo* pinf=NULL){ return false; };
    virtual bool 	FrustumPick		(const CFrustum& frustum){ return false; };
    virtual bool 	SpherePick		(const Fvector& center, float radius){ return false; };

    void			ResetTransform	(){
    					FScale.set				(1,1,1);
    					FRotation.set			(0,0,0);
    					FPosition.set			(0,0,0);
						FTransform.identity		();
	    				FTransformRP.identity	();
                        FITransform.identity	();
	    				FITransformRP.identity	();
					}
    virtual void 	ResetAnimation	(bool upd_t=true){;}
    virtual void 	UpdateTransform	(bool bForced=false){m_bUpdateTransform=TRUE;if(bForced)OnUpdateTransform();}

    // grouping methods
    void			OnDetach		();
    void            OnAttach		(CCustomObject* owner);
    CCustomObject* 	GetOwner		(){return m_pOwnerObject;}

    virtual bool	OnChooseQuery	(LPCSTR specific){return true;}
    
    // change position/orientation methods
    virtual void 	NumSetPosition	(const Fvector& pos)	{ SetPosition(pos); }
	virtual void 	NumSetRotation	(const Fvector& rot)	{ SetRotation(rot);	}
    virtual void 	NumSetScale		(const Fvector& scale)	{ SetScale(scale);	}
	virtual void 	MoveTo			(const Fvector& pos, const Fvector& up);
	virtual void 	Move			(Fvector& amount);
	virtual void 	RotateParent	(Fvector& axis, float angle );
	virtual void 	RotateLocal		(Fvector& axis, float angle );
	virtual void 	Scale			(Fvector& amount);
	virtual void 	PivotRotateParent(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& axis, float angle );
	virtual void 	PivotRotateLocal(const Fmatrix& parent, Fvector& pivot, Fvector& axis, float angle );
	virtual void 	PivotScale		(const Fmatrix& prev_inv, const Fmatrix& current, Fvector& amount );

	virtual bool 	Load			(IReader&);
	virtual void 	Save			(IWriter&);
    virtual bool	ExportGame		(SExportStreams& data){return false;}

	virtual bool 	GetBox			(Fbox& box){return false;}
	virtual bool 	GetUTBox		(Fbox& box){return false;}
	virtual void 	OnSceneUpdate	(){;}
    virtual void 	OnObjectRemove	(const CCustomObject* object){;}
    virtual bool 	OnSelectionRemove(){return true;} // возвращает можно ли его удалять вообще

	virtual void 	OnDeviceCreate	(){;}
	virtual void 	OnDeviceDestroy	(){;}

	virtual void 	OnSynchronize	(){;}
    virtual void    OnShowHint      (AStringVec& dest);

					CCustomObject	(LPVOID data, LPCSTR name){
                        ClassID 	= OBJCLASS_DUMMY;
                        FName[0] 	= 0;
                        if (name) 	strcpy(FName, name);
                        m_bSelected = false;
                        m_bVisible 	= true;
                        m_bLocked 	= false;
                        m_bValid	= true;
                        m_pOwnerObject= 0;
                        ResetTransform();
                        m_bUpdateTransform = FALSE;
                    }

					CCustomObject	(CCustomObject* source){
                        ClassID 	= source->ClassID;
                        Name		= source->Name;
                        m_bSelected	= false;
                        m_bVisible 	= true;
                        m_bLocked 	= false;
                        m_bValid	= true;
                        m_pOwnerObject= 0;
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
public:
	static void		SnapMove		(Fvector& pos, Fvector& rot, const Fmatrix& rotRP, const Fvector& amount);
	static void		NormalAlign		(Fvector& rot, const Fvector& up);
};

typedef xr_list<CCustomObject*> 		ObjectList;
typedef ObjectList::iterator 			ObjectIt;
typedef xr_map<EObjClass,ObjectList> 	ObjectMap;
typedef ObjectMap::iterator 			ObjectPairIt;

//----------------------------------------------------
#endif /* _INCDEF_CustomObject_H_ */
