//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "BonePart.h"
#include "Bone.h"
#include "UI_Tools.h"
#include "FolderLib.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfrmBonePart *frmBonePart;
TElTree* T[4];
TEdit* E[4];
//---------------------------------------------------------------------------
bool TfrmBonePart::Run(CEditableObject* object)
{
	int res = mrCancel;
	if (object){
        m_EditObject = object;
        m_BoneParts = &object->BoneParts();
        res = ShowModal();
    }else{
    	ELog.DlgMsg(mtError,"Scene empty. Load object first.");
    }
    return (res==mrOk);
}
//---------------------------------------------------------------------------
__fastcall TfrmBonePart::TfrmBonePart(TComponent* Owner)
	: TForm(Owner)
{
    T[0] = tvPart1;
    T[1] = tvPart2;
    T[2] = tvPart3;
    T[3] = tvPart4;
    E[0] = edPart1Name;
    E[1] = edPart2Name;
    E[2] = edPart3Name;
    E[3] = edPart4Name;
}
//---------------------------------------------------------------------------
void __fastcall TfrmBonePart::FormShow(TObject *Sender)
{
	FillBoneParts();
}
//---------------------------------------------------------------------------

void __fastcall TfrmBonePart::FillBoneParts()
{
    for (int k=0; k<4; k++) T[k]->IsUpdating = true;
    for (k=0; k<4; k++){T[k]->Items->Clear();E[k]->Text="";}
	for (BPIt it=m_BoneParts->begin(); it!=m_BoneParts->end(); it++){
        E[it-m_BoneParts->begin()]->Text = it->alias;
        for (INTIt w_it=it->bones.begin(); w_it!=it->bones.end(); w_it++)
        	FOLDER::AppendObject(T[it-m_BoneParts->begin()],m_EditObject->BoneNameByID(*w_it));
    }
    for (k=0; k<4; k++) T[k]->IsUpdating = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmBonePart::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
	if (Key==VK_ESCAPE)	return;
}
//---------------------------------------------------------------------------

void __fastcall TfrmBonePart::tvPartStartDrag(TObject *Sender,
      TDragObject *&DragObject)
{
	for (TElTreeItem* node=((TElTree*)Sender)->GetNextSelected(0); node; node=((TElTree*)Sender)->GetNextSelected(node))
		FDragItems.push_back(node);
}
//---------------------------------------------------------------------------

void __fastcall TfrmBonePart::tvPartDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	for (int k=0; k<FDragItems.size(); k++){
		FOLDER::AppendObject(((TElTree*)Sender),AnsiString(FDragItems[k]->Text).c_str());
        if (ebMoveMode->Down) FDragItems[k]->Delete();
    }
    FDragItems.clear();
}
//---------------------------------------------------------------------------

void __fastcall TfrmBonePart::tvPartDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    Accept = false;
    if (Source == Sender) return;
    Accept = true;
}
//---------------------------------------------------------------------------


void __fastcall TfrmBonePart::ebSaveClick(TObject *Sender)
{
	for (int k=0; k<4; k++){
    	if (T[k]->Items->Count&&E[k]->Text.IsEmpty()){
        	ShowMessage("Verify parts name.");
        	return;
        }
        for (int i=k-1; i>=0; i--){
	    	if (!T[k]->Items->Count) continue;
        	if (E[k]->Text.UpperCase()==E[i]->Text.UpperCase()){
                ShowMessage("Unique name required.");
                return;
            }
        }
    }
	m_BoneParts->clear();
    for (k=0; k<4; k++){
    	if (T[k]->Items->Count){
            m_BoneParts->push_back(SBonePart());
            SBonePart& BP = m_BoneParts->back();
            BP.alias = E[k]->Text;
		    for ( TElTreeItem* node = T[k]->Items->GetFirstNode(); node; node = node->GetNext())
            	BP.bones.push_back(m_EditObject->BoneIDByName(AnsiString(node->Text).c_str()));
        }
    }
    Tools.MotionModified();
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmBonePart::ebCancelClick(TObject *Sender)
{
    ModalResult = mrCancel;
	Close();
}
//---------------------------------------------------------------------------

void __fastcall TfrmBonePart::ExtBtn1Click(TObject *Sender)
{
	m_BoneParts->clear();
    // fill default bone part
    m_BoneParts->push_back(SBonePart());
    SBonePart& BP = m_BoneParts->back();
    BP.alias = "default";
    for (BoneIt it=m_EditObject->FirstBone(); it!=m_EditObject->LastBone(); it++)
        BP.bones.push_back(m_EditObject->BoneIDByName((*it)->Name()));
    FillBoneParts();
}
//---------------------------------------------------------------------------

