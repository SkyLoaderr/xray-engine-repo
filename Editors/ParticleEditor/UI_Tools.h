//---------------------------------------------------------------------------
#ifndef UI_ToolsH
#define UI_ToolsH

#include "ParticleSystem.h"
#include "ParticleEffect.h"
#include "ParticleGroup.h"
#include "pure.h"
#include "PropertiesList.h"
#include "ItemList.h"

// refs
class CEditableObject;
class TfrmText;

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

enum EEditMode{
    emEffect,
    emGroup
};

class CParticleTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CEditableObject*	m_EditObject;
    bool				m_bModified;
    bool				m_bReady;

    EAction				m_Action;
    bool				m_bHiddenMode;
	// move
    Fvector				m_MovingXVector;
    Fvector				m_MovingYVector;
    Fvector				m_MovingReminder;
	// scale
    Fvector				m_ScaleCenter;
    // rotate
    Fvector				m_RotateCenter;
    Fvector				m_RotateVector;
    float				m_fRotateSnapAngle;

    // PE variables
    PS::CPEDef*			m_LibPED;
    PS::CParticleEffect*m_EditPE;

    // PG variables
    PS::CPGDef*			m_LibPGD;
    PS::CParticleGroup*	m_EditPG;

    Fmatrix 			m_Transform;

    void __fastcall		OnApplyClick		();
    void __fastcall		OnCloseClick		(bool& can_close);
	bool __fastcall 	OnCodeInsight		(const AnsiString& src_line, AnsiString& hint);

    void __fastcall		OnPPMenuItemClick	(TObject* sender);

    void __fastcall		OnItemModified		(void); 

    void __fastcall 	OnParticleItemFocused	(ListItemsVec& items);
	void __fastcall 	OnParticleItemRename	(LPCSTR old_name, LPCSTR new_name, EItemType type);
    BOOL __fastcall 	OnParticleItemRemove	(LPCSTR name, EItemType type);

    void				RealUpdateProperties();
	void 				SelectListItem		(LPCSTR pref, LPCSTR name, bool bVal, bool bLeaveSel, bool bExpand);

    void				RealApplyParent		();
    void				ApplyParent			(bool bForce=false){m_Flags.set(flApplyParent,TRUE); if (bForce) RealApplyParent();}
public:
	EEditMode			m_EditMode;
    
    TfrmText*			m_EditText;
    TProperties*		m_ItemProps;
    TItemList*			m_PList;
public:
	void				EditActionList		();
    void				ResetState			();
public:
    float 				fFogness;
    u32					dwFogColor;
public:
	// flags
    enum{
    	flRefreshProps 		= (1<<0),
        flApplyParent		= (1<<1),
    };
    Flags32				m_Flags;
public:
						CParticleTools		();
    virtual 			~CParticleTools		();

    void				Render				();
    void				RenderEnvironment	(){;}
    void				OnFrame				();

    bool				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			();
    bool				IsModified			(){return m_bModified;}
    void				Modified			();

    void				ZoomObject			(BOOL bObjectOnly);
    void				ChangeAction		(EAction action);

    void				PlayCurrent			();
    void				StopCurrent			(bool bFinishPlaying);

    void				Rename				(LPCSTR src_name, LPCSTR part_name, int part_idx);
    void				Rename				(LPCSTR src_name, LPCSTR dest_name);

    // PS routine
	void 				CloneCurrent		();
    void				ResetCurrent		();
    void				RemoveCurrent		();
	void 				Remove				(LPCSTR name);

    // PG routine
    PS::CPEDef*			FindPE				(LPCSTR name);
    PS::CPEDef*			AppendPE			(PS::CPEDef* src);
    void 				SetCurrentPE		(PS::CPEDef* P);

    // PG routine
    PS::CPGDef*			FindPG				(LPCSTR name);
    PS::CPGDef*			AppendPG			(PS::CPGDef* src);
    void 				SetCurrentPG		(PS::CPGDef* P);

    void				Load				();
    void				Save				();
    void				Reload				();

    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    void				SelectPreviewObject	(int p);
    void				ResetPreviewObject	();

    void				OnShowHint			(AStringVec& SS);

    bool __fastcall 	MouseStart  		(TShiftState Shift);
    bool __fastcall 	MouseEnd    		(TShiftState Shift);
    void __fastcall 	MouseMove   		(TShiftState Shift);
	bool __fastcall 	HiddenMode  		(){return m_bHiddenMode;}
    bool __fastcall 	KeyDown     		(WORD Key, TShiftState Shift){return false;}
    bool __fastcall 	KeyUp       		(WORD Key, TShiftState Shift){return false;}
    bool __fastcall 	KeyPress    		(WORD Key, TShiftState Shift){return false;}

    bool				Pick				(TShiftState Shift){return false;}
    bool				RayPick				(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt=0, Fvector* n=0);

    void				SetFog				(u32 fog_color, float fogness){dwFogColor=fog_color;fFogness=fogness;}
    void				GetCurrentFog		(u32& fog_color, float& s_fog, float& e_fog);
    LPCSTR				GetInfo				();
	void				SetNumPosition		(CCustomObject* p1){;}
	void				SetNumRotation		(CCustomObject* p1){;}
	void				SetNumScale			(CCustomObject* p1){;}

    void				ShowProperties		(){;}
    void				UpdateProperties	(bool bForced=false){m_Flags.set(flRefreshProps,TRUE); if (bForced) RealUpdateProperties();}
    void				RefreshProperties	(){;}
};
#define SYSTEM_PREFIX 	"Systems"
#define EFFECT_PREFIX 	"Effects"
#define GROUP_PREFIX 	"Groups"
extern CParticleTools	Tools;
//---------------------------------------------------------------------------
#endif
