//---------------------------------------------------------------------------
#ifndef UI_MainH
#define UI_MainH

//---------------------------------------------------------------------------
struct strDX{
	LPDIRECT3DDEVICE7		pD3DDev;
	LPDIRECTDRAW7			pDD;
	LPDIRECTDRAWSURFACE7	pBackBuffer;
	DDSURFACEDESC2			BackDesc;
};
extern "C" __declspec(dllimport) HRESULT WINAPI ReleaseD3DX();
extern "C" __declspec(dllimport) HRESULT WINAPI InitD3DX(HWND hWnd, strDX** dx);
extern "C" __declspec(dllimport) HRESULT WINAPI BeginDraw();
extern "C" __declspec(dllimport) HRESULT WINAPI EndDraw();
extern "C" __declspec(dllimport) HRESULT WINAPI ResizeD3DX(int X, int Y);
//------------------------------------------------------------------------------

#define COMMAND_EXIT        0
#define COMMAND_HIDE        1
#define COMMAND_CONSOLE     2
#define COMMAND_CLEAR       3
#define COMMAND_LOAD        4
#define COMMAND_SAVE        5
#define COMMAND_SAVEAS      6
#define COMMAND_BUILD       7
#define COMMAND_OPTIONS     8
#define COMMAND_MENU        9
#define COMMAND_ENTER_PIVOT 10
#define COMMAND_UPDATE_ALL  11

#define COMMAND_CUT         14
#define COMMAND_COPY        15
#define COMMAND_PASTE       16
#define COMMAND_UNDO        17
#define COMMAND_REDO        18

#define COMMAND_INVERT_SELECTION_ALL    19
#define COMMAND_SELECT_ALL              20
#define COMMAND_DESELECT_ALL            21

#define COMMAND_DELETE_SELECTION        22
#define COMMAND_SHOWPROPERTIES          23
#define COMMAND_HIDE_SELECTION          24
//------------------------------------------------------------------------------

// refs
class SceneObject;
class TUI_Tools;
class CFrustumClipper;
class CStatistic;
class TUI_Tools;
//------------------------------------------------------------------------------

class TUI{
	char m_LastFileName[MAX_PATH];
    TPanel* paRender;
    TPaintBox* ppRender;
public:
    TPaintBox*  GetPPRender(){return ppRender;}
    TPanel*     GetPARender(){return paRender;}
protected:
	float m_AngleSnap;
	float m_MoveSnap;
	Fvector m_PivotSnap;
	Fvector m_Pivot;

	FPcolor m_ColorFore_S;
	FPcolor m_ColorFore_B;
	FPcolor m_ColorBack_S;

	FPcolor m_ColorGrid;
	FPcolor m_ColorGridTh;
	FPcolor m_BackgroundColor;

	IGroundPoint m_GridCenter;
	IGroundPoint m_GridStep;
	int m_GridSubDiv[2];
	int m_GridCounts[2];

protected:
	bool m_SelectionRect;
	POINT m_SelStart;
	POINT m_SelEnd;
protected:
	bool m_SelectionChanged;
	bool m_VisibilityChanged;
	bool m_D3D_ok;
	bool bRedraw;
	bool bResize;

	Fvector m_LastPick;
	Fvector m_LastStart;
	Fvector m_LastDir;

public:
	Fmatrix m_Identity;
	Fmatrix m_Camera;
	Fmatrix m_Projection;
protected:
	float m_Aspect;
	float m_FOV;
	float m_Znear;
	float m_Zfar;
	long m_StartTime;
    int m_RenderWidth, m_RenderHeight;
    int m_RealWidth, m_RealHeight;
    float m_ScreenQuality;

	vector<FLvertex> m_GridPoints;
	vector<WORD> m_GridIndices;

    strDX*  m_DX;

	void D3D_StartDraw();
	void D3D_EndDraw();

	void D3D_DrawSelectionRect();
	void D3D_UpdateGrid();
	void D3D_DrawGrid();
	void D3D_DrawPivot();

    void Redraw();

    void D3D_Resize();
	bool D3D_Create();
	void D3D_Clear();

