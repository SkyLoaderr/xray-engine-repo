//---------------------------------------------------------------------------
#ifndef UI_ToolsH
#define UI_ToolsH

#include "eltree.hpp"
#include "mxplacemnt.hpp"
#include "BodyInstance.h"
// refs
class TProperties;
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

// refs
class CSMotion;

class CActorTools: public pureDeviceCreate, public pureDeviceDestroy
{
    class PreviewModel{
	    TProperties*	m_Props;
		CEditableObject*m_pObject;
        float			m_fSpeed;
        float			m_fSegment;
        Flags32			m_Flags;
        Fvector			m_vPosition;
        AnsiString 		m_LastObjectName;
    public:
    	enum{
        	pmScroll	= (1<<0),
        	force_dword = u32(-1)
        };
        enum EScrollAxis{
        	saZp,
            saZn,
            saXp,
            saXn,
            saforceDWORD=(-1)
        };
        EScrollAxis		m_ScrollAxis;
    public:
        				PreviewModel		(){m_pObject=0;m_fSpeed=5.f;m_fSegment=50.f;m_Flags.zero();m_Props=0;m_vPosition.set(0,0,0);m_ScrollAxis=saZp;}
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
    	CMemoryWriter	m_GeometryStream;
	    CMemoryWriter	m_MotionKeysStream;
	    CMemoryWriter	m_MotionDefsStream;
	    bool			UpdateGeometryStream	(CEditableObject* source);
    	bool			UpdateMotionKeysStream	(CEditableObject* source);
    	bool			UpdateMotionDefsStream	(CEditableObject* source);
    public:
    	float			m_fLOD;
	    IVisual*		m_pVisual;
        CBlend*			m_pBlend;
        AnsiString		m_BPPlayCache[MAX_PARTS];
    public:
        				EngineModel			(){m_pVisual=0;m_fLOD=1.f;m_pBlend=0;}
        void			DeleteVisual		(){Device.Models.Delete(m_pVisual);m_pBlend=0;}
        void			ClearCache			()
        {
            for (int k=0; k<MAX_PARTS; k++) m_BPPlayCache[k]="";
        }
        void			Clear				()
        {
        	ClearCache				();
        	DeleteVisual			(); 
            m_GeometryStream.clear	();
            m_MotionKeysStream.clear();
            m_MotionDefsStream.clear();
        }
        bool 			UpdateVisual		(CEditableObject* source, bool bUpdGeom, bool bUpdKeys, bool bUpdDefs);
        bool			IsRenderable		(){return !!m_pVisual;}
        void			Render				(const Fmatrix& mTransform);
        void			PlayMotion			(CSMotion* motion);
    };

    bool				m_bObjectModified;
    bool				m_bMotionModified;
    bool				m_bReady;
    bool				m_bNeedUpdateMotionKeys;
    bool				m_bNeedUpdateMotionDefs;

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
	void __fastcall		MotionOnChange			(PropValue* sender);

	void __fastcall 	OnChangeTransform		(PropValue* sender);
	void __fastcall 	BPOnAfterEdit			(PropValue* sender, LPVOID edit_val);
	void __fastcall 	BPOnBeforeEdit			(PropValue* sender, LPVOID edit_val);
	void __fastcall 	BPOnDraw				(PropValue* sender, LPVOID draw_val);
	void __fastcall 	OnMotionNameChange		(PropValue* sender);
public:
	EngineModel			m_RenderObject;
    PreviewModel		m_PreviewObject;

    TProperties*		m_ObjectProps;
    TProperties*		m_MotionProps;

    TfrmKeyBar* 		m_KeyBar;

    bool				m_bNeedRefreshShaders;
    void				RefreshShaders		(){m_bNeedRefreshShaders=true;}
public:
						CActorTools			();
    virtual 			~CActorTools		();

    void				Render				();
	void				RenderEnvironment	(){;}
    void				OnFrame				();

    bool				OnCreate			();
    void				OnDestroy			();

    bool				IfModified			();
    bool				IsModified			(){return m_bObjectModified||m_bMotionModified;}
    bool				IsObjectModified	(){return m_bObjectModified;}
    bool				IsMotionModified	(){return m_bMotionModified;}
    void __fastcall		OnObjectModified	(void);
    void __fastcall		OnMotionDefsModified(void); 
    void 				OnMotionKeysModified(void); 

    bool				IsVisualPresent		(){return m_RenderObject.IsRenderable();}

    CEditableObject*	CurrentObject		(){return m_pEditObject;}
    void				SetCurrentMotion	(LPCSTR name);
    CSMotion*			GetCurrentMotion	();
    void				FillObjectProperties();
	void				FillMotionProperties();
    void				PlayMotion			();
    void				StopMotion			();
    void				PauseMotion			();
    bool				RenameMotion		(LPCSTR old_name, LPCSTR new_name);

    void				GetCurrentFog		(u32& fog_color, float& s_fog, float& e_fog);
    LPCSTR				GetInfo				();
    
    void				ZoomObject			(bool bSelOnly);
    void				ChangeAction		(EAction action);
    void				MakePreview			();

	void				SetNumPosition		(CCustomObject* p1){;}
	void				SetNumRotation		(CCustomObject* p1){;}
	void				SetNumScale			(CCustomObject* p1){;}

    bool				Load				(LPCSTR name);
    bool				Save				(LPCSTR name);
    bool				ExportSkeleton		(LPCSTR name);
    bool				ExportObject		(LPCSTR name);
    bool				LoadMotions			(LPCSTR name);
    bool				SaveMotions			(LPCSTR name);
    bool				AppendMotion		(LPCSTR name, LPCSTR fn);
    bool				RemoveMotion		(LPCSTR name);
    void				Reload				();
    void 				WorldMotionRotate	(const Fvector& R);

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

    void				ShowProperties		(){;}
    void				UpdateProperties	(){;}
    void				RefreshProperties	(){;}
    
	void				GetStatTime			(float& a, float& b, float& c);
};
extern CActorTools	Tools;
//---------------------------------------------------------------------------
#endif
