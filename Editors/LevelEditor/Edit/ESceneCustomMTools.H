#ifndef SceneCustomMToolsH
#define SceneCustomMToolsH

// refs
struct SSceneSummary;

class ESceneCustomMTools
{
	EObjClass 			FClassID;
protected:
	int 				render_priority;
    LPSTR				class_name;
    LPSTR				class_desc;
public:
    PropertyGP			(FClassID,FClassID) 	EObjClass ClassID;
    IC void				SetDefinition			(LPCSTR cls_name, LPCSTR cls_desc, int rp)
    {
        class_name		= xr_strdup(cls_name);
        class_desc		= xr_strdup(cls_desc);
        render_priority	= rp;
    }
	// definition
    IC LPCSTR			ClassName				(){return class_name;}
    IC LPCSTR			ClassDesc				(){return class_desc;}
    IC int				RenderPriority			(){return render_priority;}
protected:
    enum{
    	flVisible		= (0<<1),
    };
    Flags32				m_Flags;
public:
						ESceneCustomMTools		(EObjClass cls)
                        {
                        	ClassID				= cls;
                            class_name			= 0;
                            class_desc			= 0;
                            render_priority		= 0;
                            m_Flags.set			(flVisible);
                        }
	virtual				~ESceneCustomMTools		(){xr_free(class_name);xr_free(class_desc);}

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
    virtual void		OnRender				(int priority, bool strictB2F)=0;

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
#endif //SceneCustomMToolsH
 