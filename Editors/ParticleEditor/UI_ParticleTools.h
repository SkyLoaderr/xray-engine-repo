//---------------------------------------------------------------------------
#ifndef UI_ParticleToolsH
#define UI_ParticleToolsH

#include "ParticleSystem.h"
#include "ParticleEffect.h"
#include "ParticleGroup.h"
#include "pure.h"
#include "PropertiesList.h"
#include "ui_toolscustom.h"
#include "ui_maincommand.h"

// refs
class CEditableObject;
class CObjectAnimator;

enum EEditMode{
	emNone,
    emEffect,
    emGroup
};

class CParticleTools: public CToolsCustom
{
	typedef CToolsCustom inherited;
	friend class TfraLeftBar;

	CEditableObject*	m_EditObject;
    bool				m_bModified;

    // PE variables
    PS::CPEDef*			m_LibPED;
    PS::CParticleEffect*m_EditPE;

    // PG variables
    PS::CPGDef*			m_LibPGD;
    PS::CParticleGroup*	m_EditPG;

    Fmatrix 			m_Transform;
    Fvector				m_Vel;

    void __stdcall		OnItemModified		(void); 

    void __stdcall 		OnParticleItemFocused	(ListItemsVec& items);
	void __stdcall 		OnParticleItemRename	(LPCSTR old_name, LPCSTR new_name, EItemType type);
    void __stdcall	 	OnParticleItemRemove	(LPCSTR name, EItemType type, bool& res);

    void				RealUpdateProperties();
	void 				SelectListItem		(LPCSTR pref, LPCSTR name, bool bVal, bool bLeaveSel, bool bExpand);

    void				RealApplyParent		();
    void				ApplyParent			(bool bForce=false){m_Flags.set(flApplyParent,TRUE); if (bForce) RealApplyParent();}
    void				RealCompileEffect	();
    void				CompileEffect		(bool bForced=false){m_Flags.set(flCompileEffect,TRUE); if (bForced) RealCompileEffect();}
    u32					remove_action_num;
    void				RealRemoveAction	();

    void __stdcall		OnControlClick		(PropValue* sender, bool& bDataModified, bool& bSafe);
public:
    void				RemoveAction		(u32 idx, bool bForced=false){remove_action_num=idx;m_Flags.set(flRemoveAction,TRUE); if (bForced) RealRemoveAction();}
public:
	EEditMode			m_EditMode;
    
    TProperties*		m_ItemProps;
    TItemList*			m_PList;
public:
	// flags
    enum{
    	flRefreshProps 		= (1<<0),
        flApplyParent		= (1<<1),
        flCompileEffect		= (1<<2),
        flRemoveAction		= (1<<3),
        flAnimatedParent	= (1<<4),
        flAnimatedPath		= (1<<5),
        flSelectEffect		= (1<<6),
        flSetXFORM			= (1<<7),
    };
    Flags32				m_Flags;
protected:
	xr_string			sel_eff_name;

	void __stdcall		OnChangeMotion		(PropValue* sender);
	CObjectAnimator*	m_ParentAnimator;

    void 				PrepareLighting		();
public:
						CParticleTools		();
    virtual 			~CParticleTools		();

    virtual void		Render				();
	virtual void		RenderEnvironment	(){;}
    virtual void		OnFrame				();

    virtual bool		OnCreate			();
    virtual void		OnDestroy			();

    virtual bool		IfModified			();
    virtual bool		IsModified			(){return m_bModified;}
    virtual void		Modified			(); 

    virtual LPCSTR		GetInfo				();
    
    virtual void		ZoomObject			(BOOL bSelOnly);

    virtual bool		Load				(LPCSTR path, LPCSTR name);
    virtual bool		Save				(LPCSTR path, LPCSTR name, bool bInternal=false);
    virtual void		Reload				();
    
    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    virtual void		Clear				(){inherited::Clear();}

    virtual void		OnShowHint			(AStringVec& SS);

    virtual bool __fastcall 	MouseStart  		(TShiftState Shift);
    virtual bool __fastcall 	MouseEnd    		(TShiftState Shift);
    virtual void __fastcall 	MouseMove   		(TShiftState Shift);

    virtual bool		Pick				(TShiftState Shift){return false;}
	virtual bool 		RayPick				(const Fvector& start, const Fvector& dir, float& dist, Fvector* pt, Fvector* n);

    virtual void		ShowProperties		(){;}
    virtual void		UpdateProperties	(BOOL bForced=FALSE){m_Flags.set(flRefreshProps,TRUE); if (bForced) RealUpdateProperties();}
    virtual void		RefreshProperties	(){;}

    void				PlayCurrent			(int idx=-1);
    void				StopCurrent			(bool bFinishPlaying);
    void				SelectEffect		(LPCSTR name);

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

    void				SelectPreviewObject	(int p);
    void				ResetPreviewObject	();
    void				EditPreviewPrefs	();

    bool 				Validate			(bool bMsg);
    
    // commands
    CCommandVar			CommandSelectPreviewObj	(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandEditPreviewProps	(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandSave				(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandSaveBackup		(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandReload			(CCommandVar p1, CCommandVar p2);
    CCommandVar			CommandValidate			(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandClear			(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandPlayCurrent		(CCommandVar p1, CCommandVar p2);
	CCommandVar			CommandStopCurrent		(CCommandVar p1, CCommandVar p2);
};
#define SYSTEM_PREFIX 	"Systems"
#define EFFECT_PREFIX 	"Effects"
#define GROUP_PREFIX 	"Groups"
extern CParticleTools*&	PTools;
//---------------------------------------------------------------------------
#endif
