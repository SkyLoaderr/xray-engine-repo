#include "pch.h"
#pragma hdrstop

#include "ui_tools.h"
#include "SelectTexturing.h"
#include "sceneobject.h"
#include "scene.h"
#include "sceneclasslist.h"
#include "ui_main.h"
#include "texturizer.h"
#include "SObject2.h"
#include "StaticMesh.h"
#include "texture.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "RXCtrls"
#pragma link "multiobj"
#pragma link "RxMenus"
#pragma resource "*.dfm"
TfraSelectTexturing *fraSelectTexturing;

#define LAYER_TAG_OFFS 0xAD00

//---------------------------------------------------------------------------
__fastcall TfraSelectTexturing::TfraSelectTexturing(TComponent* Owner)
        : TFrame(Owner)
{
    UI.m_Tools->AddFrame(etTexturing,eaSelect,this);
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectTexturing::PanelMinClick(TObject *Sender)
{
    PanelMinimizeClick(Sender);
}
//---------------------------------------------------------------------------
void __fastcall TfraSelectTexturing::TopClick(TObject *Sender)
{
    PanelMaximizeOnlyClick(Sender);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::UpdateSelection()
{
    lbObjectName->Caption   = (Texturizer.GetObjectName())?Texturizer.GetObjectName():"<none>";
    lbMeshName->Caption     = (Texturizer.GetMeshName())?Texturizer.GetMeshName():"<none>";
    lbLayerName->Caption    = (Texturizer.GetLayerName())?Texturizer.GetLayerName():"<none>";
}

void __fastcall TfraSelectTexturing::ebSelectObjectClick(TObject *Sender)
{
//    Texturizer.m_EditObject = 
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::ebSelectMeshClick(TObject *Sender)
{
    if (Texturizer.GetObject()){
    }
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::ebSelectLayerClick(TObject *Sender)
{
    SelectLayer(-1);
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::SceneUpdateClick(TObject *Sender)
{
    ebFit->Click();
    UI.UpdateScene();
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::SelectLayer(int face_id)
{
    if (Texturizer.GetObject()&&Texturizer.GetMesh()){
        // clear all menu items
        miSelectLayer->Clear();
        // fill layers menu
        DWORD l_cnt = Texturizer.GetMesh()->m_Mesh->GetLayerCount();
        if (l_cnt){
            TMenuItem* mi;
            for (DWORD i=0; i<l_cnt; i++){
                CTextureLayer* l = Texturizer.GetMesh()->m_Mesh->GetLayer(i);
                if (l->ContainsFace(face_id)||(face_id<0)){
                    AnsiString name;
                    name.sprintf("%s (%s)", l->m_LayerName,(l->m_Texture)?l->m_Texture->name():"no texture");
                    mi = new TMenuItem(miSelectLayer);
                    mi->Tag = i+LAYER_TAG_OFFS;
                    mi->Caption = name;
                    mi->OnClick = miSelectLayerClick;
                    miSelectLayer->Add(mi);
                }
            }
            if (face_id>=0){
                mi = new TMenuItem(miSelectLayer);
                mi->Caption = "-";
                miSelectLayer->Add(mi);
                for (DWORD i=0; i<l_cnt; i++){
                    CTextureLayer* l = Texturizer.GetMesh()->m_Mesh->GetLayer(i);
                    if (!l->ContainsFace(face_id)){
                        AnsiString name;
                        name.sprintf("%s (%s)", l->m_LayerName,(l->m_Texture)?l->m_Texture->name():"no texture");
                        mi = new TMenuItem(miSelectLayer);
                        mi->Tag = i+LAYER_TAG_OFFS;
                        mi->Caption = name;
                        mi->OnClick = miSelectLayerClick;
                        miSelectLayer->Add(mi);
                    }
                }
            }
        }
        POINT pt; GetCursorPos(&pt); UI.UpdateScene(true);
        pmLayers->Popup(pt.x,pt.y);
    }else{
        MessageDlg("Select Object and Mesh.", mtError, TMsgDlgButtons() << mbOK, 0);
    }
}
void __fastcall TfraSelectTexturing::miSelectLayerClick(TObject *Sender)
{
    TMenuItem* mi = dynamic_cast<TMenuItem*>(Sender);
    if (mi){
        DWORD id = mi->Tag-LAYER_TAG_OFFS;
        Texturizer.SetEditLayer(Texturizer.GetMesh()->m_Mesh->GetLayer(id));
    }
}
//---------------------------------------------------------------------------


void __fastcall TfraSelectTexturing::miRemoveSelectedClick(TObject *Sender)
{
    Texturizer.RemoveEditLayer();
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::miAddNewLayerClick(TObject *Sender)
{
    Texturizer.AddNewLayer();
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::miPropertiesClick(TObject *Sender)
{
    Texturizer.Edit();
}
//---------------------------------------------------------------------------

void __fastcall TfraSelectTexturing::ebFitClick(TObject *Sender)
{
    Texturizer.Fit();    
}
//---------------------------------------------------------------------------

