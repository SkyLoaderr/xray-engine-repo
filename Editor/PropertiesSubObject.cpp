//---------------------------------------------------------------------------
#include "Pch.h"
#pragma hdrstop

#include "PropertiesSubObject.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "FileSystem.h"
#include "gizmo.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma link "multiobj"
#pragma link "RXCtrls"
#pragma link "ToolEdit"
#pragma link "Placemnt"
#pragma link "DualList"
#pragma resource "*.dfm"
TfrmPropertiesSubObject *frmPropertiesSubObject;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesSubObject::TfrmPropertiesSubObject(TComponent* Owner)
    : TForm(Owner)
{
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsMeshProp->IniFileName = buf;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Drag&Drop
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSubObject::tvTLayersStartDrag(
      TObject *Sender, TDragObject *&DragObject)
{
    FDragItem = tvTLayers->ItemFocused;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSubObject::tvTLayersDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    TElTreeItem* node;
    Accept = false;
    if (Source != tvTLayers) return;
    node = tvTLayers->GetItemAtY(Y);
    if (node&&(!node->IsUnder(FDragItem))) Accept = true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesSubObject::tvTLayersDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
    TElTreeItem* node;
    node = tvTLayers->GetItemAtY(Y);
    if (node&&(!node->IsUnder(FDragItem))){
        FDragItem->MoveToIns(node->Parent, node->Index);
        OnModified();
        m_EditMesh->m_Layers.clear();
        for ( TElTreeItem* node = tvTLayers->Items->GetFirstNode(); node; node = node->GetNext())
            m_EditMesh->m_Layers.push_back((CTextureLayer*)node->Data);
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::FormShow(TObject *Sender)
{
    ebOk->Enabled = false;
    pcMesh->ActivePage = tsInfo;
}
//---------------------------------------------------------------------------

int __fastcall TfrmPropertiesSubObject::Edit(Mesh* mesh)
{
    m_EditMesh = mesh;              VERIFY(m_EditMesh);
    m_SaveMesh = new Mesh(mesh);    VERIFY(m_SaveMesh);
    return ShowModal();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::tsInfoShow(TObject *Sender)
{
    lbVertices->Caption = m_EditMesh->GetVertexCount();
    lbFaces->Caption = m_EditMesh->GetFaceCount();
    lbTLayers->Caption = m_EditMesh->GetLayerCount();
}
//---------------------------------------------------------------------------

void TfrmPropertiesSubObject::AddLayerToList(CTextureLayer* l){
    AnsiString name;
    name.sprintf("%s (%s)", l->m_LayerName,(l->m_Texture)?l->m_Texture->name():"no texture");
    tvTLayers->Items->AddObject(0,name,l);
}
//---------------------------------------------------------------------------

void TfrmPropertiesSubObject::RemoveLayerFromList(CTextureLayer* l){
    // disable response change item
    tvTLayers->OnItemSelectedChange = 0;
    for ( TElTreeItem* node = tvTLayers->Items->GetFirstNode(); node; node = node->GetNext())
        if (node->Data==l){
            tvTLayers->Items->Delete(node);
            break;
        }
    tvTLayers->Selected = 0;
    // enable response change item
    tvTLayers->OnItemSelectedChange = tvTLayersItemSelectedChange;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::tsLayersShow(TObject *Sender)
{
    tvTLayers->Items->Clear();
    FSelectedLayer = 0;
    for(TLayerIt l=m_EditMesh->m_Layers.begin(); l!=m_EditMesh->m_Layers.end();l++)
        AddLayerToList(*l);
    Update();
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesSubObject::ebCancelClick(TObject *Sender)
{
    m_SaveMesh->CloneFrom(m_EditMesh);
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::ebOkClick(TObject *Sender)
{
    Close();
    ModalResult=mrOk;
}
//---------------------------------------------------------------------------

int __fastcall TfrmPropertiesSubObject::FindInCBClass(unsigned __int64 cls){
    char buf[10]; CLSID2TEXT(FSelectedLayer->m_CLS,buf);
    AnsiString c_name = buf;
    c_name = c_name.Trim();
    for (int i = 0; i<cbClassID->Items->Count; i++)
        if (c_name.AnsiCompareIC(cbClassID->Items->Strings[i].Trim())==0) return i;
    return -1;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::Update()
{
    if (FSelectedLayer){
        edName->Text = FSelectedLayer->m_LayerName;  edName->MaxLength = MAX_LAYER_NAME;
        edScript->Text = FSelectedLayer->m_Script;   edScript->MaxLength = SCRIPT_MAX_SIZE;
        rbShader->ItemIndex = FSelectedLayer->m_SOP;
        cbClassID->ItemIndex = FindInCBClass(FSelectedLayer->m_CLS);
        edTextureName->Text = (FSelectedLayer->m_Texture)?FSelectedLayer->m_Texture->name():"no texture";
//        Fmaterial           m_Mat;      // d3d material
        DrawTexture(); paImage->Repaint();
// update list
        for ( TElTreeItem* node = tvTLayers->Items->GetFirstNode(); node; node = node->GetNext())
            if (node->Data==FSelectedLayer){
                AnsiString name;
                name.sprintf("%s (%s)", FSelectedLayer->m_LayerName,(FSelectedLayer->m_Texture)?FSelectedLayer->m_Texture->name():"no texture");
                node->Text = name;
                break;
            }
        edName->Enabled = true;
        edScript->Enabled = true;
        rbShader->Enabled = true;
        cbClassID->Enabled = true;
        edTextureName->Enabled = true;
        paImage->Enabled = true;
    }else{
        edName->Text = "";          edName->Enabled = false;
        edScript->Text = "";        edScript->Enabled = false;
        rbShader->ItemIndex = -1;   rbShader->Enabled = false;
        cbClassID->ItemIndex = -1;  cbClassID->Enabled = false;
        edTextureName->Text = "";   edTextureName->Enabled = false;
        paImage->Repaint();         paImage->Enabled = false;
    }
}
//---------------------------------------------------------------------------

void TfrmPropertiesSubObject::DrawTexture(){
    if (FSelectedLayer->m_Texture){
        RECT r; r.left = 1; r.top = 1;
        float w, h;
        w = FSelectedLayer->m_Texture->width();
        h = FSelectedLayer->m_Texture->height();
        if (w>h){   r.right = pbImage->Width; r.bottom = h/w*pbImage->Height;
        }else{      r.right = h/w*pbImage->Width; r.bottom = pbImage->Height;}
        HDC hdc = GetDC(paImage->Handle);
        FSelectedLayer->m_Texture->stretchthumb(hdc, &r);
        ReleaseDC(paImage->Handle,hdc);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::OnModified()
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::ebAddLayerClick(TObject *Sender)
{
    CTextureLayer* layer = m_EditMesh->AddTextureLayer();
    AddLayerToList(layer);
    OnModified();
    Update();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::ebDeleteLayerClick(TObject *Sender)
{
    if (FSelectedLayer){
        RemoveLayerFromList(FSelectedLayer);
        m_EditMesh->RemoveTextureLayer(&FSelectedLayer);
        OnModified();
        Update();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    tvTLayers->Items->Clear();
    FSelectedLayer = 0;
    SAFE_DELETE(m_SaveMesh);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::tvTLayersItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
    FSelectedLayer = (Item)?(CTextureLayer*)Item->Data:0;
    Update();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::edNameChange(TObject *Sender)
{
    if (FSelectedLayer){
        strcpy(FSelectedLayer->m_LayerName,edName->Text.c_str());
        OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::edScriptChange(TObject *Sender)
{
    if (FSelectedLayer){
        strcpy(FSelectedLayer->m_Script,edScript->Text.c_str());
        OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::ebTextureClick(TObject *Sender)
{
    if (FSelectedLayer){
        char buffer[MAX_PATH]="";
        if (FS.getopenname(&FS.m_Pictures,buffer)){
            AnsiString tname = ExtractFileName(buffer);
            ETexture* m_Texture = new ETexture( tname.c_str() );
            if (m_Texture->valid()){
                SAFE_DELETE(FSelectedLayer->m_Texture);
                FSelectedLayer->m_Texture = m_Texture;
                DrawTexture(); paImage->Repaint();
                Update();
                OnModified();
            }else{
                MessageDlg("Texture can't load!", mtError, TMsgDlgButtons() << mbOK, 0);
                SAFE_DELETE(m_Texture);
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::ebDestroyTexClick(TObject *Sender)
{
    if (FSelectedLayer){
        SAFE_DELETE(FSelectedLayer->m_Texture);
        DrawTexture(); paImage->Repaint();
        Update();
        OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::pbImagePaint(TObject *Sender)
{
    if (FSelectedLayer){
        DrawTexture();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::rbShaderClick(TObject *Sender)
{
    if (FSelectedLayer){
        FSelectedLayer->m_SOP = (EShaderOP)rbShader->ItemIndex;
        OnModified();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::cbClassIDChange(TObject *Sender)
{
    if (FSelectedLayer){
        FSelectedLayer->m_CLS = TEXT2CLSID(cbClassID->Text.c_str());
        OnModified();
    }
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesSubObject::ebFaceListClick(TObject *Sender)
{
    if (FSelectedLayer){
        dldFaces->List1->Clear();
        dldFaces->List2->Clear();
        for(int i=0; i<m_EditMesh->GetFaceCount(); i++)
            dldFaces->List1->AddObject(Format("Face %d", ARRAYOFCONST(((int)i))),(TObject*)i);
        for(DWORDIt _F=FSelectedLayer->m_GFaceIDs.begin(); _F!=FSelectedLayer->m_GFaceIDs.end();_F++){
            dldFaces->List2->AddObject(Format("Face %d", ARRAYOFCONST(((int)*_F))),(TObject*)(*_F));
            int id = dldFaces->List1->IndexOfObject((TObject*)(*_F));
            if (id>=0) dldFaces->List1->Delete(id);
        }
        if (dldFaces->Execute()){
            for(i=0; i<dldFaces->List1->Count; i++)
                FSelectedLayer->DelFace((DWORD)dldFaces->List1->Objects[i]);
            for(i=0; i<dldFaces->List2->Count; i++)
                FSelectedLayer->AddFace((DWORD)dldFaces->List2->Objects[i]);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesSubObject::tvTLayersDblClick(TObject *Sender)
{
    ebFaceListClick(Sender);
}
//---------------------------------------------------------------------------


