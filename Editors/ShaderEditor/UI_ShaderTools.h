//---------------------------------------------------------------------------
#ifndef UI_ShaderToolsH
#define UI_ShaderToolsH

#include "SHToolsInterface.h"
#include "UI_ToolsCustom.h"

// refs
class CEditableObject;
class CLibObject;
class IBlender;
class TProperties;

class CShaderTools: public CToolsCustom
{
    void				RegisterTools		();
    void				UnregisterTools		();
    void				RealUpdateProperties();

    enum{
    	flRefreshProps 	= (1ul<<0ul),
    };
    Flags32				m_Flags;
public:
	TItemList*			m_Items;
    TProperties*		m_ItemProps;
    TProperties*		m_PreviewProps;

    DEFINE_MAP			(EToolsID,ISHTools*,ToolsMap,ToolsPairIt);
    ToolsMap			m_Tools;
    ISHTools*			m_Current;
    ISHTools*			Current				(){return m_Current;}

    void __fastcall 	OnItemFocused		(ListItemsVec& items);
public:
						CShaderTools		();
    virtual 			~CShaderTools		();

    virtual void		Render				();
	virtual void		RenderEnvironment	(){;}
    virtual void		OnFrame				();

    virtual bool		OnCreate			();
    virtual void		OnDestroy			();

    virtual bool		IfModified			();
    virtual bool		IsModified			();
    virtual void		Modified			(); 

    virtual LPCSTR		GetInfo				();
    
    virtual void		ZoomObject			(bool bSelOnly);

    virtual bool		Load				(LPCSTR path, LPCSTR name);
    virtual bool		Save				(LPCSTR path, LPCSTR name, bool bInternal=false);
    virtual void		Reload				();
    
    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    virtual void		Clear				(){;}

    virtual void		OnShowHint			(AStringVec& SS);

    virtual bool __fastcall 	MouseStart  		(TShiftState Shift){return false;}
    virtual bool __fastcall 	MouseEnd    		(TShiftState Shift){return false;}
    virtual void __fastcall 	MouseMove   		(TShiftState Shift){;}

    virtual bool		Pick				(TShiftState Shift){return false;}
	virtual bool 		RayPick				(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n);

    virtual void		ShowProperties		();
    virtual void		UpdateProperties	(bool bForced=false){m_Flags.set(flRefreshProps,TRUE); if (bForced) RealUpdateProperties();}
    virtual void		RefreshProperties	(){;}

    LPCSTR				CurrentToolsName	();

    void				OnChangeEditor		(ISHTools* tools);

    void				ApplyChanges		();

    ISHTools*			FindTools			(EToolsID id);
    ISHTools*			FindTools			(TElTabSheet* sheet);
};
extern CShaderTools*&	STools;
//---------------------------------------------------------------------------
#endif
