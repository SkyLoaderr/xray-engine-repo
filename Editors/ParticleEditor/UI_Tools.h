//---------------------------------------------------------------------------
#ifndef UIToolsH
#define UIToolsH

#include "ParticleSystem.h"

// refs
class CPSObject;
class SDef;
class TfrmPropertiesPSDef;
class CEditableObject;

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
public:
						CParticleTools		();
    virtual 			~CParticleTools		();

    void				Render				();
    void				OnFrame				();

    bool				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			();
    bool				IsModified			(){return m_bModified;}
    void				Modified			();

    void				ZoomObject			(BOOL bObjectOnly);
    void				ChangeAction		(EAction action);

    PS::SDef*			AppendPS			(LPCSTR folder_name, PS::SDef* src);
    void				RemovePS			(LPCSTR name);
    void				ResetCurrentPS		();
    void				SetCurrentPS		(LPCSTR name);
    void 				SetCurrentPS		(PS::SDef* P);
    PS::SDef*			ClonePS				(LPCSTR name);
    void				RenamePS			(LPCSTR src_name, LPCSTR part_name, int part_idx);
    void				RenamePS			(LPCSTR src_name, LPCSTR dest_name);
    PS::SDef*			GetCurrentPS		(){return m_LibPS?&m_EditPS:0;}
    void				UpdateCurrent		();
    void				UpdateEmitter		();

    void				PlayCurrentPS		();
    void				StopCurrentPS		();

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

    bool				Pick				(){return false;}
};
extern CParticleTools	Tools;
//---------------------------------------------------------------------------
#endif
