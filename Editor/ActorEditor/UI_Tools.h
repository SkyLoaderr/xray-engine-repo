//---------------------------------------------------------------------------
#ifndef UIToolsH
#define UIToolsH

#include "eltree.hpp"
// refs
class TfrmProperties;
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

class CActorTools: public pureDeviceCreate, public pureDeviceDestroy
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

    TElTreeItem*		m_pCycleNode;
    TElTreeItem*		m_pFXNode;
	void __fastcall		OnMotionTypeChange	(LPVOID data);

	void __fastcall 	FloatOnAfterEdit(LPVOID data);
	void __fastcall 	FloatOnBeforeEdit(LPVOID data);
	LPCSTR __fastcall 	FloatOnDraw(LPVOID data);
public:
    TfrmProperties*		m_Props;
public:
						CActorTools			();
    virtual 			~CActorTools		();

    void				Render				();
    void				Update				();

    bool				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			();
    bool				IsModified			(){return m_bModified;}
    void __fastcall		Modified			(void);

    CEditableObject*	CurrentObject		(){return m_EditObject;}
    void				SetCurrentMotion	(LPCSTR name);
    void				FillBaseProperties	();
	void				FillMotionProperties();
    void				PlayMotion			();
    void				StopMotion			();
    void				PauseMotion			();

    void				ZoomObject			();
    void				ChangeAction		(EAction action);

    bool				Load				(LPCSTR name);
    bool				Save				(LPCSTR name);
    bool				Export				(LPCSTR name);
    bool				LoadMotions			(LPCSTR name);
    bool				SaveMotions			(LPCSTR name);
    bool				AppendMotion		(LPCSTR name, LPCSTR fn);
    bool				RemoveMotion		(LPCSTR name);
    void				Reload				();
    void				ApplyChanges		();

    virtual void		OnDeviceCreate		();
    virtual void		OnDeviceDestroy		();

    void				Clear				();

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
extern CActorTools	Tools;
//---------------------------------------------------------------------------
#endif
