#ifndef SceneCustomMToolsH
#define SceneCustomMToolsH

// refs
struct SSceneSummary;

class ESceneCustomMTools
{
	EObjClass 		FClassID;
public:
    PropertyGP(FClassID,FClassID)			EObjClass ClassID;
public:
					ESceneCustomMTools		(){;}
	virtual			~ESceneCustomMTools		(){;}
                                               
    // snap 
	virtual ObjectList& GetSnapList				()=0;
    virtual void		UpdateSnapList			()=0;
    
	// selection manipulate
    virtual bool		PickGround				(Fvector& dest, const Fvector& start, const Fvector& dir, float dist)=0;
	virtual int			RaySelect				(bool flag, float& distance, const Fvector& start, const Fvector& direction)=0;
	virtual int			FrustumSelect			(bool flag)=0;
	virtual int 		SelectObjects           (bool flag)=0;
	virtual int 		InvertSelection         ()=0;
	virtual int 		RemoveSelection         ()=0;
	virtual int 		SelectionCount          (bool testflag)=0;
	virtual int 		ShowObjects				(bool flag, bool bAllowSelectionFlag=false, bool bSelFlag=true)=0;

    virtual void		Clear					(bool bSpecific=false)=0;       

    // validation
    virtual bool		Valid					()=0;

    // events
	virtual void		OnDeviceCreate			()=0;
	virtual void		OnDeviceDestroy			()=0;
	virtual void		OnSynchronize			()=0;
    virtual void		OnObjectRemove			(CCustomObject* O)=0;
	virtual void		OnFrame					()=0;
    virtual void		OnRender				(int priority, bool strictB2F)=0;

    // IO
    virtual bool   		IsNeedSave				()=0;
    virtual bool		Load            		(IReader&)=0;
    virtual void		Save            		(IWriter&)=0;
    virtual bool		Export          		(LPCSTR fn)=0;

    // properties
    virtual void		FillProp          		(LPCSTR pref, PropItemVec& items)=0;

    // utils
	virtual bool 		GetSummaryInfo			(SSceneSummary* inf)=0;
};

typedef xr_map<EObjClass,ESceneCustomMTools*> 	SceneToolsMap;
#endif //SceneCustomMToolsH
 