//---------------------------------------------------------------------------
#ifndef UI_MainH
#define UI_MainH

#include "RenderWindow.hpp"
#include "device.h"
#include "UI_MainCommand.h"
#include "ColorPicker.h"
#include "FController.h"

// refs
class CCustomObject;
class TUI_Tools;
class TUI_Tools;
class C3DCursor;
//------------------------------------------------------------------------------

enum EEditorState{
	esNone,
	esSceneLocked,
    esEditScene,
    esEditLibrary,
    esEditImages,
    esBuildLevel
};

typedef vector<EEditorState> EStateList;
typedef EStateList::iterator EStateIt;

class TUI: public CController{
    friend class TfrmEditorPreferences;
    friend class CRenderDevice;
	char m_LastFileName[MAX_PATH];
    TD3DWindow* m_D3DWindow;

    TShiftState m_ShiftState;

    bool m_bAppActive;
protected:
    EStateList m_EditorState;
    bool bNeedAbort;
public:
	bool m_bReady;
    TD3DWindow* GetD3DWindow(){return m_D3DWindow;}
protected:
	float m_AngleSnap;
	float m_MoveSnap;
	Fvector m_Pivot;
protected:
	bool m_SelectionRect;
	Ipoint m_SelStart;
	Ipoint m_SelEnd;
protected:
	bool bRedraw;
    bool bUpdateScene;
	bool bResize;
protected:
	long m_StartTime;

    void Redraw();
    void RealUpdateScene();
protected:
    void OutUICursorPos();
	void OutGridSize();

    void D3D_CreateStateBlocks();
    void D3D_DestroyStateBlocks();
public:
	// non-hidden ops
	Ipoint m_StartCp;
	Ipoint m_CurrentCp;

	Fvector m_StartRStart;
	Fvector m_StartRNorm;

	Fvector m_CurrentRStart;
	Fvector m_CurrentRNorm;

	// hidden ops
	Ipoint m_StartCpH;
	Ipoint m_DeltaCpH;

#ifdef _LEVEL_EDITOR
    C3DCursor*   m_Cursor;
#endif
protected:
	bool m_MouseCaptured;
	bool m_MouseMultiClickCaptured;
	bool bMouseInUse;

	THintWindow* m_pHintWindow;
	AnsiString m_LastHint;
    bool  m_bHintShowing;
    POINT m_HintPoint;
public:
    void ShowObjectHint();
    bool ShowHint(const AStringVec& SS);
    void HideHint();
public:
    // mouse sensetive
    float m_MouseSM, m_MouseSS, m_MouseSR;
public:
    TUI();
    virtual ~TUI();
    IC HANDLE GetHWND()					{   return m_D3DWindow->Handle; }
    int GetRenderWidth()                {   return Device.dwWidth; }
    int GetRenderHeight()               {   return Device.dwHeight; }
    int GetRealWidth()            	    {   return Device.m_RealWidth; }
    int GetRealHeight()             	{   return Device.m_RealHeight; }
	__inline Fvector& pivot()           {	return m_Pivot; }
	__inline float anglesnap()          {	return m_AngleSnap; }
	__inline float movesnap()           {	return m_MoveSnap; }

    IC float ZFar()						{	return Device.m_Camera.m_Zfar; }

    bool OnCreate						();
    void OnDestroy						();

    char* GetEditFileName()             {   return m_LastFileName; }
    char* GetCaption();
    char* GetTitle();

    bool IsModified						();

    void __fastcall Idle();
    void Resize()                       {   bResize = true; bRedraw = true; }
    // add, remove, changing objects/scene
    void UpdateScene(bool bForced=false){	bUpdateScene = true; if (bForced) Idle();}
    // only redraw scene
    void RedrawScene(bool bForced=false){   bRedraw = true; if (bForced) Idle();}

    void SetRenderQuality(float q)      {   Device.m_ScreenQuality = q;}
// mouse action
    void EnableSelectionRect	( bool flag );
    void UpdateSelectionRect	( const Ipoint& from, const Ipoint& to );
//	bool MouseBox( ICullPlane *planes, const Fvector2& point1, const Fvector2& point2 );
	bool PickGround				(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap=1, Fvector* hitnormal=0);
    bool SelectionFrustum		(CFrustum& frustum);

    void MouseMultiClickCapture (bool b){m_MouseMultiClickCaptured = b;}

    bool __fastcall IsMouseCaptured(){return m_MouseCaptured|m_MouseMultiClickCaptured;}
    bool __fastcall IsMouseInUse(){return bMouseInUse;}

    bool __fastcall KeyDown     (WORD Key, TShiftState Shift);
    bool __fastcall KeyUp       (WORD Key, TShiftState Shift);
    bool __fastcall KeyPress    (WORD Key, TShiftState Shift);
	void __fastcall MousePress	(TShiftState Shift, int X, int Y);
	void __fastcall MouseRelease(TShiftState Shift, int X, int Y);
	void __fastcall MouseMove	(TShiftState Shift, int X, int Y);

    bool Command( int _Command, int p = 0, int p2 = 0 );

    void BeginEState(EEditorState st){ m_EditorState.push_back(st); }
    void EndEState(){ m_EditorState.pop_back(); }
    void EndEState(EEditorState st){
    	VERIFY(find(m_EditorState.begin(),m_EditorState.end(),st)!=m_EditorState.end());
        for (EStateIt it=m_EditorState.end()-1; it>=m_EditorState.begin(); it--)
        	if (*it==st){
            	m_EditorState.erase(it,m_EditorState.end());
            	break;
            }
    }
    EEditorState GetEState(){ return m_EditorState.back(); }
    bool ContainEState(EEditorState st){ return find(m_EditorState.begin(),m_EditorState.end(),st)!=m_EditorState.end(); }

	void ProgressStart(float max_val, const char* text);
	void ProgressEnd();
	void ProgressUpdate(float val);
    void ProgressInc();

    void OutCameraPos();
    void SetStatus(LPSTR s);

	// direct input
//	virtual void OnMousePress			(int btn);
//	virtual void OnMouseRelease			(int btn);
	virtual void OnMouseMove			(int x, int y);

    void OnAppActivate					();
    void OnAppDeactivate                ();

    bool    NeedAbort               	(){Application->ProcessMessages(); return bNeedAbort;}
    void 	NeedBreak					(){bNeedAbort = true;}
    void 	ResetBreak					(){bNeedAbort = false;}

    void 	ApplyShortCut				(WORD Key, TShiftState Shift);
    void 	ApplyGlobalShortCut			(WORD Key, TShiftState Shift);

	void 	ShowContextMenu				(int cls);

    void	SetGradient					(DWORD color){;}
};
//---------------------------------------------------------------------------
extern TUI UI;
//---------------------------------------------------------------------------
void ResetActionToSelect();
//---------------------------------------------------------------------------
#endif
