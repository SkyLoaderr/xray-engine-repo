//----------------------------------------------------
// file: UI_Main.h
//----------------------------------------------------

#ifndef _INCDEF_UI_Main_H_
#define _INCDEF_UI_Main_H_

#include "UI_ViewControl.h"
#include "UI_RControl.h"
#include "UI_PControl.h"
#include "UI_SControl.h"

#include "TB_Main.h"
#include "TB_AddLandscape.h"
#include "TB_SelLandscape.h"
#include "TB_MoveLandscape.h"
#include "TB_RotateLandscape.h"
#include "TB_ScaleLandscape.h"
#include "TB_AddObject.h"
#include "TB_SelObject.h"
#include "TB_MoveObject.h"
#include "TB_RotateObject.h"
#include "TB_ScaleObject.h"
#include "TB_AddLight.h"
#include "TB_SelLight.h"
#include "TB_AddSound.h"
#include "TB_SelSound.h"
#include "TB_MoveSound.h"
//#include "TB_MoveLight.h"
//#include "TB_RotateLight.h"
#include "TB_AddSndPlane.h"
#include "TB_SelSndPlane.h"
#include "TB_MoveSndPlane.h"
#include "TB_RotateSndPlane.h"
#include "TB_ScaleSndPlane.h"

#include "MSC_List.h"

//----------------------------------------------------

class ETexture;
class SceneObject;

//----------------------------------------------------

#define GL_TEXT_LIST_BASE 200

#define TARGET_LANDSCAPE 0
#define TARGET_OBJECT    1
#define TARGET_LIGHT     2
#define TARGET_PIVOT     3
#define TARGET_SNDPLANE  4
#define TARGET_SOUND	 5

#define ACTION_SELECT    0
#define ACTION_ADD       1
#define ACTION_MOVE      2
#define ACTION_ROTATE    3
#define ACTION_SCALE     4

#define VIEWPORT_MAP     0
#define VIEWPORT_USER    1
#define VIEWPORT_OBJECT  2

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

#define COMMAND_INVERT_SELECTION_ALL  19
#define COMMAND_SELECT_ALL            20
#define COMMAND_DESELECT_ALL          21

#define COMMAND_DELETE_SELECTION      22


class XRayEditorUI {
protected:

	friend BOOL CALLBACK WP_LeftBar( HWND hw, UINT msg, WPARAM wp, LPARAM lp );
	friend BOOL CALLBACK WP_BottomBar( HWND hw, UINT msg, WPARAM wp, LPARAM lp );
	friend BOOL CALLBACK WP_TopBar( HWND hw, UINT msg, WPARAM wp, LPARAM lp );

	HINSTANCE m_Instance;

	HWND m_Window;
	HWND m_VisWindow;

	HWND m_LeftSubPlace;
	HWND m_LeftBar;
	HWND m_TopBar;
	HWND m_BottomBar;

	int m_Target;
	int m_Action;
	int m_Viewport;

	int m_WireMode;
	int m_TexturesEnabled;

	char m_LastFileName[MAX_PATH];

	int m_GridEnabled;
	IGroundPoint m_GridCenter;
	IGroundPoint m_GridStep;
	int m_GridSubDiv[2];
	int m_GridCounts[2];

	float m_AngleSnap;
	IVector m_PivotSnap;
	IVector m_Pivot;
	
	int m_FltPivots;
	int m_FltSounds;
	int m_FltLights;

	IColor m_ColorFore_S;
	IColor m_ColorFore_B;
	IColor m_ColorBack_S;
	
	IColor m_ColorGrid;
	IColor m_ColorGridTh;
	IColor m_BackgroundColor;

	UI_RControl m_RotateControl;
	UI_PControl m_MoveControl;
	UI_SControl m_ZoomControl;

	XRayEditorUIToolBox *m_CurrentBox;

	TB_AddLandscape m_CtlAddLandscape;
	TB_SelLandscape m_CtlSelectLandscape;
	TB_MoveLandscape m_CtlMoveLandscape;
	TB_RotateLandscape m_CtlRotateLandscape;
	TB_ScaleLandscape m_CtlScaleLandscape;

	TB_AddSndPlane m_CtlAddSndPlane;
	TB_SelSndPlane m_CtlSelectSndPlane;
	TB_MoveSndPlane m_CtlMoveSndPlane;
	TB_RotateSndPlane m_CtlRotateSndPlane;
	TB_ScaleSndPlane m_CtlScaleSndPlane;

	TB_AddObject m_CtlAddObject;
	TB_SelObject m_CtlSelectObject;
	TB_MoveObject m_CtlMoveObject;
	TB_RotateObject m_CtlRotateObject;
	TB_ScaleObject m_CtlScaleObject;

	TB_AddLight m_CtlAddLight;
	TB_SelLight m_CtlSelectLight;

	TB_AddSound m_CtlAddSound;
	TB_SelSound m_CtlSelectSound;

	XRayEditorUIToolBox m_CtlMoveLight;
	XRayEditorUIToolBox m_CtlRotateLight;

	XRayEditorUIToolBox m_CtlNothing;

protected:

	bool m_SelectionRect;
	POINT m_SelStart;
	POINT m_SelEnd;

	bool m_MouseCaptured;
	MouseCallback *m_CurrentMouseCB;

protected:

