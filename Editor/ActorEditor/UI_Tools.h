//---------------------------------------------------------------------------
#ifndef UIToolsH
#define UIToolsH

#include "eltree.hpp"
#include "mxplacemnt.hpp"
// refs
class TfrmProperties;
class CEditableObject;
class PropValue;

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
    class PreviewModel{
	    TfrmProperties*	m_Props;
		CEditableObject*m_pObject;
        float			m_fSpeed;
        float			m_fSegment;
        DWORD			m_dwFlags;
        Fvector			m_vPosition;
        AnsiString 		m_LastObjectName;
    public:
    	enum{
        	pmScroll	= (1<<0),
        	force_dword = DWORD(-1)
        };
    public:
        				PreviewModel		(){m_pObject=0;m_fSpeed=5.f;m_fSegment=50.f;m_dwFlags=0;m_Props=0;m_vPosition.set(0,0,0);}
    	void			OnCreate			();
    	void			OnDestroy			();
    	void			Clear				();
        void			SelectObject		();
        void			SetPreferences		();
        void			Render				();
        void			Update				();
        void			RestoreParams		(TFormStorage* s);
        void			SaveParams			(TFormStorage* s);
    };

	CEditableObject*	m_pEditObject;
    class EngineModel{
    	CFS_Memory		m_GeometryStream;
	    CFS_Memory		m_MotionsStream;
	    bool			UpdateGeometryStream(CEditableObject* source);
    	bool			UpdateMotionsStream	(CEditableObject* source);
    public:
	    FBasicVisual*	m_pVisual;
    public:
        				EngineModel			(){m_pVisual=0;}
        void			DeleteVisual		(){Device.Models.Delete(m_pVisual);}
        void			Clear				(){DeleteVisual(); m_GeometryStream.clear();m_MotionsStream.clear();}
        bool 			UpdateVisual		(CEditableObject* source, bool bUpdGeom=false, bool bUpdMotions=false);
        bool			IsRenderable		(){return !!m_pVisual;}
    };
	EngineModel			m_RenderObject;

    bool				m_bObjectModified;
    bool				m_bMotionModified;
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
	void __fastcall		MotionOnAfterEdit	(PropValue* sender, LPVOID edit_val);

	void __fastcall 	FloatOnAfterEdit	(PropValue* sender, LPVOID edit_val);
	void __fastcall 	FloatOnBeforeEdit	(PropValue* sender, LPVOID edit_val);
	void __fastcall 	FloatOnDraw			(PropValue* sender, LPVOID draw_val);
public:
    TfrmProperties*		m_ObjectProps;
    TfrmProperties*		m_MotionProps;
    PreviewModel		m_PreviewObject;
public:
						CActorTools			();
    virtual 			~CActorTools		();

    void				Render				();
    void				Update				();

    bool				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			();
    bool				IsModified			(){return m_bObjectModified||m_bMotionModified;}
    bool				IsObjectModified	(){return m_bObjectModified;}
    bool				IsMotionModified	(){return m_bMotionModified;}
    void __fastcall		ObjectModified		(void);
    void __fastcall		MotionModified		(void);

    bool				IsVisualPresent		(){return m_RenderObject.IsRenderable();}

    CEditableObject*	CurrentObject		(){return m_pEditObject;}
    void				SetCurrentMotion	(LPCSTR name);
    void				FillObjectProperties();
	void				FillMotionProperties();
    void				PlayMotion			();
    void				StopMotion			();
    void				PauseMotion			();
    bool				RenameMotion		(LPCSTR old_name, LPCSTR new_name);

    void				ZoomObject			();
    void				ChangeAction		(EAction action);
    void				MakePreview			();

    bool				Load				(LPCSTR name);
    bool				Save				(LPCSTR name);
    bool				Export				(LPCSTR name);
    bool				LoadMotions			(LPCSTR name);
    bool				SaveMotions			(LPCSTR name);
    bool				AppendMotion		(LPCSTR name, LPCSTR fn);
    bool				RemoveMotion		(LPCSTR name);
    void				Reload				();

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

    void 				SelectPreviewObject	(bool bClear);
    void				SetPreviewObjectPrefs();
};
extern CActorTools	Tools;
//---------------------------------------------------------------------------
#endif
