//---------------------------------------------------------------------------
#ifndef UI_MainH
#define UI_MainH

#include "RenderWindow.hpp"
#include "UI_MainCommand.h"
#include "IInputReceiver.h"

// refs
class CCustomObject;
class TUI_Tools;
class TUI_Tools;
class C3DCursor;
//------------------------------------------------------------------------------

enum EEditorState{
	esNone,
    esEditScene,
    esEditLibrary,
    esEditLightAnim,
    esBuildLevel
};

typedef xr_vector<EEditorState> EStateList;
typedef EStateList::iterator EStateIt;

class ECORE_API TUI: public IInputReceiver{
protected:
    friend class CEditorPreferences;
    friend class CRenderDevice;
	AnsiString 	m_LastFileName;
    TD3DWindow* m_D3DWindow;
    TPanel*		m_D3DPanel;

    TShiftState m_ShiftState;

    bool m_bAppActive;
protected:
    EStateList m_EditorState;
    bool bNeedAbort;
public:
	bool m_bReady;
    TD3DWindow* GetD3DWindow(){return m_D3DWindow;}
protected:
	Fvector m_Pivot;
protected:
	bool m_SelectionRect;
	Ivector2 m_SelStart;
	Ivector2 m_SelEnd;
protected:
	enum{
    	flRedraw		= (1<<0),
        flUpdateScene	= (1<<1),
        flResize		= (1<<2),
        flNeedQuit		= (1<<3),
    };
	Flags32 m_Flags;
protected:
	long m_StartTime;

    void Redraw();
protected:
    virtual void RealUpdateScene()=0;

    virtual void OutUICursorPos	()=0;
	virtual void OutGridSize	()=0;
	virtual void OutInfo		()=0;

    void D3D_CreateStateBlocks();
    void D3D_DestroyStateBlocks();
public:
	// non-hidden ops
	Ivector2 m_StartCp;
	Ivector2 m_CurrentCp;

	Fvector m_CurrentRStart;
	Fvector m_CurrentRNorm;

	// hidden ops
	Ivector2 m_StartCpH;
	Ivector2 m_DeltaCpH;
protected:
	bool m_MouseCaptured;
	bool m_MouseMultiClickCaptured;
	bool bMouseInUse;

	THintWindow* m_pHintWindow;
	AnsiString m_LastHint;
    bool  m_bHintShowing;
    POINT m_HintPoint;

    // mailslot
    HANDLE			hMailSlot;
public:
    void ShowObjectHint();
    void ShowHint(const AnsiString& s);
    bool ShowHint(const AStringVec& SS);
    void HideHint();
public:
    // mouse sensetive
    float m_MouseSM, m_MouseSS, m_MouseSR;
protected:
    virtual bool	CommandExt				(int _Command, int p = 0, int p2 = 0)=0;
    virtual bool 	ApplyShortCutExt		(WORD Key, TShiftState Shift)=0;
    virtual bool 	ApplyGlobalShortCutExt	(WORD Key, TShiftState Shift)=0;

    virtual void	RealQuit				()=0;
public:
    				TUI				();
    virtual 		~TUI			();

    void			Quit			()	{	m_Flags.set(flNeedQuit,TRUE); }

    IC HANDLE 		GetHWND			()	{   return m_D3DWindow->Handle; }
    int 			GetRenderWidth	()	{   return Device.dwWidth; }
    int 			GetRenderHeight	()	{   return Device.dwHeight; }
    int 			GetRealWidth	()	{   return Device.m_RealWidth; }
    int 			GetRealHeight	()  {   return Device.m_RealHeight; }

    IC float 		ZFar			()	{	return Device.m_Camera.m_Zfar; }
    IC TShiftState	GetShiftState 	()	{	return m_ShiftState; }

    virtual bool 	OnCreate		(TD3DWindow* w, TPanel* p);
    virtual void 	OnDestroy		();

    const AnsiString&	GetEditFileName	()	{ return m_LastFileName; }
    virtual char* 		GetCaption		()=0;

    bool 			IsModified		();

    void __fastcall Idle			();
    void 			Resize				(bool bForced=false){   m_Flags.set(flResize|flRedraw,TRUE); if (bForced) Idle(); }
    // add, remove, changing objects/scene
    void 			UpdateScene			(bool bForced=false){	m_Flags.set(flUpdateScene,TRUE); if (bForced) Idle();}
    // only redraw scene
    void 			RedrawScene			(bool bForced=false){   m_Flags.set(flRedraw,TRUE); if (bForced) Idle();}

