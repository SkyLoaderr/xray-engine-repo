//---------------------------------------------------------------------------
#ifndef UI_MainH
#define UI_MainH

#include "RenderWindow.hpp"
#include "device.h"
#include "FileSystem.h"
#include "UI_MainCommand.h"
#include "ColorPicker.h"
#include "FController.h"
extern bool g_bEditorValid;


// refs
class CCustomObject;
class TUI_Tools;
class TUI_Tools;
class CActorTools;
//class CTexturizer;
//------------------------------------------------------------------------------
class TUI: public CController{
    friend class TfrmEditorPreferences;
    friend class CRenderDevice;
	char m_LastFileName[MAX_PATH];
    TD3DWindow* m_D3DWindow;

    TShiftState m_ShiftState;
public:
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
protected:
    void OutUICursorPos();
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
protected:
	bool m_MouseCaptured;
	bool m_MouseMultiClickCaptured;
	bool bMouseInUse;
public:
    CActorTools*	m_ActorTools;
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
    void UpdateSelectionRect	( const Ipoint& from, const Ipoint& to );

    void MouseMultiClickCapture (bool b){m_MouseMultiClickCaptured = b;}

    bool __fastcall IsMouseCaptured(){return m_MouseCaptured|m_MouseMultiClickCaptured;}
    bool __fastcall IsMouseInUse(){return bMouseInUse;}

    bool __fastcall KeyDown     (WORD Key, TShiftState Shift);
    bool __fastcall KeyUp       (WORD Key, TShiftState Shift);
    bool __fastcall KeyPress    (WORD Key, TShiftState Shift);

    bool Command( int _Command, int p = 0 );

	void ProgressStart(float max_val, const char* text);
	void ProgressEnd();
	void ProgressUpdate(float val);
    void ProgressInc();

    void OutCameraPos();
    void SetStatus(LPSTR s);

	// direct input
	void OnMousePress					(TShiftState State);
	virtual void OnMouseRelease			(int btn);
	virtual void OnMouseMove			(int x, int y);

    void OnAppActivate					(){;}
    void OnAppDeactivate                (){;}
};
extern TUI* UI;
#endif
