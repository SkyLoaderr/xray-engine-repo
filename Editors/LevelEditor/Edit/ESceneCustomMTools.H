#ifndef ESceneCustomMToolsH
#define ESceneCustomMToolsH

// refs
struct SSceneSummary;
class TUI_CustomControl;
class ESceneCustomMTools;

DEFINE_VECTOR(TUI_CustomControl*,ControlsVec,ControlsIt);

class ESceneCustomMTools
{
	EObjClass 			FClassID;
protected:
	// controls
    ControlsVec 		m_Controls;
    int             	action;
    int					sub_target;
public:
    TUI_CustomControl* 	pCurControl;
    TForm*				pFrame;
protected:
    void            	AddControl				(TUI_CustomControl* c);
    TUI_CustomControl* 	FindControl				(int subtarget, int action);
    void            	UpdateControl			();
    void				CreateDefaultControls	();
public:
    void            	SetAction   			(int action);
    void            	SetSubTarget			(int target);
    void            	ResetSubTarget			();
protected:
    virtual void		CreateControls			();
    virtual void		RemoveControls			();
public:
    virtual void    	OnActivate  			();
    virtual void    	OnDeactivate			();
    virtual void    	OnObjectsUpdate			(){;}
public:
    PropertyGP			(FClassID,FClassID) 	EObjClass ClassID;
	// definition
    virtual LPCSTR		ClassName				()=0;
    virtual LPCSTR		ClassDesc				()=0;
    virtual int			RenderPriority			()=0;
protected:
    enum{
    	flVisible		= (0<<1),
    };
    Flags32				m_Flags;
public:
						ESceneCustomMTools		(EObjClass cls);
	virtual				~ESceneCustomMTools		();

    virtual void		OnCreate				();
    virtual void		OnDestroy				();
    
    // snap 
	virtual ObjectList* GetSnapList				()=0;
    virtual void		UpdateSnapList			()=0;
    
	// selection manipulate
    virtual bool		PickGround				(Fvector& dest, const Fvector& start, const Fvector& dir, float dist)=0;
	virtual int			RaySelect				(bool flag, float& distance, const Fvector& start, const Fvector& direction)=0;
	virtual int			FrustumSelect			(bool flag, const CFrustum& frustum)=0;
	virtual int 		SelectObjects           (bool flag)=0;
	virtual int 		InvertSelection         ()=0;
	virtual int 		RemoveSelection         ()=0;
	virtual int 		SelectionCount          (bool testflag)=0;
	virtual int 		ShowObjects				(bool flag, bool bAllowSelectionFlag=false, bool bSelFlag=true)=0;

    virtual void		Clear					(bool bSpecific=false)=0;       

    // validation
    virtual bool		Valid					()=0;
    virtual bool		Validate				()=0;

    // events
	virtual void		OnDeviceCreate			()=0;
	virtual void		OnDeviceDestroy			()=0;
	virtual void		OnSynchronize			()=0;
	virtual void		OnSceneUpdate			(){;}
    virtual void		OnObjectRemove			(CCustomObject* O)=0;
	virtual void		OnFrame					()=0;

    // render
    virtual void		BeforeRender			(){;}
    virtual void		OnRender				(int priority, bool strictB2F)=0;
    virtual void		AfterRender				(){;}

    // IO
    virtual bool   		IsNeedSave				()=0;
    virtual bool		Load            		(IReader&)=0;
    virtual void		Save            		(IWriter&)=0;
    virtual bool		LoadSelection      		(IReader&)=0;
    virtual void		SaveSelection      		(IWriter&)=0;

    virtual bool		Export          		(LPCSTR fn){return false;}
    virtual bool		ExportGame         		(SExportStreams& F){return FALSE;}

    // properties
    virtual void		FillProp          		(LPCSTR pref, PropItemVec& items)=0;

    // utils
	virtual bool 		GetSummaryInfo			(SSceneSummary* inf)=0;
};

DEFINE_MAP(EObjClass,ESceneCustomMTools*,SceneToolsMap,SceneToolsMapPairIt);
#endif //ESceneCustomMToolsH
 