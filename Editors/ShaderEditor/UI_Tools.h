//---------------------------------------------------------------------------
#ifndef UI_ToolsH
#define UI_ToolsH

#include "SHToolsInterface.h"

// refs
class CEditableObject;
class CLibObject;
class IBlender;
class TProperties;

enum EAction{
    eaSelect=0,
    eaAdd,
    eaMove,
    eaRotate,
    eaScale,
    eaMaxActions
};

enum EAxis{
    eAxisX=0,
	eAxisY,
    eAxisZ,
    eAxisZX
};

class CShaderTools: public pureDeviceCreate, public pureDeviceDestroy
{
    EAction 			m_Action;

    void				RegisterTools		();
    void				UnregisterTools		();
public:
    float 				fFogness;
    u32					dwFogColor;
public:
    TProperties*		m_ItemProps;
    TProperties*		m_PreviewProps;

    DEFINE_MAP			(EToolsID,ISHTools*,ToolsMap,ToolsPairIt);
    ToolsMap			m_Tools;
    ISHTools*			m_Current;
    ISHTools*			Current				(){return m_Current;}
public:
						CShaderTools		();
    virtual 			~CShaderTools		();

    void				Render				();
    void				RenderEnvironment	(){;}
    void				OnFrame				();

    bool				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			();
    bool				IsModified			();
    void				Modified			();

    void				ZoomObject			(bool bOnlySel=false);

    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    void				SetFog				(u32 fog_color, float fogness){dwFogColor=fog_color;fFogness=fogness;}
    void				GetCurrentFog		(u32& fog_color, float& s_fog, float& e_fog);
    LPCSTR				GetInfo				();
    LPCSTR				CurrentToolsName	();

    void				OnChangeEditor		(ISHTools* tools);

    void				OnShowHint			(AStringVec& ss);

    void				ChangeAction		(EAction action){m_Action=action;}
    void				ApplyChanges		();

    bool __fastcall 	MouseStart  		(TShiftState Shift){return false;}
    bool __fastcall 	MouseEnd    		(TShiftState Shift){return false;}
    void __fastcall 	MouseMove   		(TShiftState Shift){;}
	bool __fastcall 	HiddenMode  		(){return false;}
    bool __fastcall 	KeyDown     		(WORD Key, TShiftState Shift){return false;}
    bool __fastcall 	KeyUp       		(WORD Key, TShiftState Shift){return false;}
    bool __fastcall 	KeyPress    		(WORD Key, TShiftState Shift){return false;}

    bool				Pick				(TShiftState Shift){return false;}

    void				ShowProperties		();
    void				UpdateProperties	(bool bForced=false){;}
    void				RefreshProperties	(){;}

	void				SetNumPosition		(CCustomObject* p1){;}
	void				SetNumRotation		(CCustomObject* p1){;}
	void				SetNumScale			(CCustomObject* p1){;}

    ISHTools*			FindTools			(EToolsID id);
    ISHTools*			FindTools			(TElTabSheet* sheet);

    void				Save				();
	void 				Reload				();
};
extern CShaderTools	Tools;
//---------------------------------------------------------------------------
#endif
