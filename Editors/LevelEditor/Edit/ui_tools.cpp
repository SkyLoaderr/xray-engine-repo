#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_control.h"
#include "UI_CustomTools.h"
#include "LeftBar.h"
#include "TopBar.h"
#include "Scene.h"
#include "ui_main.h"

#include "editlibrary.h"

#define DETACH_FRAME(a) if (a){ (a)->Parent = NULL;}
#define ATTACH_FRAME(a,b) if (a){ (a)->Parent = (b);}

TUI_Tools Tools;

TShiftState ssRBOnly;
//---------------------------------------------------------------------------
TUI_Tools::TUI_Tools()
{
	m_Props = TProperties::CreateForm(0,alClient,OnPropsModified,0,OnPropsClose);
    m_Flags.zero();
}
//---------------------------------------------------------------------------
TUI_Tools::~TUI_Tools()
{
	TProperties::DestroyForm(m_Props);
    for (DWORD i=0; i<etMaxTarget; i++) _DELETE(m_pTools[i]);
}
//---------------------------------------------------------------------------

TFrame*	TUI_Tools::GetFrame(){
	if (pCurTools) return pCurTools->pFrame;
    return 0;
}
//---------------------------------------------------------------------------

void TUI_Tools::OnCreate(){
    target          = -1;
    action          = -1;
    ZeroMemory      (m_pTools,sizeof(TUI_CustomTools*)*etMaxTarget);
    pCurTools       = 0;
    ssRBOnly << ssRight;
    paParent = fraLeftBar->paFrames;   VERIFY(paParent);
    m_Flags.set(flChangeAction,FALSE);
    m_Flags.set(flChangeTarget,FALSE);
// create tools
    for (int tgt=etFirstTool; tgt<etMaxTarget; tgt++)
		m_pTools[tgt]=NewToolFromTarget(tgt);
}
//---------------------------------------------------------------------------

void TUI_Tools::OnDestroy(){
    pCurTools->OnDeactivate();
}
//---------------------------------------------------------------------------
void TUI_Tools::Reset(){
	SetTarget(GetTarget(),true);
}
//---------------------------------------------------------------------------