	friend class UI_RControl;
	friend class UI_PControl;
	friend class UI_SControl;

	bool m_SelectionChanged;
	bool m_VisibilityChanged;

	bool m_D3D_ok;

	IVector m_LastPick;
	IVector m_LastStart;
	IVector m_LastDir;

	IMatrix m_Identity;
	IMatrix m_Camera;
	IMatrix m_Projection;
	float m_Aspect;
	float m_FOV;
	float m_Znear;
	float m_Zfar;
	int m_RenderWidth;
	int m_RenderHeight;
	int m_RenderBpp;
	long m_StartTime;

	vector<D3DLVERTEX> m_GridPoints;
	vector<WORD> m_GridIndices;

	LPDIRECTDRAW7 m_DDraw;
	LPDIRECTDRAWSURFACE7 m_FrontBuffer;
	LPDIRECTDRAWSURFACE7 m_BackBuffer;
	LPDIRECTDRAWSURFACE7 m_ZBuffer;
	LPDIRECT3DDEVICE7 m_D3Dev;
	LPDIRECT3D7 m_D3D;

	DDSURFACEDESC2 m_FrontDesc;
	DDSURFACEDESC2 m_BackDesc;
	DDSURFACEDESC2 m_ZDesc;

	bool D3D_CreateClipper();
	bool D3D_CreateBackBuffers();
	bool D3D_CreateDevice();
	bool D3D_Create();
	void D3D_Clear();
	void D3D_StartDraw( RECT *_Rect );
	void D3D_EndDraw();

	void D3D_DrawSelectionRect();
	void D3D_UpdateGrid();
	void D3D_DrawGrid();
	void D3D_DrawPivot();

protected:

	long m_FramePrevTime;
	float m_FrameDTime;

protected:

	bool CreateMainWindow();
	bool CreateLeftBar();
	bool CreateTopBar();
	bool CreateBottomBar();
	bool CreateVisWindow();

	void DestroyMainWindow();
	void DestroyLeftBar();
	void DestroyTopBar();
	void DestroyBottomBar();
	void DestroyVisWindow();

public:

	void SetDlgItemFloat( HWND hw, int ctlid, float value, int prec=3 );
	float GetDlgItemFloat( HWND hw, int ctlid );

	__forceinline HINSTANCE inst(){
		return m_Instance; }
	__forceinline HWND wnd(){
		return m_Window; }
	__forceinline bool ddvalid() {
		return m_D3D_ok; }
	__forceinline LPDIRECTDRAW7 dd() {
		return m_DDraw; }
	__forceinline LPDIRECT3DDEVICE7 d3d() {
		return m_D3Dev; }
	__forceinline LPDDPIXELFORMAT backdesc(){
		return &m_BackDesc.ddpfPixelFormat; }
	__forceinline int te() {
		return m_TexturesEnabled; }
	__forceinline int wmode(){
		return m_WireMode; }
	__forceinline IMatrix& camera(){
		return m_Camera; }
	__forceinline IVector& pivot(){
		return m_Pivot; }
	__forceinline IVector& pivotsnap(){
		return m_PivotSnap; }
	__forceinline float anglesnap(){
		return m_AngleSnap; }
	__forceinline bool candrawlights(){
		return !!m_FltLights; }
	__forceinline bool candrawsounds(){
		return !!m_FltSounds; }

	__forceinline void VisibilityChange(){
		m_VisibilityChanged = true; }
	__forceinline void SelectionChange(){
		m_SelectionChanged = true; }

	bool Init( HINSTANCE _Instance );
	void Clear( );
	void Idle( );
	void Redraw( );

	XRayEditorUI();
	~XRayEditorUI();

	void EnterPivotDialog();
	void AlignWindows();
	void UpdateCaption();
	void UpdateToolOptions();
	void ChangeTarget( int _NewTarget );
	void ChangeAction( int _NewAction );
	void ChangeViewport( int _NewViewport );
	void Command( int _Command );
	void AccelCommand( int _Command );
	void PrintCamera();
	void SetSelection( bool flag );
	void InvertSelection();
	int ClassIDFromTargetID( int target );
	

	void ProcessSelectionChanges();

	void MouseStart( bool m_Alt );
	void MouseEnd();
	void MouseProcess();

	void ResetToDefaultGrid();
	void ResetToDefaultColors();
	void DrawButton( LPDRAWITEMSTRUCT _DS );
	void DrawCheck( LPDRAWITEMSTRUCT _DS, int _Selected );
	void DrawGoldText( LPDRAWITEMSTRUCT _DS );
	void GoldFill( HDC hdc, RECT *rc );

	ETexture *SelectLandscape();
	SceneObject *SelectSObject();

public:

	void MouseRayFromPoint(
		IVector *start,
		IVector *direction,
		POINT *point );

	bool MouseBox(
		ICullPlane *planes, // four planes !
		POINT *point1, POINT *point2 );

	bool PickGround(
		IVector *hitpoint,
		IVector *start,
		IVector *direction );

	void EnableSelectionRect( bool flag );
	void UpdateSelectionRect( POINT *from, POINT *to );
};


extern XRayEditorUI UI;
extern int g_RenderLightMaps;
extern int g_LinearTFilter;


#endif /*_INCDEF_UI_Main_H_*/
