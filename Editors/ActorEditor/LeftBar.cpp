#include "stdafx.h"
#pragma hdrstop

#include "LeftBar.h"
#include "BottomBar.h"
#include "UI_Main.h"
#include "main.h"
#include "xr_trims.h"
#include "UI_Tools.h"
#include "FolderLib.h"
#include "EditObject.h"
#include "Motion.h"
#include "PropertiesList.h"
#include "BonePart.h"
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
#pragma link "MXCtrls"
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

LPCSTR TfraLeftBar::FirstRecentFile()
{
	if (miRecentFiles->Count>0)
    	return miRecentFiles->Items[0]->Caption.c_str();
}

//---------------------------------------------------------------------------
__fastcall TfraLeftBar::TfraLeftBar(TComponent* Owner)
        : TFrame(Owner)
{
	DEFINE_INI(fsStorage);

    InplaceParticleEdit->Editor->Color		= TColor(0x00A0A0A0);
    InplaceParticleEdit->Editor->BorderStyle= bsNone;
    frmMain->paLeftBar->Width = paLeftBar->Width+2;
    frmMain->sbToolsMin->Left = paLeftBar->Width-frmMain->sbToolsMin->Width-3;

    for (int i=5; i>=0; i--)
    {
		AnsiString recent_fn= fsStorage->ReadString	(AnsiString("RecentFiles")+AnsiString(i),"");
        if (!recent_fn.IsEmpty()) AppendRecentFile(recent_fn.c_str());
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::AppendRecentFile(LPCSTR name)
{
	R_ASSERT(miRecentFiles->Count<=5);

	for (int i = 0; i < miRecentFiles->Count; i++)
    	if (miRecentFiles->Items[i]->Caption==name){
        	miRecentFiles->Items[i]->MenuIndex = 0;
            return;
		}

	if (miRecentFiles->Count==5) miRecentFiles->Remove(miRecentFiles->Items[4]);

    TMenuItem *MI = new TMenuItem(0);
    MI->Caption = name;
    MI->OnClick = miRecentFilesClick;
    MI->Tag		= 0x1001;
    miRecentFiles->Insert(0,MI);

    miRecentFiles->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miRecentFilesClick(TObject *Sender)
{
	TMenuItem* MI = dynamic_cast<TMenuItem*>(Sender); R_ASSERT(MI&&(MI->Tag==0x1001));
    AnsiString fn = MI->Caption;
    if (Engine.FS.Exist(fn.c_str()))	UI.Command(COMMAND_LOAD,(DWORD)fn.c_str());
    else						ELog.DlgMsg(mtError, "Error reading file '%s'",fn.c_str());
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::fsStorageSavePlacement(TObject *Sender)
{
    Tools.m_ObjectProps->SaveColumnWidth(fsStorage);
    Tools.m_MotionProps->SaveColumnWidth(fsStorage);
    Tools.m_PreviewObject.SaveParams(fsStorage);
	for (int i = 0; i < miRecentFiles->Count; i++)
	{
		TMenuItem* MI = miRecentFiles->Items[i];
		fsStorage->WriteString(AnsiString("RecentFiles")+AnsiString(i),MI->Caption);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::fsStorageRestorePlacement(TObject *Sender)
{
    Tools.m_ObjectProps->RestoreColumnWidth(fsStorage);
    Tools.m_MotionProps->RestoreColumnWidth(fsStorage);
    Tools.m_PreviewObject.RestoreParams(fsStorage);
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
    if (ebRenderEngineStyle->Down){
    	ebMakePreview->Enabled = Tools.IsModified();
	    if (!Tools.IsVisualPresent()) SetRenderStyle(false);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSaveClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
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

void __fastcall TfraLeftBar::ebMakePreviewClick(TObject *Sender)
{
	UI.Command( COMMAND_MAKE_PREVIEW );
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

void __fastcall TfraLeftBar::ebActorMotionsFileMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmMotionsFile,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebSceneCommandsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmSceneFile,Sender);
}

//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebPreviewObjectClickMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	ShowPPMenu(pmPreviewObject,Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbRight)	ShowPPMenu(pmShaderList,Sender);
}
//---------------------------------------------------------------------------

void TfraLeftBar::ClearMotionList(){
	tvMotions->Items->Clear();
}
//---------------------------------------------------------------------------

void TfraLeftBar::AddMotion(LPCSTR full_name, bool bLoadMode){
	TElTreeItem* node = FOLDER::AppendObject(tvMotions,full_name);
    if (!bLoadMode){
	    if (node&&node->Parent) node->Parent->Expand(false);
    	node->Selected = true;
		tvMotions->Selected = node;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CreateFolder1Click(TObject *Sender)
{
	AnsiString folder;
    AnsiString start_folder;
    FOLDER::MakeName(tvMotions->Selected,0,start_folder,true);
    FOLDER::GenerateFolderName(tvMotions,tvMotions->Selected,folder);
    folder = start_folder+folder;
	TElTreeItem* node = FOLDER::AppendFolder(tvMotions,folder.c_str());
    if (tvMotions->Selected) tvMotions->Selected->Expand(false);
    tvMotions->EditItem(node,-1);
	Tools.MotionModified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ExpandAll1Click(TObject *Sender)
{
	tvMotions->FullExpand();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::CollapseAll1Click(TObject *Sender)
{
	tvMotions->FullCollapse();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebMotionsRemoveClick(TObject *Sender)
{
    TElTreeItem* pNode = tvMotions->Selected;
    if (pNode){
		AnsiString full_name;
    	if (FOLDER::IsFolder(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected folder?") == mrYes){
		        for (TElTreeItem* item=pNode->GetFirstChild(); item&&(item->Level>pNode->Level); item=item->GetNext()){
                    FOLDER::MakeName(item,0,full_name,false);
                	if (FOLDER::IsObject(item)) Tools.RemoveMotion(full_name.c_str());
                }
//				Tools.ResetCurrentPS();
	            pNode->Delete();
                Tools.MotionModified();
        	}
        }
    	if (FOLDER::IsObject(pNode)){
	        if (ELog.DlgMsg(mtConfirmation, "Delete selected item?") == mrYes){
				FOLDER::MakeName(pNode,0,full_name,false);
	            Tools.RemoveMotion(full_name.c_str());
//				Tools.ResetCurrentPS();
	            pNode->Delete();
                Tools.MotionModified();
        	}
        }
    }else{
		ELog.DlgMsg(mtInformation, "At first select item.");
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebMotionsClearClick(TObject *Sender)
{
	if (Tools.CurrentObject()){
    	CEditableObject* object=Tools.CurrentObject();
    	object->ClearSMotions();
        UpdateMotionList();
		Tools.MotionModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsItemFocused(TObject *Sender)
{
	AnsiString name;
    FOLDER::MakeName(tvMotions->Selected, 0, name, false);
	Tools.SetCurrentMotion(name.c_str());
	UpdateMotionProperties();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key==VK_DELETE) ebMotionsRemoveClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Rename1Click(TObject *Sender)
{
	TElTreeItem* node = tvMotions->Selected;
    if (node) tvMotions->EditItem(node,-1);
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::InplaceParticleEditValidateResult(
      TObject *Sender, bool &InputValid)
{
	TElTreeInplaceAdvancedEdit* IE=InplaceParticleEdit;

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
                Tools.RenameMotion(full_name.c_str(),new_text.c_str());//,node->Level);
            }
        }
    }else if (FOLDER::IsObject(node)){
        FOLDER::MakeName(node,0,full_name,false);
        Tools.RenameMotion(full_name.c_str(),new_text.c_str());//,node->Level);
    }
	Tools.MotionModified();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::eMotionsAppendClick(TObject *Sender)
{
    AnsiString folder,nm,fnames,full_name;
    if (Engine.FS.GetOpenName(Engine.FS.m_SMotion,fnames,true)){
	    AStringVec lst;
    	SequenceToList(lst,fnames.c_str());
        for (AStringIt it=lst.begin(); it!=lst.end(); it++){
            TElTreeItem* node=0;
            if (tvMotions->Selected&&FOLDER::IsFolder(tvMotions->Selected))
                node = tvMotions->Selected;
            FOLDER::MakeName(node,0,folder,true);
            FOLDER::GenerateObjectName(tvMotions,node,nm,ChangeFileExt(ExtractFileName(*it),"").c_str());
            full_name = folder+nm;
            if (Tools.AppendMotion(full_name.c_str(),it->c_str())){
                tvMotions->Selected = FOLDER::AppendObject(tvMotions,full_name.c_str());
                Tools.MotionModified();
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	FOLDER::StartDrag(Sender,DragObject);
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::tvMotionsDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
	FOLDER::DragOver(Sender,Source,X,Y,State,Accept);
}
//---------------------------------------------------------------------------
void __fastcall TfraLeftBar::RenameItem(LPVOID p0, LPVOID p1)
{
    Tools.RenameMotion((LPCSTR)p0,(LPCSTR)p1);
    Tools.MotionModified();
}
//---------------------------------------------------------------------------
void __fastcall TfraLeftBar::tvMotionsDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	FOLDER::DragDrop(Sender,Source,X,Y,RenameItem);
}
//---------------------------------------------------------------------------

void TfraLeftBar::UpdateMotionList()
{
	tvMotions->Items->Clear();
    if (Tools.CurrentObject()){
		SMotionVec&	lst=Tools.CurrentObject()->SMotions();
    	for (SMotionIt it=lst.begin(); it!=lst.end(); it++)
        	FOLDER::AppendObject(tvMotions,(*it)->Name());
    }
    UpdateProperties();
}
//---------------------------------------------------------------------------

void TfraLeftBar::UpdateProperties()
{
	if (Tools.CurrentObject()){
    	paObjectProperties->Enabled = true;
        Tools.FillObjectProperties();
        Tools.FillMotionProperties();
    }else{
    	paObjectProperties->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::UpdateMotionProperties()
{
	if (Tools.CurrentObject()){
        Tools.FillMotionProperties();
        CSMotion* M=Tools.CurrentObject()->GetActiveSMotion();
        if (M){
			lbCurFrames->Caption 	= M->Length();
            lbCurFPS->Caption 		= AnsiString().sprintf("%3.1f",M->FPS());
        }else{
			lbCurFrames->Caption 	= "...";
            lbCurFPS->Caption 		= "...";
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCurrentPlayClick(TObject *Sender)
{
	Tools.PlayMotion();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCurrentStopClick(TObject *Sender)
{
	Tools.StopMotion();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebCurrentPauseClick(TObject *Sender)
{
	Tools.PauseMotion();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Import1Click(TObject *Sender)
{
	UI.Command( COMMAND_IMPORT );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Load1Click(TObject *Sender)
{
	UI.Command( COMMAND_LOAD );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Clear1Click(TObject *Sender)
{
	UI.Command( COMMAND_CLEAR );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Save2Click(TObject *Sender)
{
	UI.Command( COMMAND_SAVE );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::SaevAs1Click(TObject *Sender)
{
	UI.Command( COMMAND_SAVEAS );
}
//---------------------------------------------------------------------------


void __fastcall TfraLeftBar::ebBonePartClick(TObject *Sender)
{
	frmBonePart->Run(Tools.CurrentObject());
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::LoadClick(TObject *Sender)
{
	UI.Command( COMMAND_LOAD_MOTIONS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miSaveMotionsClick(TObject *Sender)
{
	UI.Command( COMMAND_SAVE_MOTIONS );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miExportSkeletonClick(TObject *Sender)
{
	UI.Command( COMMAND_EXPORT_SKELETON );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::miExportObjectClick(TObject *Sender)
{
	UI.Command( COMMAND_EXPORT_OBJECT );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::ebRenderStyleClick(TObject *Sender)
{
	if (Sender==ebRenderEngineStyle){
		if (!Tools.IsVisualPresent()) UI.Command( COMMAND_MAKE_PREVIEW );
        if (!Tools.IsVisualPresent()) SetRenderStyle(false);
        else						  SetRenderStyle(true);
    }
}
//---------------------------------------------------------------------------

void TfraLeftBar::SetRenderStyle(bool bEngineStyle)
{
    if (Tools.IsVisualPresent()&&bEngineStyle) 	ebRenderEngineStyle->Down = true;
    else 										ebRenderEditorStyle->Down = true;
	Tools.PlayMotion();
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Custom1Click(TObject *Sender)
{
	UI.Command( COMMAND_SELECT_PREVIEW_OBJ, false );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::none1Click(TObject *Sender)
{
	UI.Command( COMMAND_SELECT_PREVIEW_OBJ, true );
}
//---------------------------------------------------------------------------

void __fastcall TfraLeftBar::Preferences1Click(TObject *Sender)
{
	UI.Command( COMMAND_PREVIEW_OBJ_PREF );
}
//---------------------------------------------------------------------------