bool __fastcall TUI_Tools::MouseStart(TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->Start(Shift);
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::MouseMove(TShiftState Shift){
    if(pCurTools->pCurControl) pCurTools->pCurControl->Move(Shift);
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::MouseEnd(TShiftState Shift){
    if(pCurTools->pCurControl)	return pCurTools->pCurControl->End(Shift);
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::OnFrame(){
	if(!UI.IsMouseCaptured()){
        // если нужно изменить target выполняем после того как мышь освободится
        if(m_Flags.is(flChangeTarget)) 		SetTarget(iNeedTarget);
        // если нужно изменить action выполняем после того как мышь освободится
        if(m_Flags.is(flChangeAction)) 		SetAction(iNeedAction);
    }
	if(m_Flags.is(flUpdateProperties)) 	RealUpdateProperties();
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::OnObjectsUpdate(){
    if(pCurTools->pCurControl) return pCurTools->OnObjectsUpdate();
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::HiddenMode(){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->HiddenMode();
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyDown   (WORD Key, TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->KeyDown(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyUp     (WORD Key, TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->KeyUp(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyPress  (WORD Key, TShiftState Shift){
    if(pCurTools->pCurControl) return pCurTools->pCurControl->KeyPress(Key,Shift);
    return false;
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::SetAction   (int act)
{
    action = act;
    switch(act){
        case eaSelect:  UI.GetD3DWindow()->Cursor = crCross;     break;
        case eaAdd:     UI.GetD3DWindow()->Cursor = crArrow;     break;
        case eaMove:    UI.GetD3DWindow()->Cursor = crSizeAll;   break;
        case eaRotate:  UI.GetD3DWindow()->Cursor = crSizeWE;    break;
        case eaScale:   UI.GetD3DWindow()->Cursor = crVSplit;    break;
        default:        UI.GetD3DWindow()->Cursor = crHelp;
    }
    if (pCurTools) pCurTools->SetAction(action);
    UI.RedrawScene();
    fraTopBar->ChangeAction(act);
    UI.Command(COMMAND_UPDATE_TOOLBAR);
    m_Flags.set	(flChangeAction,FALSE);
}

void __fastcall TUI_Tools::ChangeAction(int act){
	// если мышь захвачена - изменим action после того как она освободится
	if (!(UI.IsMouseCaptured()||UI.IsMouseInUse())){
    	SetAction	(act);
    }
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::SetTarget   (int tgt,bool bForced)
{
    if(bForced||(target!=tgt)){
        target = tgt;
        if (pCurTools){
            DETACH_FRAME(pCurTools->pFrame);
            pCurTools->OnDeactivate();
        }
        pCurTools=m_pTools[tgt]; VERIFY(pCurTools);
        pCurTools->OnActivate();
        pCurTools->SetAction(action);
        pCurTools->ResetSubTarget();
        ATTACH_FRAME(pCurTools->pFrame, paParent);
    }
    UI.RedrawScene();
    fraLeftBar->ChangeTarget(tgt);
    UI.Command(COMMAND_UPDATE_TOOLBAR);
    m_Flags.set(flChangeTarget,FALSE);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::SetSubTarget(int tgt)
{
    pCurTools->SetSubTarget(tgt);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::UnsetSubTarget(int tgt)
{
    pCurTools->UnsetSubTarget(tgt);
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::ChangeTarget(int tgt){
	// если мышь захвачена - изменим target после того как она освободится
	if (!(UI.IsMouseCaptured()||UI.IsMouseInUse())) SetTarget(tgt);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumPosition(CCustomObject* O){
	if (pCurTools) pCurTools->SetNumPosition(O);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumRotation(CCustomObject* O){
	if (pCurTools) pCurTools->SetNumRotation(O);
}
//---------------------------------------------------------------------------
void __fastcall	TUI_Tools::SetNumScale(CCustomObject* O){
	if (pCurTools) pCurTools->SetNumScale(O);
}
//---------------------------------------------------------------------------


#define MIN_PANEL_HEIGHT 15
void __fastcall PanelMinimizeClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }else{
        pa->Tag    = pa->Height;
        pa->Height = MIN_PANEL_HEIGHT;
    }
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
    UI.Command(COMMAND_UPDATE_TOOLBAR);
}
//---------------------------------------------------------------------------

EObjClass TUI_Tools::CurrentClassID(){
	return (fraLeftBar->ebIgnoreMode->Down)?OBJCLASS_DUMMY:GetTargetClassID();
}
//---------------------------------------------------------------------------

void TUI_Tools::OnShowHint(AStringVec& ss){
	Scene.OnShowHint(ss);
}
//---------------------------------------------------------------------------

bool TUI_Tools::Pick()
{
    if( Scene.locked() && (esEditLibrary==UI.GetEState())){
        UI.iGetMousePosReal(Device.m_hRenderWnd, UI.m_CurrentCp);
        UI.m_StartCp = UI.m_CurrentCp;
        Device.m_Camera.MouseRayFromPoint(UI.m_StartRStart, UI.m_StartRNorm, UI.m_StartCp );
        Device.m_Camera.MouseRayFromPoint(UI.m_CurrentRStart, UI.m_CurrentRNorm, UI.m_CurrentCp );
        SRayPickInfo pinf;
        TfrmEditLibrary::RayPick(UI.m_CurrentRStart,UI.m_CurrentRNorm,&pinf);
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------

#include "PropertiesLight.h"
#include "PropertiesEvent.h"
#include "PropertiesPS.h"
#include "PropertiesWayPoint.h"

void TUI_Tools::ShowProperties()
{
    ObjectList lst;
    EObjClass cls_id				= CurrentClassID();
    if (Scene.GetQueryObjects		(lst,cls_id)){
        bool bChange				= false;
        switch(cls_id){
//        case OBJCLASS_LIGHT:    	frmPropertiesLightRun(&lst,bChange);	   	break;
        case OBJCLASS_EVENT:   		frmPropertiesEventRun(&lst,bChange);		break;
        case OBJCLASS_WAY:   		TfrmPropertiesWayPoint::Run(&lst,bChange);	break;
        case OBJCLASS_PS:			TfrmPropertiesPS::Run(&lst,bChange);		break;
        default:
            m_Props->ShowProperties	();
            UpdateProperties		();
        }
        if (bChange) Scene.UndoSave();
        UI.RedrawScene();
    }
}
//---------------------------------------------------------------------------

void TUI_Tools::HideProperties()
{
	m_Props->HideProperties			();
}
//---------------------------------------------------------------------------
/*
static s8 		_s8=0;
static u32 		_u32=0;
static float	_float0=0.f;
static float	_float1=1.f;
static BOOL		_BOOL=0.f;
static Flags16	_flag16={0};
static Fcolor 	_fcolor={0,0,0,0};
static Fvector 	_fvector={0,0,0};
static string32 _text={0};
static AnsiString _atext="";
static WaveForm	_wave;
*/
void TUI_Tools::RealUpdateProperties()
{
	if (m_Props->Visible){
		if (m_Props->IsModified()) Scene.UndoSave();
        ObjectList lst;
        EObjClass cls_id				= CurrentClassID();
        PropItemVec items;
/*
    	PHelper.CreateCaption	(items,	"Caption",	"Caption2");
    	PHelper.CreateS8		(items,	"S8", 		&_s8);
    	PHelper.CreateU32		(items,	"U32", 		&_u32);
    	PHelper.CreateFloat		(items,	"Float",	&_float0);
    	PHelper.CreateFloat		(items,	"Float",	&_float1);
    	PHelper.CreateBOOL		(items,	"BOOL",		&_BOOL);
    	PHelper.CreateFlag16	(items, "Flag16",	&_flag16,0x01);
	    PHelper.CreateVector	(items, "Vector",	&_fvector);
        PHelper.CreateText		(items, "Text", 	_text, sizeof(_text));
		PHelper.CreateAText		(items, "AText", 	&_atext);
        PHelper.CreateWave		(items, "Wave",		&_wave);
	IC TokenValue2*   	CreateToken2	(PropItemVec& items, LPCSTR key, u32* val, AStringVec* lst)
	IC TokenValue3*   	CreateToken3	(PropItemVec& items, LPCSTR key, u32* val, u32 cnt, const TokenValue3::Item* lst)
	IC ListValue* 	 	CreateList		(PropItemVec& items, LPCSTR key, LPSTR val, AStringVec* lst)
	IC ListValue* 	 	CreateListA		(PropItemVec& items, LPCSTR key, LPSTR val, u32 cnt, LPCSTR* lst)
	IC TextValue* 	 	CreateEShader	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
	IC TextValue* 	   	CreateCShader	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
	IC TextValue* 	   	CreateTexture	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
	IC TextValue* 	  	CreateTexture2	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
	IC ATextValue* 		CreateAEShader	(PropItemVec& items, LPCSTR key, AnsiString* val)
	IC ATextValue* 		CreateACShader	(PropItemVec& items, LPCSTR key, AnsiString* val)
    IC ATextValue*	   	CreateAGameMtl	(PropItemVec& items, LPCSTR key, AnsiString* val)
	IC ATextValue* 		CreateATexture	(PropItemVec& items, LPCSTR key, AnsiString* val)
	IC TextValue*	 	CreateLightAnim	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
	IC TextValue* 	 	CreateLibObject	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
	IC ATextValue* 		CreateALibObject(PropItemVec& items, LPCSTR key, AnsiString* val)
	IC TextValue* 	 	CreateGameObject(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    IC TextValue*		CreateLibSound	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    IC ATextValue*	  	CreateALibSound	(PropItemVec& items, LPCSTR key, AnsiString* val)
    IC TextValue*	 	CreateLibPS		(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    IC ATextValue*	 	CreateALibPS	(PropItemVec& items, LPCSTR key, AnsiString* val)
	IC TextValue* 		CreateEntity	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    IC TextValue* 		CreateGameMtl	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
*/
        if (Scene.GetQueryObjects(lst,cls_id)){
            for (ObjectIt it=lst.begin(); it!=lst.end(); it++){
                LPCSTR pref				= GetClassNameByClassID((*it)->ClassID);
                (*it)->FillProp	 		(pref,items);
            }
        }

		m_Props->AssignItems		(items,true,"Object Inspector");
    }
	m_Flags.set(flUpdateProperties,FALSE);
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::OnPropsClose()
{
	if (m_Props->IsModified()) Scene.UndoSave();
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::OnPropsModified()
{
	Scene.Modified();
//	Scene.UndoSave();
}
//---------------------------------------------------------------------------