    void SetSelection( bool flag );
    void InvertSelection();
protected:
    void D3D_CreateStateBlocks();
    void D3D_DestroyStateBlocks();
public:
    DWORD SB_DrawMeshLayer;
    DWORD SB_DrawMeshLayerAlpha;
    DWORD SB_DrawMeshLayerAdd;
    DWORD SB_DrawMeshLayerMul;
    DWORD SB_DrawWire;
    DWORD SB_DrawSelectionRect;
    DWORD SB_DrawClipPlane;
    void __forceinline D3D_ApplyRenderState(DWORD state){ d3d()->ApplyStateBlock(state); }
public:
    CFrustumClipper*    frustum;
    CStatistic*         stat;
    TUI_Tools*          m_Tools;

	// non-hidden ops
	POINT m_StartCp;
	POINT m_CurrentCp;

	Fvector m_StartRStart;
	Fvector m_StartRNorm;

	Fvector m_CurrentRStart;
	Fvector m_CurrentRNorm;

	// hidden ops
	POINT m_CenterCpH;
	POINT m_StartCpH;
	POINT m_DeltaCpH;
protected:
    bool m_CameraMoveMode;
	bool m_MouseCaptured;
	bool m_MouseMultiClickCaptured;
public:
	long m_FramePrevTime;
	float m_FrameDTime;
public:
    bool bRenderTextures;
    bool bRenderWire;
    bool bRenderLights;
    bool bRenderFilter;
    bool bRenderMultiTex;
    bool bDrawLights;
    bool bDrawSounds;
    bool bDrawGrid;
    bool bDrawPivot;
    bool bDrawStatistic;
public:
    TUI();
    virtual ~TUI();
	__inline bool ddvalid()             { 	return m_D3D_ok; }
	LPDIRECTDRAW7 dd();
	LPDIRECT3DDEVICE7 d3d();
	LPDDPIXELFORMAT backdesc();
	__inline Fmatrix& camera()          {	return m_Camera; }
	__inline Fvector& pivot()           {	return m_Pivot; }
	__inline Fvector& pivotsnap()       {	return m_PivotSnap; }
	__inline float anglesnap()          {	return m_AngleSnap; }
	__inline float movesnap()           {	return m_MoveSnap; }

	__inline void VisibilityChange()    {	m_VisibilityChanged = true; }
	__inline void SelectionChange()     {	m_SelectionChanged = true; }

    bool Init(TPanel* pa, TPaintBox* pb);
    void Clear();

    char* GetEditFileName()             {   return m_LastFileName; }

    void __fastcall Idle();
    void Resize()                       {   bResize = true; bRedraw = true; }
    void UpdateScene(bool bForced=false){   bRedraw = true; if (bForced) Idle();}

    void CameraPan(float X, float Z);
    void CameraScale(float Y);
    void CameraRotate(float X, float Y);

    void SetRenderQuality(float q)      {   m_ScreenQuality = q;}
// mouse action
	void MouseRayFromPoint(	Fvector *start,	Fvector *direction,	POINT *point );
    void EnableSelectionRect( bool flag );
    void UpdateSelectionRect( POINT *from, POINT *to );
	bool MouseBox( ICullPlane *planes, POINT *point1, POINT *point2 );
	bool PickGround( Fvector *hitpoint,	Fvector *start,	Fvector *direction );

    SceneObject*    SelectSObject();

    void __fastcall MouseStart  (TShiftState Shift, int X, int Y);
    void __fastcall MouseEnd    (TShiftState Shift, int X, int Y);
    void __fastcall MouseProcess(TShiftState Shift, int X, int Y);
    void MouseMultiClickCapture (bool b){m_MouseMultiClickCaptured = b;}

    bool __fastcall IsMouseCaptured(){return m_MouseCaptured;}

    bool __fastcall KeyDown     (WORD Key, TShiftState Shift);
    bool __fastcall KeyUp       (WORD Key, TShiftState Shift);
    bool __fastcall KeyPress    (WORD Key, TShiftState Shift);

    bool Command( int _Command, int p = 0 );
};
extern TUI UI;
#endif
