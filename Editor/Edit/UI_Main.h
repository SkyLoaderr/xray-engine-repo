//---------------------------------------------------------------------------
#ifndef UI_MainH
#define UI_MainH

#include "RenderWindow.hpp"
#include "ui_tools.h"
#include "device.h"
#include "FileSystem.h"
#include "UI_MainCommand.h"
#include "ColorPicker.h"
extern bool g_bEditorValid;


// refs
class SceneObject;
class TUI_Tools;
class TUI_Tools;
class C3DCursor;
//class CTexturizer;
//------------------------------------------------------------------------------

enum EEditorState{
	esNone,
	esSceneLocked,
    esEditScene,
    esEditLibrary,
    esEditShaders,
    esEditParticles,
    esEditImages,
    esBuildLevel
};

typedef vector<EEditorState> EStateList;
typedef EStateList::iterator EStateIt;

class TUI{
    friend class TfrmEditorPreferences;
	char m_LastFileName[MAX_PATH];
    TD3DWindow* m_D3DWindow;
protected:
    EStateList m_EditorState;
public:
    TD3DWindow* GetD3DWindow(){return m_D3DWindow;}
protected:
	float m_AngleSnap;
	float m_MoveSnap;
	Fvector m_Pivot;
protected:
	bool m_SelectionRect;
	Fvector2 m_SelStart;
	Fvector2 m_SelEnd;
protected:
	bool bRedraw;
    bool bUpdateScene;
	bool bResize;
public:
	CDevice Device;
protected:
	long m_StartTime;

	FLvertexVec m_GridPoints;
	WORDVec m_GridIndices;

	void D3D_DrawSelectionRect();
	void D3D_UpdateGrid();
	void D3D_DrawGrid();
	void D3D_DrawPivot();
    void D3D_DrawAxis();

    void Redraw();
    void RealUpdateScene();
protected:
    void OutUICursorPos();

    void D3D_CreateStateBlocks();
    void D3D_DestroyStateBlocks();
public:
    TUI_Tools*          m_Tools;
//    CTexturizer*        m_pTexturizer;

	// non-hidden ops
	Fvector2 m_StartCp;
	Fvector2 m_CurrentCp;

	Fvector m_StartRStart;
	Fvector m_StartRNorm;

	Fvector m_CurrentRStart;
	Fvector m_CurrentRNorm;

	// hidden ops
	POINT m_CenterCpH;
	POINT m_StartCpH;
	POINT m_DeltaCpH;

    C3DCursor*   m_Cursor;
protected:
	bool m_MouseCaptured;
	bool m_MouseMultiClickCaptured;
	bool bMouseInUse;

    float fHintPause;
    float fHintPauseTime;
    float fHintHide;
    float fHintHideTime;
    bool  bHintShowing;
public:
    void ShowObjectHint();
    bool ShowHint(const AStringVec& SS);
    void HideHint();
public:
    // mouse sensetive
    float m_MouseSM, m_MouseSS, m_MouseSR;
public:
	DWORD dwRenderFillMode;
	DWORD dwRenderShadeMode;
	DWORD dwRenderHWTransform;
    bool  bRenderTextures;
    bool  bRenderLights;
    bool  bRenderFilter;
    bool  bRenderEdgedFaces;
    bool  bRenderRealTime;
    bool  bRenderFog;
    bool  bDrawGrid;
public:
    TUI();
    virtual ~TUI();
    IC HANDLE GetHWND()					{   return m_D3DWindow->Handle; }
    int GetRenderWidth()                {   return Device.m_RenderWidth; }
    int GetRenderHeight()               {   return Device.m_RenderHeight; }
    int GetRealWidth()            	    {   return Device.m_RealWidth; }
    int GetRealHeight()             	{   return Device.m_RealHeight; }
	__inline Fvector& pivot()           {	return m_Pivot; }
	__inline float anglesnap()          {	return m_AngleSnap; }
	__inline float movesnap()           {	return m_MoveSnap; }

    IC float ZFar()						{	return Device.m_Camera.m_Zfar; }

    bool Init(TD3DWindow* wnd);
    void Clear();

    char* GetEditFileName()             {   return m_LastFileName; }

    void __fastcall Idle();
    void Resize()                       {   bResize = true; bRedraw = true; }
    // add, remove, changing objects/scene
    void UpdateScene(bool bForced=false){	bUpdateScene = true; if (bForced) Idle();}
    // only redraw scene
    void RedrawScene(bool bForced=false){   bRedraw = true; if (bForced) Idle();}

    void SetRenderQuality(float q)      {   Device.m_ScreenQuality = q;}
// mouse action
    void EnableSelectionRect	( bool flag );
    void UpdateSelectionRect	( const Fvector2& from, const Fvector2& to );
//	bool MouseBox( ICullPlane *planes, const Fvector2& point1, const Fvector2& point2 );
	bool PickGround				(Fvector& hitpoint, const Fvector& start, const Fvector& direction, int bSnap=1);
    bool SelectionFrustum		(CFrustum& frustum);

    void __fastcall MouseStart  (TShiftState Shift, int X, int Y);
    void __fastcall MouseEnd    (TShiftState Shift, int X, int Y);
    void __fastcall MouseProcess(TShiftState Shift, int X, int Y);
    void MouseMultiClickCapture (bool b){m_MouseMultiClickCaptured = b;}

    bool __fastcall IsMouseCaptured(){return m_MouseCaptured|m_MouseMultiClickCaptured;}
    bool __fastcall IsMouseInUse(){return bMouseInUse;}

    bool __fastcall KeyDown     (WORD Key, TShiftState Shift);
    bool __fastcall KeyUp       (WORD Key, TShiftState Shift);
    bool __fastcall KeyPress    (WORD Key, TShiftState Shift);

    bool Command( int _Command, int p = 0 );

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

    EObjClass CurrentClassID();
};
extern TUI* UI;
#endif
