//---------------------------------------------------------------------------
#ifndef UIToolsH
#define UIToolsH

#include "eltree.hpp"
#include "mxplacemnt.hpp"
// refs
class TfrmProperties;
class CEditableObject;
class PropValue;
class TfrmKeyBar;
class CBlend;

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
        enum EScrollAxis{
        	saZp,
            saZn,
            saforceDWORD=(-1)
        };
        EScrollAxis		m_ScrollAxis;
    public:
        				PreviewModel		(){m_pObject=0;m_fSpeed=5.f;m_fSegment=50.f;m_dwFlags=0;m_Props=0;m_vPosition.set(0,0,0);m_ScrollAxis=saZp;}
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
    	float			m_fLOD;
	    CVisual*		m_pVisual;
        CBlend*			m_pBlend;
    public:
        				EngineModel			(){m_pVisual=0;m_fLOD=1.f;m_pBlend=0;}
        void			DeleteVisual		(){Device.Models.Delete(m_pVisual);m_pBlend=0;}
        void			Clear				(){DeleteVisual(); m_GeometryStream.clear();m_MotionsStream.clear();}
        bool 			UpdateVisual		(CEditableObject* source, bool bUpdGeom=false, bool bUpdMotions=false);
        bool			IsRenderable		(){return !!m_pVisual;}
        void			Render				(const Fmatrix& mTransform);
    };

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
	void __fastcall		MotionOnAfterEdit	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);

	void __fastcall 	RotateOnAfterEdit	 	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	RotateOnBeforeEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	RotateOnDraw			(PropValue* sender, LPVOID draw_val);
	void __fastcall 	OnAfterTransformation	(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	OnAfterShaderEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	OnAfterTextureEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	NameOnAfterEdit			(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	NameOnBeforeEdit		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
	void __fastcall 	NameOnDraw				(PropValue* sender, LPVOID draw_val);
public:
	EngineModel			m_RenderObject;
    PreviewModel		m_PreviewObject;

    TfrmProperties*		m_ObjectProps;
    TfrmProperties*		m_MotionProps;

    TfrmKeyBar* 		m_KeyBar;
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

	void				GetStatTime			(float& a, float& b, float& c);
};
extern CActorTools	Tools;
//---------------------------------------------------------------------------
#endif
