#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Texture.h"
#include "xrShader.h"
#include "Shader.h"
#include "ui_main.h"
#include "scene.h"
#include "XRShaderDef.h"
#include "main.h"
#include "motion.h"
#include "EditLibrary.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma link "RxMenus"
#pragma link "multi_check"
#pragma link "multi_edit"
#pragma link "Placemnt"
#pragma resource "*.dfm"
          
TfrmPropertiesObject *frmPropertiesObject=0;
//---------------------------------------------------------------------------
int frmPropertiesObjectRun(ObjectList* pObjects, bool& bChange){
	frmPropertiesObject = new TfrmPropertiesObject(0);
    int res = frmPropertiesObject->Run(pObjects,bChange);
    _DELETE(frmPropertiesObject);
    return res;
}
//---------------------------------------------------------------------------
__fastcall TfrmPropertiesObject::TfrmPropertiesObject(TComponent* Owner)
    : TForm(Owner)
{
    sgTransform->Cells[0][0]="Position"; sgTransform->Cells[0][1]="Rotation"; sgTransform->Cells[0][2]="Scale";
    sgBB->Cells[0][0]="X"; sgBB->Cells[0][1]="Y"; sgBB->Cells[0][2]="Z";
    tx_selected = 0;
    surf_selected = 0;
    char buf[MAX_PATH] = {"ed.ini"};  FS.m_ExeRoot.Update(buf);
    fsStorage->IniFileName = buf;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormShow(TObject *Sender)
{
    pcObjects->ActivePage = tsMainOptions;
    ebOk->Enabled       = false;
    tvMeshes->Enabled   = true;
    edName->Hint 		= "Warning!!! Be careful before edit item library name.";
    tsMainOptionsShow   (Sender);
    tsMeshesShow        (Sender);
    ebDropper->Enabled	= false;
    ebDropper->Down		= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::FormClose(TObject *Sender,
      TCloseAction &Action)
{
	tvMeshes->Items->Clear();
    tvSurfaces->Items->Clear();
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::OnModified(TObject *Sender)
{
    ebOk->Enabled = true;
    UI->RedrawScene();
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebOkClick(TObject *Sender)
{
    if (!ApplyObjectsInfo()) return;
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
    if (!bMultiSelection){
        // save shader names for cancel...
        int k=0;
        for (SurfaceIt s_it=m_EditObject->FirstSurface(); s_it!=m_EditObject->LastSurface(); s_it++){
        	bool bCreateShader=false;
            if ((*s_it)->shader){
                if(strcmp(SH_Names[k].c_str(),(*s_it)->shader->shader->cName)!=0){
                    UI->Device.Shader.Delete((*s_it)->shader);
                    bCreateShader=true;
                }
            }else{
                (*s_it)->shader = UI->Device.Shader.Create(SHLib->FindShader(SH_Names[k])->cName,(*s_it)->textures);
                bCreateShader = true;
            }
            if (bCreateShader){
                SH_ShaderDef* sh_base=SHLib->FindShader(SH_Names[k]);
                VERIFY2(sh_base,"Can't find shader.");
                (*s_it)->shader = UI->Device.Shader.Create(sh_base->cName,(*s_it)->textures);
                VERIFY((*s_it)->shader);
                if (sh_base->Passes_Count>0)
                    (*s_it)->has_alpha = sh_base->Passes[0].Flags.bABlend;
            }
            k++;
        }
    }
}
//--------------------------------------------------------------------------------------------------

void TfrmPropertiesObject::GetObjectsInfo(){
	VERIFY(!m_Objects.empty());

	bMultiSelection = (m_Objects.size()>1)?true:false;
	SH_Names.clear();
    paSurface->Enabled 	= !bMultiSelection;
	ebSelectShader->Enabled=!bMultiSelection;
	if( !bMultiSelection ){
        m_EditObject = (CEditObject*)m_Objects.front(); VERIFY(m_EditObject);
        // save shader names for cancel...
        for (SurfaceIt s_it=m_EditObject->FirstSurface(); s_it!=m_EditObject->LastSurface(); s_it++)
        	SH_Names.push_back((*s_it)->shader->shader->cName);
        if (m_EditObject->IsReference()){
            ebSelectShader->Enabled = false;
        }
    }

	ObjectIt _F = m_Objects.begin();	VERIFY( (*_F)->ClassID()==OBJCLASS_EDITOBJECT );
	CEditObject *_O = (CEditObject *)(*_F);

	mmScript->Text = _O->GetClassScript();
	edName->Text   = _O->GetName();
	cbMakeDynamic->ObjFirstInit( TCheckBoxState(_O->IsDynamic()) );

	_F++;
	for(;_F!=m_Objects.end();_F++){
		VERIFY( (*_F)->ClassID()==OBJCLASS_EDITOBJECT );
    	_O = (CEditObject *)(*_F);
		cbMakeDynamic->ObjNextInit( TCheckBoxState(_O->IsDynamic()) );
	}
}

bool TfrmPropertiesObject::ApplyObjectsInfo(){
    if (!bMultiSelection){
        if (!edName->Text.Length()){
            MessageDlg("Enter Object Name!", mtError, TMsgDlgButtons() << mbOK, 0);
            return false;
        }
        if (!tvMeshes->Items->Count){
            MessageDlg("Add mesh!", mtError, TMsgDlgButtons() << mbOK, 0);
            return false;
        }
        VERIFY( m_EditObject->ClassID()==OBJCLASS_EDITOBJECT );
        cbMakeDynamic->ObjApply( m_EditObject->m_DynamicObject );
        m_EditObject->GetClassScript() = mmScript->Text;
        strcpy( m_EditObject->GetName(), edName->Text.c_str() );
    }else{
        VERIFY( !m_Objects.empty() );
        CEditObject *_O = 0;
        ObjectIt _F = m_Objects.begin();
        for(;_F!=m_Objects.end();_F++){
            VERIFY( (*_F)->ClassID()==OBJCLASS_EDITOBJECT );
            _O = (CEditObject *)(*_F);
            cbMakeDynamic->ObjApply( _O->m_DynamicObject );
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

void __fastcall TfrmPropertiesObject::cbMakeDynamicClick(TObject *Sender)
{
    tsScript->TabVisible 		= cbMakeDynamic->Checked&&!bMultiSelection;
    tsOAnimation->TabVisible 	= cbMakeDynamic->Checked&&!bMultiSelection;
    tsSAnimation->TabVisible 	= cbMakeDynamic->Checked&&!bMultiSelection&&m_EditObject->IsSkeleton();
    OnModified(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tsMainOptionsShow(TObject *Sender)
{
	if( bMultiSelection ){
		edName->Text = "<Multiple selection (can't change)>";
		mmScript->Text = "<Multiple selection (can't change)>";
        lbLibRef->Caption = "<Multiple selection>";
	}else{
        lbLibRef->Caption = "";
        if (m_EditObject->IsLibItem()){
            lbLibRef->Caption = "Object is 'LIBRARY' item.";
        }else{
            if (m_EditObject->IsReference()){
                AnsiString t; t.sprintf("Object is 'LIBRARY' reference: %s",m_EditObject->GetRefName());
                lbLibRef->Caption = t;
            }else{
                lbLibRef->Caption = "Object is 'SCENE' item.";
            }
        }
    }
    edName->Enabled 		= !bMultiSelection;
    mmScript->Enabled 		= !bMultiSelection;
    gbTemplates->Enabled 	= !bMultiSelection;
    tsScript->TabVisible 	= cbMakeDynamic->Checked&&!bMultiSelection;
    tsOAnimation->TabVisible= cbMakeDynamic->Checked&&!bMultiSelection;
    tsSAnimation->TabVisible= cbMakeDynamic->Checked&&!bMultiSelection&&m_EditObject->IsSkeleton();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
int __fastcall TfrmPropertiesObject::Run(ObjectList* pObjects, bool& bChange)
{
    VERIFY(pObjects);
    tsMainOptions->TabVisible   = true;
    tsInfo->TabVisible          = true;
    tsSurfaces->TabVisible      = true;
    tsScript->TabVisible        = true;
    tsMeshes->TabVisible        = true;
    tsOAnimation->TabVisible 	= true;
    tsSAnimation->TabVisible 	= true;

    m_Objects 		= *pObjects;
    m_EditObject 	= 0;
	GetObjectsInfo();
    int mr = 0;
    while (mr!=mrOk && mr!=mrCancel) mr = frmPropertiesObject->ShowModal();
    bChange = ebOk->Enabled;
    return mr;
}
//---------------------------------------------------------------------------


void __fastcall TfrmPropertiesObject::pcObjectsChange(TObject *Sender)
{
    ebDropper->Down		= false;
	if (((pcObjects->ActivePage==tsMeshes)||(pcObjects->ActivePage==tsSurfaces))&&m_EditObject)
	    ebDropper->Enabled	= true;
    else
	    ebDropper->Enabled	= false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::OnIdle(){
	if (ebDropper->Down&&m_EditObject){
		if ((pcObjects->ActivePage==tsMeshes)||(pcObjects->ActivePage==tsSurfaces)){	
        // check cursor position and define ...
            POINT pt,wpt;
            GetCursorPos( &pt );

            SHORT ks=GetAsyncKeyState(VK_LBUTTON);

			TWinControl* ctr = FindVCLWindow(pt);
            if (ctr!=frmMain) return;

            if (ks&0x0001){
                Fvector S, D;
                Fvector2 CP;
                pt = UI->GetD3DWindow()->ScreenToClient(pt);
                CP.set(float(pt.x),float(pt.y));
                UI->Device.m_Camera.MouseRayFromPoint(S, D, CP );

                float dist=flt_max;
                SRayPickInfo pinf;
                if (m_EditObject->RayPick(dist,S,D,precalc_identity,&pinf)){
					if (pcObjects->ActivePage==tsMeshes){
 	                   	tvMeshes->Selected = tvMeshes->Items->LookForItem(0,pinf.mesh->GetName(),0,0,false,true,false,false,true);
                        tvMeshes->EnsureVisible(tvSurfaces->Selected);
	                }else if (pcObjects->ActivePage==tsSurfaces){
						UI->RedrawScene();
						st_Surface* surf=pinf.mesh->GetSurfaceByFaceID(pinf.rp_inf.id);
 	                   	tvSurfaces->Selected = tvSurfaces->Items->LookForItem(0,surf->name,0,0,false,true,false,false,true);
                        tvSurfaces->EnsureVisible(tvSurfaces->Selected);
    	            }
                }
            }
        }
    }
}