    void 			SetRenderQuality	(float q)      {   Device.m_ScreenQuality = q;}
// mouse action
    void 			EnableSelectionRect	(bool flag );
    void 			UpdateSelectionRect	(const Ivector2& from, const Ivector2& to );

    void 			MouseMultiClickCapture(bool b){m_MouseMultiClickCaptured = b;}

    bool __fastcall IsMouseCaptured		()	{	return m_MouseCaptured|m_MouseMultiClickCaptured;}
    bool __fastcall IsMouseInUse		()	{	return bMouseInUse;}

    bool __fastcall KeyDown     		(WORD Key, TShiftState Shift);
    bool __fastcall KeyUp       		(WORD Key, TShiftState Shift);
    bool __fastcall KeyPress    		(WORD Key, TShiftState Shift);
	void __fastcall MousePress			(TShiftState Shift, int X, int Y);
	void __fastcall MouseRelease		(TShiftState Shift, int X, int Y);
	void __fastcall MouseMove			(TShiftState Shift, int X, int Y);

    bool 			Command				(int _Command, int p = 0, int p2 = 0);

    void 			BeginEState			(EEditorState st){ m_EditorState.push_back(st); }
    void 			EndEState			(){ m_EditorState.pop_back(); }
    void 			EndEState			(EEditorState st){
    	VERIFY(std::find(m_EditorState.begin(),m_EditorState.end(),st)!=m_EditorState.end());
        for (EStateIt it=m_EditorState.end()-1; it>=m_EditorState.begin(); it--)
        	if (*it==st){
            	m_EditorState.erase(it,m_EditorState.end());
            	break;
            }
    }
    EEditorState 	GetEState			(){ return m_EditorState.back(); }
    bool 			ContainEState		(EEditorState st){ return std::find(m_EditorState.begin(),m_EditorState.end(),st)!=m_EditorState.end(); }

	virtual void 	ProgressInfo		(const char* text, bool bWarn=false)=0;
	virtual void 	ProgressStart		(float max_val, const char* text)=0;
	virtual void 	ProgressEnd			()=0;
	virtual void 	ProgressUpdate		(float val)=0;
    virtual void 	ProgressInc			(const char* info=0, bool bWarn=false)=0;

    virtual void 	OutCameraPos		()=0;
    virtual void 	SetStatus			(LPSTR s, bool bOutLog=true)=0;
    virtual void 	ResetStatus			()=0;
    
	// direct input
	virtual void 	IR_OnMouseMove		(int x, int y);

    void 			OnAppActivate		();
    void 			OnAppDeactivate     ();

    bool    		NeedAbort           (){Application->ProcessMessages(); return bNeedAbort;}
    void 			NeedBreak			(){bNeedAbort = true;}
    void 			ResetBreak			(){bNeedAbort = false;}

    bool 			ApplyShortCut		(WORD Key, TShiftState Shift);
    bool 			ApplyGlobalShortCut	(WORD Key, TShiftState Shift);

    void			SetGradient			(u32 color){;}

    void 			OnDeviceCreate		();
    void			OnDeviceDestroy		();

    // mailslot
	bool 			CreateMailslot		();
	void 			CheckMailslot		();
	void 			OnReceiveMail		(LPCSTR msg);
//	void 			SendMail			();

    void			CheckWindowPos		(TForm* form);

    virtual LPCSTR 	EditorName			()=0;
    virtual LPCSTR	EditorDesc			()=0;
};
//---------------------------------------------------------------------------
extern ECORE_API TUI* UI;  
//---------------------------------------------------------------------------
void ResetActionToSelect();
#define COMMAND0(cmd)		{Command(cmd);bExec=true;}
#define COMMAND1(cmd,p0)	{Command(cmd,p0);bExec=true;}
extern ECORE_API void __fastcall PanelMinMax		(TPanel *pa);
extern ECORE_API void __fastcall PanelMinimize		(TPanel *pa);
extern ECORE_API void __fastcall PanelMaximize		(TPanel *pa);
extern ECORE_API void __fastcall PanelMinMaxClick	(TObject *sender);
extern ECORE_API void __fastcall PanelMinimizeClick	(TObject *sender);
extern ECORE_API void __fastcall PanelMaximizeClick	(TObject *sender);
//---------------------------------------------------------------------------
#endif
