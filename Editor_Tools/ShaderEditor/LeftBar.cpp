#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Main.h"
#include "main.h"
#include "Blender.h"
#include "xr_trims.h"
#include "ShaderTools.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "ExtBtn"
#pragma link "MxMenus"
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma resource "*.dfm"
TfraLeftBar *fraLeftBar;

#define TYPE_RAZDEL 0
#define TYPE_FOLDER 1
#define TYPE_OBJECT 2

//---------------------------------------------------------------------------
bool __fastcall FolderLookupFunc(TElTreeItem* Item, void* SearchDetails){
	if (((DWORD)Item->Data)!=TYPE_FOLDER) return false;
    AnsiString& s1 = *(AnsiString*)SearchDetails;
    AnsiString& s2 = Item->Text;
	return (s1==s2);
}
//---------------------------------------------------------------------------

TElTreeItem* TfraLeftBar::FindFolderItem(TElTreeItem* start_item, const AnsiString& name){
	if (start_item) return tvShaders->Items->LookForItemEx(start_item,-1,false,true,false,(LPVOID)&name,FolderLookupFunc);
    else{
    	for (TElTreeItem* node=tvShaders->Items->GetFirstNode(); node; node=node->GetNextSibling())
        	if (node->Text==name) return node;
    }
    return 0;
}
//---------------------------------------------------------------------------

#define MIN_PANEL_HEIGHT 15
//---------------------------------------------------------------------------
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
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}
void __fastcall PanelMaximizeOnlyClick(TObject *Sender)
{
    TPanel* pa = ((TPanel*)((TControl*)Sender)->Parent);
    if (pa->Tag > 0){
        pa->Height = pa->Tag;
        pa->Tag    = 0;
    }
    UI->Command(COMMAND_UPDATE_TOOLBAR);
}

