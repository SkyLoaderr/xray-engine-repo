#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Main.h"
#include "main.h"
#include "Blender.h"
#include "xr_trims.h"
#include "UI_Tools.h"
#include "FolderLib.h"
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
__fastcall TfraLeftBar::TfraLeftBar(TComponent* Owner)
        : TFrame(Owner)
{
	DEFINE_INI(fsStorage);

    InplaceEngineEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceEngineEdit->Editor->BorderStyle	= bsNone;
    InplaceCompilerEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceCompilerEdit->Editor->BorderStyle= bsNone;
    DragItem = 0;
    frmMain->paLeftBar->Width = paLeftBar->Width+2;
    frmMain->sbToolsMin->Left = paLeftBar->Width-frmMain->sbToolsMin->Width-3;
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
	UI.Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebReloadClick(TObject *Sender)
{
	UI.Command( COMMAND_RELOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRefreshTexturesClick(TObject *Sender)
{
	UI.Command( COMMAND_REFRESH_TEXTURES );
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
	UI.Command(COMMAND_EDITOR_PREF);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebResetAnimationClick(TObject *Sender)
{
	UI.Command( COMMAND_RESET_ANIMATION );
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
	UI.Command( COMMAND_SHADER_PROPERTIES );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::PreviewClick(TObject *Sender)
{
	UI.Command( COMMAND_SELECT_PREVIEW_OBJ, dynamic_cast<TMenuItem*>(Sender)->Tag );
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
	FOLDER::MakeName(tvEngine->Selected,0,folder,true);
    CBlender* B = Tools.SEngine.AppendBlender(((CBlender*)mi->Tag)->getDescription().CLS,folder.c_str(),0);
	Tools.SEngine.SetCurrentBlender(B);
	Tools.SEngine.Modified();
	UI.Command(COMMAND_SHADER_PROPERTIES);
}
//---------------------------------------------------------------------------

void TfraLeftBar::ClearEShaderList(){
	tvEngine->Items->Clear();
}
//---------------------------------------------------------------------------

void TfraLeftBar::ClearCShaderList(){
    tvCompiler->Items->Clear();
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddBlender(LPCSTR full_name, bool bLoadMode){
	TElTreeItem* node = FOLDER::AppendObject(tvEngine,full_name);
    if (!bLoadMode){
	    if (node&&node->Parent) node->Parent->Expand(false);
    	node->Selected = true;
		tvEngine->Selected = node;
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddCShader(LPCSTR full_name, bool bLoadMode){    
	TElTreeItem* node = FOLDER::AppendObject(tvCompiler,full_name);
    if (!bLoadMode){
	    if (node&&node->Parent) node->Parent->Expand(false);
    	node->Selected = true;
		tvCompiler->Selected = node;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CreateFolder1Click(TObject *Sender)
{
	AnsiString folder;
    AnsiString start_folder;
    FOLDER::MakeName(CurrentView()->Selected,0,start_folder,true);
    FOLDER::GenerateFolderName(CurrentView(),CurrentView()->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = FOLDER::AppendFolder(CurrentView(),folder.c_str());
    if (CurrentView()->Selected) CurrentView()->Selected->Expand(false);
    CurrentView()->EditItem(node,-1);
	Tools.Modified();
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
	if (CurrentView()->Selected&&FOLDER::IsObject(CurrentView()->Selected))
		ebEngineShaderPropertiesClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderRemoveClick(TObject *Sender)
{
    TElTreeItem* pNode = tvEngine->Selected;
    if (pNode){
		AnsiString full_name;
    	if (FOLDER::IsFolder(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    FOLDER::MakeName(item,0,full_name,false);
                	if (FOLDER::IsObject(item)) Tools.SEngine.RemoveBlender(full_name.c_str());
                }
				Tools.SEngine.ResetCurrentBlender();
                Tools.SEngine.ApplyChanges(true);
	            pNode->Delete();
                Tools.SEngine.Modified();
        	}
        }
    	if (FOLDER::IsObject(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected blender?") == mrYes){
				FOLDER::MakeName(pNode,0,full_name,false);
	            Tools.SEngine.RemoveBlender(full_name.c_str());
				Tools.SEngine.ResetCurrentBlender();
                Tools.SEngine.ApplyChanges(true);
	            pNode->Delete();
                Tools.SEngine.Modified();
        	}
        }
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineItemFocused(TObject *Sender)
{
	if (pcShaders->ActivePage==tsEngine){
    	AnsiString name;
    	FOLDER::MakeName(tvEngine->Selected, 0, name, false);
	    Tools.SEngine.SetCurrentBlender(name.c_str());
    }else if (pcShaders->ActivePage==tsCompiler){
    	AnsiString name;
    	FOLDER::MakeName(tvCompiler->Selected, 0, name, false);
	    Tools.SCompiler.SetCurrentShader(name.c_str());
    }

}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebEngineShaderCloneClick(TObject *Sender)
{
    TElTreeItem* pNode = tvEngine->Selected;
    if (pNode&&FOLDER::IsObject(pNode)){
		AnsiString full_name;
		FOLDER::MakeName(pNode,0,full_name,false);
        Tools.SEngine.CloneBlender(full_name.c_str());
		UI.Command(COMMAND_SHADER_PROPERTIES);
		Tools.SEngine.Modified();
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
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
    switch(Tools.ActiveEditor()){
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
    if (FOLDER::IsFolder(node)){
        for (item=node->GetFirstChild(); item&&(item->Level>node->Level); item=item->GetNext()){
            if (FOLDER::IsObject(item)){
                FOLDER::MakeName(item,0,full_name,false);
                switch (Tools.ActiveEditor()){
                case aeEngine: 		Tools.SEngine.RenameBlender(full_name.c_str(),new_text.c_str(),node->Level); break;
                case aeCompiler: 	Tools.SCompiler.RenameShader(full_name.c_str(),new_text.c_str(),node->Level); break;
                }
            }
        }
    }else if (FOLDER::IsObject(node)){
        FOLDER::MakeName(node,0,full_name,false);
        switch (Tools.ActiveEditor()){
        case aeEngine: 		Tools.SEngine.RenameBlender(full_name.c_str(),new_text.c_str(),node->Level); break;
        case aeCompiler: 	Tools.SCompiler.RenameShader(full_name.c_str(),new_text.c_str(),node->Level); break;
        }
    }
	Tools.Modified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCShaderCreateClick(TObject *Sender)
{
    AnsiString folder;
	FOLDER::MakeName(tvCompiler->Selected,0,folder,true);
    Shader_xrLC* S = Tools.SCompiler.AppendShader(folder.c_str(),0);
	Tools.SCompiler.SetCurrentShader(S);
	Tools.SCompiler.Modified();
	UI.Command(COMMAND_SHADER_PROPERTIES);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::pcShadersChange(TObject *Sender)
{
	Tools.OnChangeEditor();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCompilerShaderRemoveClick(TObject* Sender)
{
    TElTreeItem* pNode = tvCompiler->Selected;
    if (pNode){
		AnsiString full_name;
    	if (FOLDER::IsFolder(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    FOLDER::MakeName(item,0,full_name,false);
                	if (FOLDER::IsObject(item)) Tools.SCompiler.RemoveShader(full_name.c_str());
                }
				Tools.SCompiler.ResetCurrentShader();
	            pNode->Delete();
                Tools.SCompiler.Modified();
        	}
        }
    	if (FOLDER::IsObject(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected blender?") == mrYes){
				FOLDER::MakeName(pNode,0,full_name,false);
	            Tools.SCompiler.RemoveShader(full_name.c_str());
				Tools.SCompiler.ResetCurrentShader();
	            pNode->Delete();
                Tools.SCompiler.Modified();
        	}
        }
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCompilerShaderCloneClick(TObject *Sender)
{
    TElTreeItem* pNode = tvCompiler->Selected;
    if (pNode&&FOLDER::IsObject(pNode)){
		AnsiString full_name;
		FOLDER::MakeName(pNode,0,full_name,false);
		Tools.SCompiler.SetCurrentShader(Tools.SCompiler.CloneShader(full_name.c_str()));
		UI.Command(COMMAND_SHADER_PROPERTIES);
		Tools.SCompiler.Modified();
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	if (tv->ItemFocused) 	DragItem = tv->ItemFocused;
  	else					DragItem = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvEngineDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	TElTreeItem* tgt;
    TElTreeItem* src=DragItem;
    TSTItemPart IP;
    int HCol;
    if (!DragItem) Accept = false;
  	else{
		tgt = tv->GetItemAt(X, Y, IP, HCol);
        if (tgt){
        	if (FOLDER::IsFolder(src)){
            	if (FOLDER::IsFolder(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt);
                }else if (FOLDER::IsObject(tgt)){
		        	Accept = (tgt!=src)&&(tgt->Parent!=src->Parent)&&(src!=tgt->Parent);
                }
            }else if (FOLDER::IsObject(src)){
            	if (FOLDER::IsFolder(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt);
                }else if (FOLDER::IsObject(tgt)){
		        	Accept = (tgt!=src)&&(src->Parent!=tgt->Parent);
                }
            }
        }else Accept = !!DragItem->Parent;
    }
}
//---------------------------------------------------------------------------
void __fastcall TfraLeftBar::tvEngineDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TElTree* tv = dynamic_cast<TElTree*>(Sender); VERIFY(Sender);
	TElTreeItem* tgt_folder = tv->GetItemAt(X, Y, 0, 0);
    if (tgt_folder&&(FOLDER::IsObject(tgt_folder))) tgt_folder=tgt_folder->Parent;

    AnsiString base_name;
    FOLDER::MakeName(tgt_folder,0,base_name,true);
    AnsiString cur_fld_name=base_name;
    TElTreeItem* cur_folder=tgt_folder;

    int drg_level=DragItem->Level;

    TElTreeItem* item = DragItem;
    do{
    	DWORD type = DWORD(item->Data);
		TElTreeItem* pNode = FOLDER::FindItemInFolder(type,CurrentView(),cur_folder,item->Text);
		if (pNode&&FOLDER::IsObject(item)){
            item=item->GetNext();
        	continue;
        }

        if (!pNode) pNode = CurrentView()->Items->AddChildObject(cur_folder,item->Text,(TObject*)type);
		if (FOLDER::IsFolder(item)){
        	cur_folder = pNode;
		    FOLDER::MakeName(cur_folder,0,cur_fld_name,true);
            item=item->GetNext();
        }else{
        	// rename
		    AnsiString old_name, new_name;
		    FOLDER::MakeName(item,0,old_name,false);
		    FOLDER::MakeName(pNode,0,new_name,false);
            switch (Tools.ActiveEditor()){
            case aeEngine: 		Tools.SEngine.RenameBlender(old_name.c_str(),new_name.c_str()); 	break;
            case aeCompiler: 	Tools.SCompiler.RenameShader(old_name.c_str(),new_name.c_str()); 	break;
            }
            Tools.Modified();

            TElTreeItem* parent=item->Parent;
            // get next item && delete existance
            TElTreeItem* next=item->GetNext();
			item->Delete();

            if (parent&&((parent->GetLastChild()==item)||(0==parent->ChildrenCount))){
	            if (0==parent->ChildrenCount) parent->Delete();
	        	cur_folder = cur_folder->Parent;
            }

            item=next;
        }
    }while(item&&(item->Level>drg_level));
}
//---------------------------------------------------------------------------


