//---------------------------------------------------------------------------
#ifndef UI_ToolsH
#define UI_ToolsH

#include "ParticleSystem.h"
#include "ParticleGroup.h"
#include "pure.h"

// refs
class CPSObject;
class SDef;
class TfrmPropertiesPSDef;
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

class CParticleTools: public pureDeviceCreate, public pureDeviceDestroy
{
	CEditableObject*	m_EditObject;
    CPSObject*  		m_TestObject;
    PS::SDef* 			m_LibPS;
    PS::SDef			m_EditPS;
    bool				m_bModified;
	TfrmPropertiesPSDef*m_PSProps;
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
    
    TfrmText*			m_TextPG;
    PS::CPGDef*			m_LibPGD;
    PS::CParticleGroup*	m_EditPG;

    void __fastcall		OnApplyClick		();
    void __fastcall		OnCloseClick		(bool& can_close);
	bool __fastcall 	OnCodeInsight		(const AnsiString& src_line, AnsiString& hint);

    void __fastcall		OnPPMenuItemClick	(TObject* sender);
public:
	void				EditActionList		();
    void				ResetState			();
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

    void				SetCurrent			(LPCSTR name);
    void				PlayCurrent			();
    void				StopCurrent			();

    void				Rename				(LPCSTR src_name, LPCSTR part_name, int part_idx);
    void				Rename				(LPCSTR src_name, LPCSTR dest_name);

    // PS routine
    PS::SDef*			FindPS				(LPCSTR name);
    PS::SDef*			AppendPS			(LPCSTR folder_name, PS::SDef* src);
    void				RemovePS			(LPCSTR name);
    void				ResetCurrent		();
    void 				SetCurrentPS		(PS::SDef* P);
    PS::SDef*			ClonePS				(LPCSTR name);
    PS::SDef*			GetCurrentPS		(){return m_LibPS?&m_EditPS:0;}
    void				UpdateCurrent		();
    void				UpdateEmitter		();

    // PG routine
    PS::CPGDef*			FindPG				(LPCSTR name);
    PS::CPGDef*			AppendPG			(LPCSTR folder_name, PS::CPGDef* src);
    void 				SetCurrentPG		(PS::CPGDef* P);

    void				Load				();
    void				Save				();
    void				Reload				();
    void				ApplyChanges		();

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

    void				GetCurrentFog		(u32& fog_color, float& s_fog, float& e_fog);
    LPCSTR				GetInfo				();
	void				SetNumPosition		(CCustomObject* p1){;}
	void				SetNumRotation		(CCustomObject* p1){;}
	void				SetNumScale			(CCustomObject* p1){;}

    void				ShowProperties		(){;}
    void				UpdateProperties	(bool bForced=false){;}
    void				RefreshProperties	(){;}
};
extern CParticleTools	Tools;
//---------------------------------------------------------------------------
#endif
