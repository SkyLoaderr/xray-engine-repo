#ifndef ESceneCustomOToolsH
#define ESceneCustomOToolsH

#include "ESceneCustomMTools.h"

class ESceneCustomOTools: public ESceneCustomMTools
{
	typedef ESceneCustomMTools inherited;
protected:
	ObjectList			m_ObjectsInGroup;
	ObjectList			m_Objects;
    
	bool 				OnLoadSelectionAppendObject(CCustomObject* obj);
	bool 				OnLoadAppendObject		(CCustomObject* obj);
public:
						ESceneCustomOTools		(EObjClass cls);
	virtual				~ESceneCustomOTools		();
                                               
    // snap 
	virtual ObjectList* GetSnapList				();
    virtual void		UpdateSnapList			();
    
	// selection manipulate
	virtual int			RaySelect				(int flag, float& distance, const Fvector& start, const Fvector& direction, BOOL bDistanceOnly);
	virtual int			FrustumSelect			(int flag, const CFrustum& frustum);
	virtual void 		SelectObjects           (bool flag);
	virtual void 		InvertSelection         ();
	virtual void		RemoveSelection         ();
	virtual int 		SelectionCount          (bool testflag);
	virtual void		ShowObjects				(bool flag, bool bAllowSelectionFlag=false, bool bSelFlag=true);

    virtual void		Clear					(bool bSpecific=false)=0;       

    // validation
    virtual bool		Valid					();
    virtual bool		Validate				();

    // events
	virtual void		OnDeviceCreate			();
	virtual void		OnDeviceDestroy			();
	virtual void		OnSynchronize			();
	virtual void		OnSceneUpdate			();
    virtual void		OnObjectRemove			(CCustomObject* O);
	virtual void		OnFrame					();
    virtual void		OnRender				(int priority, bool strictB2F);

    // IO
    virtual bool   		IsNeedSave				()=0;
    virtual bool		Load            		(IReader&);
    virtual void		Save            		(IWriter&);
    virtual bool		LoadSelection      		(IReader&);
    virtual void		SaveSelection      		(IWriter&);

    virtual bool		Export          		(LPCSTR path);
    virtual bool		ExportGame         		(SExportStreams& F);

    // properties
    virtual void		FillProp          		(LPCSTR pref, PropItemVec& items);

    // utils
	virtual bool 		GetSummaryInfo			(SSceneSummary* inf);

    //--------------------------------------------------------------------------
    // object part
    //--------------------------------------------------------------------------
    IC ObjectList&		GetObjects				(){return m_Objects;}
    IC int				ObjCount				(){return m_Objects.size();}
    virtual int 		LockObjects				(bool flag, bool bAllowSelectionFlag, bool bSelFlag);

	virtual BOOL 		_AppendObject			(CCustomObject* object);
	virtual BOOL 		_RemoveObject			(CCustomObject* object);

    // pick function
    virtual BOOL 		RayPick					(CCustomObject*& object, float& distance, const Fvector& start, const Fvector& direction, SRayPickInfo* pinf);
    virtual BOOL 		FrustumPick				(ObjectList& lst, const CFrustum& frustum);
    virtual BOOL 		SpherePick				(ObjectList& lst, const Fvector& center, float radius);
    virtual int 		GetQueryObjects			(ObjectList& lst, int iSel, int iVis, int iLock);

    virtual CCustomObject* FindObjectByName		(LPCSTR name, CCustomObject* pass=0);

    CCustomObject*		CreateObject			(LPVOID data, LPCSTR name);
};
#endif // ESceneCustomOToolsH

