//---------------------------------------------------------------------------

#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "ui_control.h"
#include "ui_scenetools.h"
#include "ui_lighttools.h"
#include "ui_soundtools.h"
#include "ui_objecttools.h"
#include "ui_pclippertools.h"
#include "ui_texturingtools.h"
#include "ui_main.h"
#include "sceneclasslist.h"
#include "movescene.h"

/*
ƒл€ добавлени€ новой тулзы:
- в UI_Tools.h добавить в enum ETarget - название тулзы (etXxxx)
- назначить визуальной кнопке sbTargetXxxx свойству Tag значение etXxxx (в конструкторе frmMain)
- создать по образу и подобию UI_XxxxTools.*
    - описать функцию InitMSCXxxx (в ней добавл€ютс€ мышиные калбэки в список)
    - описать мышиные калбэки (Start, Move, End, HiddenMode)
        - Start - должен вернуть true если нужно забрать мышь на себ€ (визуально мышь стоит на месте)
        - End - должен вернуть true если мышь была забрана и теперь ее уже можна отдать
        - HiddenMode - должен вернуть true если курсор мыши невидим
- вызвать функцию InitMSCXxxx в TUI_Tools::Init
- создать новые TFrame дл€ визуальной установки каких-либо дополнительных параметров
    - создавать можно через Repository из существующих fraSelectXxxx (Move,Add...)
    - в конструкторе TFrame зарегистрировать в списке: UI.m_Tools.AddFrame(etXxxx,eaAdd,this);
    - в опци€х проекта установить, что это автоматически создавема€ форма (по умочанию она свободна€)
*/
TShiftState ssRBOnly;
//---------------------------------------------------------------------------
__fastcall TUI_Tools::TUI_Tools()
{
    target          = etNone;
    action          = eaNone;
    pCurControl     = 0;
    pCurFrame       = 0;
    ZeroMemory(ControlTools,sizeof(TUI_Control*)*eaMaxActions*etMaxTarget);
    ZeroMemory(Frames,sizeof(TFrame*)*eaMaxActions*etMaxTarget);
    ssRBOnly << ssRight;
}
//---------------------------------------------------------------------------
TUI_Tools::~TUI_Tools()
{
}
//---------------------------------------------------------------------------

void __fastcall TUI_Tools::Init()
{
// Call Init for all Control Callback's
    InitMSCScene();
    InitMSCLight();
    InitMSCSound();
    InitMSCObject();
    InitMSCPClipper();
    InitMSCTexturing();
}
//---------------------------------------------------------------------------

bool TUI_Tools::HiddenMode (){
    return  (pCurControl)?pCurControl->HiddenMode():false;
}
//---------------------------------------------------------------------------

#define DETACH_FRAME(a) if (a){ (a)->Parent = NULL; (a)=NULL;}
void __fastcall TUI_Tools::SetTargetAction()
{
    if (pCurControl) pCurControl->OnExit();
    DETACH_FRAME(pCurFrame);
    pCurFrame  = Frames[target][action];
    pCurControl = ControlTools[target][action];
    if (pCurFrame) pCurFrame->Parent = paParent;
    switch(action){
        case eaSelect:  UI.GetPPRender()->Cursor = crCross;     break;
        case eaAdd:     UI.GetPPRender()->Cursor = crDrag;      break;
        case eaMove:    UI.GetPPRender()->Cursor = crVSplit;    break;
        case eaRotate:  UI.GetPPRender()->Cursor = crSizeWE;    break;
        case eaScale:   UI.GetPPRender()->Cursor = crSizeAll;   break;
        default:        UI.GetPPRender()->Cursor = crArrow;
    }
    if (pCurControl) pCurControl->OnEnter();
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::SetTarget   (DWORD tgt)
{
    if(target!=tgt){
        target = (ETarget)tgt;
        SetTargetAction();
    }
}

void __fastcall TUI_Tools::SetAction   (DWORD act)
{
    if(action!=act){
        action = (EAction)act;
        SetTargetAction();
    }
}

extern void ShowObjectContextMenu(const int cls);

bool __fastcall TUI_Tools::MouseStart(TShiftState Shift)
{
    bool bRes = false;
    if(pCurControl) bRes = pCurControl->Start(Shift);
    // if click only right button then Show context menu
    if (!bRes && (Shift==ssRBOnly)){
        ShowObjectContextMenu(ClassIDFromTargetID(target));
        return false;
    }
    return bRes;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::MouseEnd  (TShiftState Shift)
{
    if(pCurControl) return pCurControl->End(Shift);
    return false;
}
//---------------------------------------------------------------------------
void __fastcall TUI_Tools::MouseMove (TShiftState Shift)
{
    if(pCurControl) pCurControl->Move(Shift);
}
//---------------------------------------------------------------------------

bool __fastcall TUI_Tools::KeyDown   (WORD Key, TShiftState Shift)
{
    if(pCurControl) return pCurControl->KeyDown(Key, Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyUp     (WORD Key, TShiftState Shift)
{
    if(pCurControl) return pCurControl->KeyUp(Key, Shift);
    return false;
}
//---------------------------------------------------------------------------
bool __fastcall TUI_Tools::KeyPress  (WORD Key, TShiftState Shift)
{
    if(pCurControl) return pCurControl->KeyPress(Key, Shift);
    return false;
}
//---------------------------------------------------------------------------



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
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
}
//---------------------------------------------------------------------------
LPSTR GetNameByClassID(int cls_id){
    switch(cls_id){
    case OBJCLASS_SOBJECT2: return "Object";
    case OBJCLASS_LIGHT:    return "Light";
    case OBJCLASS_SOUND:    return "Sound";
    case OBJCLASS_PCLIPPER: return "PClipper";
    default: return 0;
    }
}
bool IsClassID(int cls_id){
    switch(cls_id){
    case OBJCLASS_SOBJECT2: return true;
    case OBJCLASS_LIGHT:    return true;
    case OBJCLASS_SOUND:    return true;
    case OBJCLASS_PCLIPPER: return true;
    default: return 0;
    }
}
int ClassIDFromTargetID( int target ){
	switch( target ){
    case etScene: return OBJCLASS_NONE;
	case etObject: return OBJCLASS_SOBJECT2;
	case etSound: return OBJCLASS_SOUND;
	case etLight: return OBJCLASS_LIGHT;
    case etPClipper: return OBJCLASS_PCLIPPER;
    default: return OBJCLASS_NONE;
	}
}

