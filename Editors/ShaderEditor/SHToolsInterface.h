//---------------------------------------------------------------------------
#ifndef SHToolsInterfaceH
#define SHToolsInterfaceH

#include "ElTree.hpp"
#include "MxMenus.hpp"
#include "ElPgCtl.hpp"
#include "PropertiesList.h"

// refs

enum EToolsID{
	aeFirstTool = 0,
	aeEngine = 0,
    aeCompiler,
    aeMtl,
    aeMtlPair,
    aeSoundEnv,
    aeMaxTools
};

struct ISHInit
{
    EToolsID				tools_id;
	TElTree*				tvView;
    TMxPopupMenu*			pmMenu;
    TProperties*			m_ItemProps;
    TProperties*			m_PreviewProps;
    TElTabSheet*			m_Sheet;
    ISHInit(){ZeroMemory(this,sizeof(ISHInit));}
	ISHInit(EToolsID id, TElTree* tv, TMxPopupMenu* pm, TElTabSheet* sh, TProperties* ip, TProperties* pp)
    {
        tools_id			= id;
        tvView				= tv;
        pmMenu				= pm;
        m_ItemProps			= ip;
        m_PreviewProps		= pp;
        m_Sheet				= sh;
    }
};
class ISHTools
{
protected:
	ISHInit					Ext;

	BOOL					m_bModified;

    virtual LPCSTR			GenerateItemName	(LPSTR name, LPCSTR pref, LPCSTR source)=0;

    BOOL					m_bLockUpdate;	// если менялся объект непосредственно  Update____From___()

public:
	void					ViewAddItem			(LPCSTR full_name);
    void					ViewSetCurrentItem	(LPCSTR full_name);
    void					ViewClearItemList	();
public:
    virtual LPCSTR			AppendItem			(LPCSTR folder_name, LPCSTR parent=0)=0;
    virtual void			RemoveItem			(LPCSTR name)=0;
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR ren_part, int level)=0;
	virtual void			RenameItem			(LPCSTR old_full_name, LPCSTR new_full_name)=0;

	virtual void			FillItemList		()=0;
public:
							ISHTools 			(ISHInit& init);
    virtual 				~ISHTools			(){;}

    TElTree*				View				(){return Ext.tvView;}
    EToolsID				ID					(){return Ext.tools_id;}
    TMxPopupMenu*			PopupMenu			(){return Ext.pmMenu;}
    TElTabSheet*			Sheet				(){return Ext.m_Sheet;}
    virtual LPCSTR			ToolsName			()=0;
    
	virtual void			Reload				()=0;
	virtual void			Load				()=0;
	virtual void			Save				()=0;

    bool					IsModified			(){return m_bModified;}
    virtual bool			IfModified			();
    virtual void __fastcall	Modified			();

    virtual bool			OnCreate			()=0;
    virtual void			OnDestroy			()=0;
	virtual void 			OnActivate			()=0;
	virtual void 			OnDeactivate		()=0;

    // misc
    virtual void			ResetCurrentItem	()=0;
    virtual void			SetCurrentItem		(LPCSTR name)=0;
    virtual void			ApplyChanges		(bool bForced=false)=0;

	virtual void 			UpdateProperties	()=0;

	virtual void 			OnFrame				()=0;
	virtual void 			OnRender			()=0;

    virtual void			OnDeviceCreate		()=0;
    virtual void			OnDeviceDestroy		()=0;

    virtual void			ZoomObject			(bool bOnlySel);
	virtual void 			OnShowHint			(AStringVec& ss){;}
};
//---------------------------------------------------------------------------
#endif