//---------------------------------------------------------------------------
__fastcall TfraLeftBar::TfraLeftBar(TComponent* Owner)
        : TFrame(Owner)
{
    char buf[MAX_PATH] = {"shader_ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------

void UpdatePanel(TPanel* p){
    if (p){
        for (int j=0; j<p->ControlCount; j++){
            TExtBtn* btn = dynamic_cast<TExtBtn *>(p->Controls[j]);
            if (btn) btn->UpdateMouseInControl();
        }
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::UpdateBar(){
    int i, j;//, h=0;
    for (i=0; i<fraLeftBar->ComponentCount; i++){
        TComponent* temp = fraLeftBar->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
/*
    for (j=0; j<paLeftBar->ControlCount; j++){
        TPanel* pa = dynamic_cast<TPanel*>(paLeftBar->Controls[j]);
        if (pa&&pa->Visible) h+=pa->Height;
    }
    paLeftBar->Height = h+2;
*/
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveClick(TObject *Sender)
{
	UI->Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebReloadClick(TObject *Sender)
{
	UI->Command( COMMAND_RELOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRefreshTexturesClick(TObject *Sender)
{
	UI->Command( COMMAND_REFRESH_TEXTURES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMimimizeClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PanelMaximizeClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
    UpdateBar();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEditorPreferencesClick(TObject *Sender)
{
	UI->Command(COMMAND_EDITOR_PREF);
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::ebResetAnimationClick(TObject *Sender)
{
	UI->Command( COMMAND_RESET_ANIMATION );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ShowPPMenu(TMxPopupMenu* M, TObject* B){
    POINT pt;
    GetCursorPos(&pt);
	M->Popup(pt.x,pt.y-10);
    TExtBtn* btn = dynamic_cast<TExtBtn*>(B); if(btn) btn->MouseManualUp();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmSceneFile,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmPreviewObject,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvShadersMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbRight)	ShowPPMenu(pmShaderList,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebShaderPropertiesClick(TObject *Sender)
{
	UI->Command( COMMAND_SHADER_PROPERTIES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PreviewClick(TObject *Sender)
{
	UI->Command( COMMAND_SELECT_PREVIEW_OBJ, dynamic_cast<TMenuItem*>(Sender)->Tag );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebApplyChangesClick(TObject *Sender)
{
	UI->Command( COMMAND_APPLY_CHANGES );
}
//---------------------------------------------------------------------------

void TfraLeftBar::InitPalette(TemplateVec& lst){
	for (TemplateIt it=lst.begin(); it!=lst.end(); it++){
    	TMenuItem* mi= new TMenuItem(0);
		mi->Caption = (*it)->getComment();
		mi->Tag     = int(*it);
		mi->OnClick = TemplateClick;
		pmBlenderList->Items->Add(mi);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebShaderCreateMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmBlenderList,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::TemplateClick(TObject *Sender)
{
	TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    AnsiString folder;
	MakeName(tvShaders->Selected,folder,false);
    CBlender* B = SHTools.AppendBlender(((CBlender*)mi->Tag)->getDescription().CLS,folder.c_str(),0);
	SHTools.SetCurrentBlender(B);
	UI->Command(COMMAND_SHADER_PROPERTIES);
}
//---------------------------------------------------------------------------

TElTreeItem* TfraLeftBar::FindFolder(LPCSTR full_name){
	int cnt = _GetItemCount(full_name,'\\')-1;
    if (cnt<=0) return 0;
    int itm = 0;
	char fld[64];
	TElTreeItem* node = 0;
    do _GetItem(full_name,itm++,fld,'\\');
    while ((node = FindFolderItem(node,fld))&&(itm<cnt));

	return node;
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddBlender(LPCSTR full_name, bool bLoadMode){
	int cnt = _GetItemCount(full_name,'\\');
    R_ASSERT2(cnt,"Blender name empty.");
    TElTreeItem* folder = FindFolder(full_name);
	char obj[64];
	_GetItem(full_name,cnt-1,obj,'\\');
    if (!folder&&(cnt>1)){
	    int itm = 0;
        cnt--;
		char fld[64];
		TElTreeItem* node = 0;
        bool bCreate=false;
    	do{ _GetItem(full_name,itm++,fld,'\\');
            if (!bCreate) node = FindFolderItem(node,fld);
            if (!node) 	bCreate = true;
        	else 		folder = node;
            if (bCreate) folder = tvShaders->Items->AddChildObject(folder,fld,(LPVOID)TYPE_FOLDER);
	    }while (itm<cnt);
    }
	TElTreeItem* node = tvShaders->Items->AddChildObject(folder,obj,(LPVOID)TYPE_OBJECT);
    if (!bLoadMode){
	    if (folder) folder->Expand(false);
    	node->Selected = true;
		tvShaders->Selected = node;
    }
}
//---------------------------------------------------------------------------

bool TfraLeftBar::MakeName(TElTreeItem* select_item, AnsiString& name, bool bFull){
	if (select_item){
    	TElTreeItem* node = (DWORD(select_item->Data)==TYPE_OBJECT)?select_item->Parent:select_item;
        while (node){
			name.Insert(node->Text+AnsiString('\\'),0);
            node=node->Parent;
        }
        if (bFull){
        	if (DWORD(select_item->Data)==TYPE_OBJECT) name+=select_item->Text;
            else return false;
        }
        return true;
    }else{
		name = "";
        return false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::GenerateFolderName(TElTreeItem* node,AnsiString& name){
	name = "folder";
    int cnt = 0;
    while (FindFolderItem(node,name))
    	name.sprintf("%s_%04d","folder",cnt++);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CreateFolder1Click(TObject *Sender)
{
	AnsiString folder;
    GenerateFolderName(tvShaders->Selected,folder);
	TElTreeItem* node = tvShaders->Items->AddChildObject(tvShaders->Selected,folder,(LPVOID)TYPE_FOLDER);
    if (tvShaders->Selected) tvShaders->Selected->Expand(false);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExpandAll1Click(TObject *Sender)
{
	tvShaders->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CollapseAll1Click(TObject *Sender)
{
	tvShaders->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvShadersAfterSelectionChange(TObject *Sender)
{
//	if (Item!=tvShaders->Selected)
    {
    	AnsiString name;
	    MakeName(tvShaders->Selected, name, true);
    	SHTools.SetCurrentBlender(name.c_str());
    }
}
//---------------------------------------------------------------------------

