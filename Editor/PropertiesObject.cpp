//---------------------------------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "PropertiesSubObject.h"
#include "SceneClassList.h"
#include "SObject2.h"
#include "FileSystem.h"
#include "StaticMesh.h"
#include "Texture.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "multiobj"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma resource "*.dfm"
TfrmPropertiesObject *frmPropertiesObject;
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesObject::TfrmPropertiesObject(TComponent* Owner)
    : TForm(Owner)
{
    sgMatrix->Cells[0][0]="0"; sgMatrix->Cells[0][1]="1"; sgMatrix->Cells[0][2]="2"; sgMatrix->Cells[0][3]="3";
    sgBB->Cells[0][0]="X"; sgBB->Cells[0][1]="Y"; sgBB->Cells[0][2]="Z";
    tx_selected = 0;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormClose(TObject *Sender,
      TCloseAction &Action)
{
    SAFE_DELETE(m_EditObject);
}
//---------------------------------------------------------------------------

void TfrmPropertiesObject::GetObjectsInfo(){
	_ASSERTE(!m_Objects.empty());

    SAFE_DELETE(m_EditObject);
	bMultiSelection = (m_Objects.size()>1)?true:false;
	if( !bMultiSelection ){
        m_SourceObject = (SObject2*)m_Objects.front();  VERIFY(m_SourceObject);
        m_EditObject = new SObject2(m_SourceObject);    VERIFY(m_EditObject);
        VERIFY( m_EditObject->ClassID()==OBJCLASS_SOBJECT2 );
    }

	ObjectIt _F = m_Objects.begin();	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
	SObject2 *_O = (SObject2 *)(*_F);

	mmScript->Text = _O->GetClassScript();
	edName->Text   = _O->GetName();
	edClass->Text  = _O->GetClassName();

	cbMakeUnique->ObjFirsInit( TCheckBoxState(_O->m_MakeUnique) );
	cbMakeDynamic->ObjFirsInit( TCheckBoxState(_O->m_DynamicList) );

	_F++;
	for(;_F!=m_Objects.end();_F++){
		_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
    	_O = (SObject2 *)(*_F);
		cbMakeUnique->ObjNextInit( TCheckBoxState(_O->m_MakeUnique) );
		cbMakeDynamic->ObjNextInit( TCheckBoxState(_O->m_DynamicList) );
	}
}

bool TfrmPropertiesObject::ApplyObjectsInfo(){
    if (!bMultiSelection){
        if (!edName->Text.Length()){
            MessageDlg("Enter Object Name!", mtError, TMsgDlgButtons() << mbOK, 0);
            return false;
        }
        if (!edClass->Text.Length()){
            MessageDlg("Enter Object Class!", mtError, TMsgDlgButtons() << mbOK, 0);
            return false;
        }
        if (!tvMeshes->Items->Count){
            MessageDlg("Add mesh!", mtError, TMsgDlgButtons() << mbOK, 0);
            return false;
        }
        _ASSERTE( m_EditObject->ClassID()==OBJCLASS_SOBJECT2 );
        cbMakeUnique->ObjApply( m_SourceObject->m_MakeUnique );
        cbMakeDynamic->ObjApply( m_SourceObject->m_DynamicList );
        strcpy( m_SourceObject->GetClassScript(), mmScript->Text.c_str() );
        strcpy( m_SourceObject->GetName(), edName->Text.c_str() );
        strcpy( m_SourceObject->GetClassName(), edClass->Text.c_str() );
// update mesh list
        m_SourceObject->ClearMeshes();
        m_SourceObject->CopyMeshes(m_EditObject);
    }else{
        _ASSERTE( !m_Objects.empty() );
        SObject2 *_O = 0;
        ObjectIt _F = m_Objects.begin();
        for(;_F!=m_Objects.end();_F++){
            _ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
            _O = (SObject2 *)(*_F);
            cbMakeUnique->ObjApply( _O->m_MakeUnique );
            cbMakeDynamic->ObjApply( _O->m_DynamicList );
        }
    }
    return true;
}

//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    GetObjectsInfo();
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebEditMeshClick(TObject *Sender)
{
    if (ebEditMesh->Enabled){
        SObject2Mesh *mdef;
        TElList* l=new TElList();
        tvMeshes->AllSelected(l);
        if (l->Count){
            if (l->Count>1){
                MessageDlg("MultiSelect not supported. Please select one item and try again.", mtConfirmation, TMsgDlgButtons() << mbOK, 0);
                delete l;
                return;
            }
            TElTreeItem* node = (TElTreeItem*)l->Items[0];
            if (node && node->Data){
                mdef = (SObject2Mesh*)node->Data;
                if (frmPropertiesSubObject->Edit(mdef->m_Mesh)==mrOk) OnModified(Sender);
            }
        }
        delete l;
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebAddMeshClick(TObject *Sender)
{
   	char buffer[MAX_PATH]="";
    FS.m_Meshes.Update(buffer);
    odMesh->InitialDir = buffer;
    if (odMesh->Execute()){
        AnsiString new_name = ExtractFileName(odMesh->FileName);
        bool bEq = false;
// find existing mesh
        for ( TElTreeItem* node = tvMeshes->Items->GetFirstNode(); node; node = node->GetNext())
            if (node->Text==new_name){ bEq = true; break; }
// if not found create and append to object
        if (!bEq){
            SObject2Mesh* _O;
            _O = new SObject2Mesh();
            strcpy(_O->m_Name,new_name.c_str());
            strcpy(_O->m_FileName,new_name.c_str());
        	_O->m_Mesh=new Mesh();
            if (_O->m_Mesh->Load(new_name.c_str())){
               	m_EditObject->m_Meshes.push_back(_O);
                OnModified(Sender);
                tsMeshesShow(Sender);
            }else{
                MessageDlg("Mesh can't load. See log file.", mtError, TMsgDlgButtons() << mbOK, 0);
                SAFE_DELETE(_O);
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebDeleteMeshClick(TObject *Sender)
{
    if (MessageDlg("Are you sure to delete mesh from list?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0)==mrYes){
//        TElTreeItem* node = tvMeshes->Selected;
        for (TElTreeItem* node = tvMeshes->GetNextSelected(0); node; node=tvMeshes->GetNextSelected(node))
            if (node && node->Data){
                SObject2Mesh* del_mesh = (SObject2Mesh*)node->Data;
                VERIFY(del_mesh);
                m_EditObject->m_Meshes.remove(del_mesh);
                _DELETE(del_mesh);
                OnModified(Sender);
                tsMeshesShow(Sender);
            }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebMergeMeshClick(TObject *Sender)
{
    SObject2Mesh* basic_mesh=0;
    if (MessageDlg("Are you sure to merge meshes?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0)==mrYes){
        for (TElTreeItem* node = tvMeshes->GetNextSelected(0); node; node=tvMeshes->GetNextSelected(node))
            if (node && node->Data){
                if (basic_mesh){
                    SObject2Mesh* add_mesh = (SObject2Mesh*)node->Data;
                    VERIFY(add_mesh); VERIFY(add_mesh->m_Mesh);
                    basic_mesh->m_Mesh->Append(add_mesh->m_Mesh);
                    m_EditObject->m_Meshes.remove(add_mesh);
                    _DELETE(add_mesh);
                    OnModified(Sender);
                    tsMeshesShow(Sender);
                }else{
                    basic_mesh = (SObject2Mesh*)node->Data;
                    VERIFY(basic_mesh); VERIFY(basic_mesh->m_Mesh);
                }
            }
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::cbMakeDynamicClick(TObject *Sender)
{
    tsScript->TabVisible = cbMakeDynamic->Checked;
    OnModified(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesObject::FormShow(TObject *Sender)
{
    pcObjects->ActivePage = tsMainOptions;
    ebOk->Enabled       = false;
    tvMeshes->Enabled   = true;
    if (mode==pomAdd)   edName->Hint = "";
    else                edName->Hint = "Warning!!! Be careful before edit item library name.";
    tsMainOptionsShow   (Sender);
    tsMeshesShow        (Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tsInfoShow(TObject *Sender)
{
// Set up info
    if (!bMultiSelection){
        paMatrix->Visible = true;
        paBB->Visible = true;
    	SObject2 *_O = m_EditObject;
        if (m_EditObject->m_LibRef) _O = m_EditObject->m_LibRef;
        lbVertices->Caption = _O->GetVertexCount();
        lbFaces->Caption    = _O->GetFaceCount();
        for (int col=1; col<5; col++) for (int row=0; row<4; row++){
            AnsiString n; n.sprintf("%.3f",m_EditObject->m_Position.m[row][col-1]);
            sgMatrix->Cells[col][row] = n;
        }

        for (col=1; col<5; col++){
            Fvector p;
            switch(col){
            case 1: p.set(m_EditObject->m_Box.vmin);break;
            case 2: p.set(m_EditObject->m_Box.vmin);break;
            case 3: m_EditObject->m_Box.getsize(p);break;
            case 4: p.set(m_EditObject->m_Center);break;
            }
            for (int row=0; row<3; row++){
                AnsiString n; n.sprintf("%.3f", p.m[row]);
                sgBB->Cells[col][row] = n;
            }
        }
    }else{
    	VERIFY(!m_Objects.empty());
        paMatrix->Enabled = false;
        paBB->Enabled = false;
        for (int col=1; col<5; col++) for (int row=0; row<4; row++) sgMatrix->Cells[col][row] = "-";
        for (col=1; col<5; col++) for (row=0; row<3; row++) sgBB->Cells[col][row] = "-";
	    ObjectIt _F = m_Objects.begin();
    	SObject2 *_O = 0;
        int f_cnt=0, v_cnt=0;
    	for(;_F!=m_Objects.end();_F++){
	    	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
        	_O = (SObject2 *)(*_F);
            if (_O->m_LibRef){
                v_cnt += _O->m_LibRef->GetVertexCount();
                f_cnt += _O->m_LibRef->GetFaceCount();
            }else{
                v_cnt += _O->GetVertexCount();
                f_cnt += _O->GetFaceCount();
            }
        }
        lbVertices->Caption = IntToStr(v_cnt);
        lbFaces->Caption = IntToStr(f_cnt);
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tsTexturesShow(TObject *Sender)
{
// Set up textures
    bool bEq;
    AnsiString name;
    tvTextures->Items->Clear();
    if (!bMultiSelection){
    	SObject2 *_O = m_EditObject;
        if (m_EditObject->m_LibRef) _O = m_EditObject->m_LibRef;

        // create root tree node (object name)
        name.sprintf("%s (%s)",m_EditObject->m_Name,_O->m_Name);
        TElTreeItem* root = tvTextures->Items->AddObject(0,name,0);
        root->ParentStyle = false;
        root->Bold = true;

        SObjMeshIt it = _O->m_Meshes.begin();
        for(;it!=_O->m_Meshes.end();it++){
            for(TLayerIt l = (*it)->m_Mesh->m_Layers.begin();l!=(*it)->m_Mesh->m_Layers.end();l++){
                if ((*l)->m_Texture){
                    bEq = false;
                    for ( TElTreeItem* branch = root->GetFirstChild(); branch; branch = branch->GetNext())
                        if (branch->Text==(*l)->m_Texture->name()){ bEq = true; break; }
                    if (!bEq) tvTextures->Items->AddChildObject(root,(*l)->m_Texture->name(),(TObject*)((*l)->m_Texture));
                }
            }
        }
    }else{
    	VERIFY(!m_Objects.empty());

	    ObjectIt _F = m_Objects.begin();
    	SObject2 *_O = 0;
    	SObject2 *_OO = 0;
    	for(;_F!=m_Objects.end();_F++){
	    	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
        	_O = (SObject2 *)(*_F);

            // if item is LibRef get info from LibRef
            if (_O->m_LibRef) _OO = _O->m_LibRef; else _OO = _O;

            // create root tree node (object name)
            name.sprintf("%s (%s)",_O->m_Name,_OO->m_Name);
            TElTreeItem* root = tvTextures->Items->AddObject(0,name,0);
            root->ParentStyle = false;
            root->Bold = true;

            SObjMeshIt it = _OO->m_Meshes.begin();
            for(;it!=_OO->m_Meshes.end();it++){
                for(TLayerIt l = (*it)->m_Mesh->m_Layers.begin();l!=(*it)->m_Mesh->m_Layers.end();l++){
                    if ((*l)->m_Texture){
                        bEq = false;
                        for ( TElTreeItem* branch = root->GetFirstChild(); branch; branch = branch->GetNext())
                            if (branch->Text==(*l)->m_Texture->name()){ bEq = true; break; }
                        if (!bEq) tvTextures->Items->AddChildObject(root,(*l)->m_Texture->name(),(TObject*)((*l)->m_Texture));
                    }
                }
            }
    	}
    }
    tvTextures->FullExpand();
    tvTextures->Sort(true);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tvTexturesItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
    if (Item->Data){
        tx_selected = (ETexture*)(Item->Data);
        lbWidth->Caption = tx_selected->width();
        lbHeight->Caption = tx_selected->height();
        lbAlpha->Caption = (tx_selected->alpha())?"present":"no alpha";
        imPaint(Sender);
    }else{
        tx_selected = 0;
        lbWidth->Caption = "...";
        lbHeight->Caption = "...";
        lbAlpha->Caption = "...";
        paImage->Repaint();
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::imPaint(TObject *Sender)
{
    if (tx_selected){
        RECT r; r.left = 1; r.top = 1;
        float w, h;
        w = tx_selected->width();
        h = tx_selected->height();
        if (w>h){   r.right = im->Width; r.bottom = h/w*im->Height;
        }else{      r.right = h/w*im->Width; r.bottom = im->Height;}
        HDC hdc = GetDC(paImage->Handle);
        tx_selected->stretchthumb(hdc, &r);
        ReleaseDC(paImage->Handle,hdc);
    }
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsMeshesShow(TObject *Sender)
{
// Set up meshes
    AnsiString name;
    if (!bMultiSelection){
        tvMeshes->Items->Clear();
        if((mode==pomAdd) || (mode==pomEdit) || (mode==pomChange && !m_EditObject->IsReference())){
            ebAddMesh->Enabled      = true;
            ebDeleteMesh->Enabled   = true;
            ebEditMesh->Enabled     = true;
            ebMergeMesh->Enabled    = true;
        }else{
            ebAddMesh->Enabled      = false;
            ebDeleteMesh->Enabled   = false;
            ebEditMesh->Enabled     = false;
            ebMergeMesh->Enabled    = false;
        }
        tvMeshes->MultiSelect = true;
    	SObject2 *_O = m_EditObject;
        if (m_EditObject->m_LibRef){
    	    _O = m_EditObject->m_LibRef;
        }
        SObjMeshIt it = _O->m_Meshes.begin();

        // root (object name)
        if (m_EditObject->IsSceneItem() || m_EditObject->IsLibItem())
            name.sprintf("%s",m_EditObject->m_Name);
        else
            name.sprintf("%s (Ref: '%s')",m_EditObject->m_Name,_O->m_Name);
        TElTreeItem* root = tvMeshes->Items->AddObject(0,name,0);
        root->ParentStyle = false;
        root->Bold = true;

        for(;it!=_O->m_Meshes.end();it++){
            name = (*it)->m_FileName;
            tvMeshes->Items->AddChildObject(root,name,(TObject*)(*it));
        }
    }else{
    	VERIFY(!m_Objects.empty());

        ebAddMesh->Enabled      = false;
        ebDeleteMesh->Enabled   = false;
        ebEditMesh->Enabled     = false;
        ebMergeMesh->Enabled    = false;

        tvMeshes->Items->Clear();

	    ObjectIt _F = m_Objects.begin();
    	SObject2 *_O = 0;
    	SObject2 *_OO = 0;
    	for(;_F!=m_Objects.end();_F++){
	    	_ASSERTE( (*_F)->ClassID()==OBJCLASS_SOBJECT2 );
        	_O = (SObject2 *)(*_F);
            if (_O->m_LibRef) _OO = _O->m_LibRef; else _OO = _O;

            // root (object name)
            name.sprintf("%s (%s)",_O->m_Name,_OO->m_Name);
            TElTreeItem* root = tvMeshes->Items->AddObject(0,name,0);
            root->ParentStyle = false;
            root->Bold = true;

            SObjMeshIt it = _OO->m_Meshes.begin();
            for(;it!=_OO->m_Meshes.end();it++){
                AnsiString name;
                name.sprintf("%s (%s)", (*it)->m_FileName, _O->m_Name);
                tvMeshes->Items->AddChildObject(root,name,(TObject*)(*it));
            }
    	}
    }
    tvMeshes->FullExpand();
    tvMeshes->Sort(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tsMainOptionsShow(TObject *Sender)
{
	if( bMultiSelection ){
		edName->Text = "<Multiple selection (can't change)>";
		edClass->Text = "<Multiple selection (can't change)>";
		mmScript->Text = "<Multiple selection (can't change)>";
        lbLibRef->Caption = "<Multiple selection>";
	}else{
        lbLibRef->Caption = "";
        if (m_EditObject->IsLibItem()){
            lbLibRef->Caption = "Object is 'LIBRARY' item.";
        }else{
            if (m_EditObject->IsReference()){
                lbLibRef->Caption = "Object is 'LIBRARY' reference.";
            }else{
                lbLibRef->Caption = "Object is 'SCENE' item.";
            }
        }
    }
    edName->Enabled = !bMultiSelection;
    edClass->Enabled = !bMultiSelection;
    mmScript->Enabled = !bMultiSelection;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvMeshesDblClick(TObject *Sender)
{
    ebEditMesh->Click();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvMeshesItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::OnModified(TObject *Sender)
{
    ebOk->Enabled = true;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
int __fastcall TfrmPropertiesObject::ChangeProperties(ObjectList* pObjects)
{
    VERIFY(pObjects);
    tsMainOptions->TabVisible   = true;
    tsInfo->TabVisible          = true;
    tsTextures->TabVisible      = true;
    tsScript->TabVisible        = true;
    tsMeshes->TabVisible        = true;

    mode = pomChange;
    m_Objects = *pObjects;
	GetObjectsInfo();
    int mr = 0;
    while (mr!=mrOk && mr!=mrCancel) mr = frmPropertiesObject->ShowModal();
    return mr;
}

//---------------------------------------------------------------------------
int __fastcall TfrmPropertiesObject::AddNewObject(SObject2* obj)
{
    VERIFY(obj);
    tsMainOptions->TabVisible   = true;
    tsInfo->TabVisible          = true;
    tsTextures->TabVisible      = true;
    tsScript->TabVisible        = true;
    tsMeshes->TabVisible        = true;

    mode = pomAdd;
    m_Objects.clear();
    m_Objects.push_back(obj);
	GetObjectsInfo();
    edName->Text = "";
    edClass->Text = "Static";
    return frmPropertiesObject->ShowModal();
}
//---------------------------------------------------------------------------
int __fastcall TfrmPropertiesObject::EditObject(SObject2* obj)
{
    VERIFY(obj);
    tsMainOptions->TabVisible   = true;
    tsInfo->TabVisible          = true;
    tsTextures->TabVisible      = true;
    tsScript->TabVisible        = true;
    tsMeshes->TabVisible        = true;

    mode = pomEdit;
    m_Objects.clear();
    m_Objects.push_back(obj);
	GetObjectsInfo();
    return frmPropertiesObject->ShowModal();
}
//---------------------------------------------------------------------------

