#include "stdafx.h"
#pragma hdrstop

#include "ui_tools.h"
#include "FrameAIMap.h"
#include "Scene.h"
#include "ESceneAIMapTools.h"
#include "UI_AIMapTools.h"
#include "FolderLib.h"
#include "ui_main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma link "ElTree"
#pragma link "ElXPThemedControl"
#pragma link "ElPanel"
#pragma link "ElSplit"
#pragma link "multi_edit"
#pragma resource "*.dfm"

//---------------------------------------------------------------------------
__fastcall TfraAIMap::TfraAIMap(TComponent* Owner, ESceneAIMapTools* _tools)
        : TFrame(Owner)
{   
	tools = _tools;  R_ASSERT(tools);
}
//---------------------------------------------------------------------------
void __fastcall TfraAIMap::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ExpandClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ebGenerateMapClick(TObject *Sender)
{
	tools->GenerateMap();
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ebSmoothNodesClick(TObject *Sender)
{
	tools->SmoothNodes();
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ebRemove0LNodesClick(TObject *Sender)
{
	tools->RemoveInvalidNodes(0); 
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ebRemove1LNodesClick(TObject *Sender)
{
	tools->RemoveInvalidNodes(1); 
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ebChangeCurrentClick(TObject *Sender)
{
	Tools.SetSubTarget		(dynamic_cast<TExtBtn*>(Sender)->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::fsStorageRestorePlacement(TObject *Sender)
{
//.	Tools.SetSubTarget				(estAIMapNode);
	tools->m_VisRadius		= seVisRadius->Value;
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ebDrawSnapObjectsClick(TObject *Sender)
{
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::ebInvertLinkClick(TObject *Sender)
{
	tools->InvertLinks();
    Scene.UndoSave();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

static const int idx[5][4]={
	{0,1,2,3},
	{1,2,3,0},
	{2,3,0,1},
	{3,0,1,2},
	{4,4,4,4},
};

int ConvertV2L(int side)
{
	if (side<4){
        const Fvector& HPB = Device.m_Camera.GetHPB();
        float h = angle_normalize(HPB.x)/PI; R_ASSERT((h>=0.f)&&(h<=2.f));
        if (h>0.25f&&h<=0.75f)		return idx[3][side];
        else if (h>0.75f&&h<=1.25f)	return idx[2][side];
        else if (h>1.25f&&h<=1.75f)	return idx[1][side];
        else return idx[0][side];
    }else return side;
}
                        
static const int fl[5]	=	{SAINode::flN1,SAINode::flN2,SAINode::flN3,SAINode::flN4,
    					 	 SAINode::flN1|SAINode::flN2|SAINode::flN3|SAINode::flN4,
//    					 	 SAINode::flN1|SAINode::flN2,SAINode::flN2|SAINode::flN3,
//    					 	 SAINode::flN3|SAINode::flN4,SAINode::flN4|SAINode::flN1
                             };
void __fastcall TfraAIMap::ebSideClick(TObject *Sender)
{
	ESceneAIMapTools::EMode mode;    
    if (ebModeAppend->Down) 		mode = ESceneAIMapTools::mdAppend;
    else if (ebModeRemove->Down)	mode = ESceneAIMapTools::mdRemove;
    else if (ebModeInvert->Down)	mode = ESceneAIMapTools::mdInvert;
    TExtBtn* btn = dynamic_cast<TExtBtn*>(Sender); R_ASSERT(btn);
	tools->MakeLinks(fl[ConvertV2L(btn->Tag)], mode);
    Scene.UndoSave();
    UI.RedrawScene();      
}
//---------------------------------------------------------------------------

void __fastcall TfraAIMap::seVisRadiusChange(TObject *Sender)
{
	tools->m_VisRadius		= seVisRadius->Value;
    UI.RedrawScene					();
}
//---------------------------------------------------------------------------



