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
#pragma link "ElTreeAdvEdit"
#pragma link "ElPgCtl"
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

TElTreeItem* TfraLeftBar::FindFolderItem(TElTree* tv, TElTreeItem* start_item, const AnsiString& name){
	if (start_item) return tv->Items->LookForItemEx(start_item,-1,false,true,false,(LPVOID)&name,FolderLookupFunc);
    else{
    	for (TElTreeItem* node=tv->Items->GetFirstNode(); node; node=node->GetNextSibling())
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
    InplaceEngineEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceEngineEdit->Editor->BorderStyle	= bsNone;
    InplaceCompilerEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceCompilerEdit->Editor->BorderStyle= bsNone;
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
    int i, j;
    for (i=0; i<fraLeftBar->ComponentCount; i++){
        TComponent* temp = fraLeftBar->Components[i];
        if (dynamic_cast<TExtBtn *>(temp) != NULL)
            ((TExtBtn*)temp)->UpdateMouseInControl();
    }
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

void __fastcall TfraLeftBar::ebEngineShaderFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmEngineShadersFile,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmPreviewObject,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbRight)	ShowPPMenu(pmShaderList,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderPropertiesClick(TObject *Sender)
{
	UI->Command( COMMAND_SHADER_PROPERTIES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PreviewClick(TObject *Sender)
{
	UI->Command( COMMAND_SELECT_PREVIEW_OBJ, dynamic_cast<TMenuItem*>(Sender)->Tag );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineApplyChangesClick(TObject *Sender)
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
		pmTemplateList->Items->Add(mi);
	}
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderCreateMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmTemplateList,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::TemplateClick(TObject *Sender)
{
	TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    AnsiString folder;
	MakeName(tvEngine->Selected,folder,false);
    CBlender* B = SHTools.Engine.AppendBlender(((CBlender*)mi->Tag)->getDescription().CLS,folder.c_str(),0);
	SHTools.Engine.SetCurrentBlender(B);
	SHTools.Engine.Modified();
	UI->Command(COMMAND_SHADER_PROPERTIES);
}
//---------------------------------------------------------------------------

TElTreeItem* TfraLeftBar::FindFolder(TElTree* tv, LPCSTR full_name){
	int cnt = _GetItemCount(full_name,'\\')-1;
    if (cnt<=0) return 0;
    int itm = 0;
	char fld[64];
	TElTreeItem* node = 0;
    do _GetItem(full_name,itm++,fld,'\\');
    while ((node = FindFolderItem(tv,node,fld))&&(itm<cnt));

	return node;
}
//---------------------------------------------------------------------------
void TfraLeftBar::ClearEShaderList(){
	tvEngine->Items->Clear();
}

void TfraLeftBar::ClearCShaderList(){
    tvCompiler->Items->Clear();
}

//---------------------------------------------------------------------------
void TfraLeftBar::AddItem(TElTree* tv, LPCSTR full_name, bool bLoadMode){
	int cnt = _GetItemCount(full_name,'\\');
    R_ASSERT2(cnt,"Name empty.");
    TElTreeItem* folder = FindFolder(tv,full_name);
	char obj[64];
	_GetItem(full_name,cnt-1,obj,'\\');
    if (!folder&&(cnt>1)){
	    int itm = 0;
        cnt--;
		char fld[64];
		TElTreeItem* node = 0;
        bool bCreate=false;
    	do{ _GetItem(full_name,itm++,fld,'\\');
            if (!bCreate) node = FindFolderItem(tv,node,fld);
            if (!node) 	bCreate = true;
        	else 		folder = node;
            if (bCreate) folder = tv->Items->AddChildObject(folder,fld,(LPVOID)TYPE_FOLDER);
	    }while (itm<cnt);
    }
	TElTreeItem* node = tv->Items->AddChildObject(folder,obj,(LPVOID)TYPE_OBJECT);
    if (!bLoadMode){
	    if (folder) folder->Expand(false);
    	node->Selected = true;
		tv->Selected = node;
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddBlender(LPCSTR full_name, bool bLoadMode){
	AddItem(tvEngine,full_name,bLoadMode);
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddCShader(LPCSTR full_name, bool bLoadMode){
	AddItem(tvCompiler,full_name,bLoadMode);
}
//---------------------------------------------------------------------------

bool TfraLeftBar::MakeName(TElTreeItem* select_item, AnsiString& name, bool bFull){
	if (select_item){
    	TElTreeItem* node = (DWORD(select_item->Data)==TYPE_OBJECT)?select_item->Parent:select_item;
        name = "";
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

void __fastcall TfraLeftBar::GenerateFolderName(TElTree* tv, TElTreeItem* node,AnsiString& name){
	name = "folder";
    int cnt = 0;
    while (FindFolderItem(tv,node,name))
    	name.sprintf("%s_%02d","folder",cnt++);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CreateFolder1Click(TObject *Sender)
{
	AnsiString folder;
    GenerateFolderName(CurrentView(),CurrentView()->Selected,folder);
	TElTreeItem* node = CurrentView()->Items->AddChildObject(CurrentView()->Selected,folder,(LPVOID)TYPE_FOLDER);
    if (CurrentView()->Selected) CurrentView()->Selected->Expand(false);
    CurrentView()->EditItem(node,-1);
	SHTools.Engine.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExpandAll1Click(TObject *Sender)
{
	CurrentView()->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CollapseAll1Click(TObject *Sender)
{
	CurrentView()->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvViewDblClick(TObject *Sender)
{
	if (CurrentView()->Selected&&((DWORD)CurrentView()->Selected->Data==TYPE_OBJECT))
		ebEngineShaderPropertiesClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderRemoveClick(TObject *Sender)
{
    TElTreeItem* pNode = tvEngine->Selected;
    if (pNode){
		AnsiString full_name;
    	if (DWORD(pNode->Data)==TYPE_FOLDER){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    MakeName(item,full_name,true);
                	if (DWORD(item->Data)==TYPE_OBJECT) SHTools.Engine.RemoveBlender(full_name.c_str());
                }
				SHTools.Engine.ResetCurrentBlender();
	            pNode->Delete();
                SHTools.Engine.Modified();
        	}
        }
    	if (DWORD(pNode->Data)==TYPE_OBJECT){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected blender?") == mrYes){
				MakeName(pNode,full_name,true);
	            SHTools.Engine.RemoveBlender(full_name.c_str());
				SHTools.Engine.ResetCurrentBlender();
	            pNode->Delete();
                SHTools.Engine.Modified();
        	}
        }
    }else{
		ELog.DlgMsg(mtInformation, "At first selected item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineItemFocused(TObject *Sender)
{
	if (pcShaders->ActivePage==tsEngine){
    	AnsiString name;
    	MakeName(tvEngine->Selected, name, true);
	    SHTools.Engine.SetCurrentBlender(name.c_str());
    }else if (pcShaders->ActivePage==tsCompiler){
    	AnsiString name;
    	MakeName(tvCompiler->Selected, name, true);
	    SHTools.Compiler.SetCurrentShader(name.c_str());
    }

}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderCloneClick(TObject *Sender)
{
    TElTreeItem* pNode = tvEngine->Selected;
    if (pNode&&(DWORD(pNode->Data)==TYPE_OBJECT)){
		AnsiString full_name;
		MakeName(pNode,full_name,true);
        CBlender* B = SHTools.Engine.CloneBlender(full_name.c_str());
		SHTools.Engine.SetCurrentBlender(B);
		UI->Command(COMMAND_SHADER_PROPERTIES);
		SHTools.Engine.Modified();
    }else{
		ELog.DlgMsg(mtInformation, "At first selected blender.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (pcShaders->ActivePage==tsEngine){
    	if (Key==VK_DELETE) ebEngineShaderRemoveClick(Sender);
    }else if (pcShaders->ActivePage==tsCompiler){
    	if (Key==VK_DELETE) ebCompilerShaderRemoveClick(Sender);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Rename1Click(TObject *Sender)
{
	TElTreeItem* node = CurrentView()->Selected;
    if (node) CurrentView()->EditItem(node,-1);
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::InplaceEngineEditValidateResult(
      TObject *Sender, bool &InputValid)
{
	TElTreeInplaceAdvancedEdit* IE=0;
    TElTree* TV=0;
    switch(SHTools.ActiveEditor()){
    case aeEngine: 		IE=InplaceEngineEdit; break;
    case aeCompiler: 	IE=InplaceCompilerEdit; break;
    }

    AnsiString new_text = AnsiString(IE->Editor->Text).LowerCase();
    IE->Editor->Text = new_text;

    TElTreeItem* node = IE->Item;
    for (TElTreeItem* item=node->GetFirstSibling(); item; item=item->GetNextSibling()){
        if ((item->Text==new_text)&&(item!=IE->Item)){
            InputValid = false;
            return;
        }
    }
    AnsiString full_name;
    if (DWORD(node->Data)==TYPE_FOLDER){
        for (item=node->GetFirstChild(); item&&(item->Level>node->Level); item=item->GetNext()){
            if (DWORD(item->Data)==TYPE_OBJECT){
                MakeName(item,full_name,true);
                switch (SHTools.ActiveEditor()){
                case aeEngine: 		SHTools.Engine.RenameBlender(full_name.c_str(),new_text.c_str(),node->Level); break;
                case aeCompiler: 	SHTools.Compiler.RenameShader(full_name.c_str(),new_text.c_str(),node->Level); break;
                }
            }
        }
    }else if (DWORD(node->Data)==TYPE_OBJECT){
        MakeName(node,full_name,true);
        switch (SHTools.ActiveEditor()){
        case aeEngine: 		SHTools.Engine.RenameBlender(full_name.c_str(),new_text.c_str(),node->Level); break;
        case aeCompiler: 	SHTools.Compiler.RenameShader(full_name.c_str(),new_text.c_str(),node->Level); break;
        }
    }
	SHTools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCShaderCreateClick(TObject *Sender)
{
    AnsiString folder;
	MakeName(tvCompiler->Selected,folder,false);
    Shader_xrLC* S = SHTools.Compiler.AppendShader(folder.c_str(),0);
	SHTools.Compiler.SetCurrentShader(S);
	SHTools.Compiler.Modified();
	UI->Command(COMMAND_SHADER_PROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::pcShadersChange(TObject *Sender)
{
	SHTools.OnChangeEditor();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCompilerShaderRemoveClick(TObject* Sender)
{
    TElTreeItem* pNode = tvCompiler->Selected;
    if (pNode){
		AnsiString full_name;
    	if (DWORD(pNode->Data)==TYPE_FOLDER){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    MakeName(item,full_name,true);
                	if (DWORD(item->Data)==TYPE_OBJECT) SHTools.Compiler.RemoveShader(full_name.c_str());
                }
				SHTools.Compiler.ResetCurrentShader();
	            pNode->Delete();
                SHTools.Compiler.Modified();
        	}
        }
    	if (DWORD(pNode->Data)==TYPE_OBJECT){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected blender?") == mrYes){
				MakeName(pNode,full_name,true);
	            SHTools.Compiler.RemoveShader(full_name.c_str());
				SHTools.Compiler.ResetCurrentShader();
	            pNode->Delete();
                SHTools.Compiler.Modified();
        	}
        }
    }else{
		ELog.DlgMsg(mtInformation, "At first selected item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCompilerShaderCloneClick(TObject *Sender)
{
    TElTreeItem* pNode = tvCompiler->Selected;
    if (pNode&&(DWORD(pNode->Data)==TYPE_OBJECT)){
		AnsiString full_name;
		MakeName(pNode,full_name,true);
		SHTools.Compiler.SetCurrentShader(SHTools.Compiler.CloneShader(full_name.c_str()));
		UI->Command(COMMAND_SHADER_PROPERTIES);
		SHTools.Compiler.Modified();
    }else{
		ELog.DlgMsg(mtInformation, "At first selected blender.");
    }
}
//---------------------------------------------------------------------------

