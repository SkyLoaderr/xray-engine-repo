#include "stdafx.h"
#pragma hdrstop

#include "OneStage.h"
#include "Shader.h"
#include "ShaderFunction.h"
#include "ShaderModificator.h"
#include "PropertiesShader.h"
#include "FileSystem.h"
#include "xr_tokens.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "CSPIN"
#pragma link "RXCtrls"
#pragma link "RXSpin"
#pragma link "CloseBtn"
#pragma link "ElHeader"
#pragma link "ElTree"
#pragma link "RxMenus"
#pragma resource "*.dfm"
TfrmOneStage *frmOneStage;
//---------------------------------------------------------------------------
xr_token							argument_token			[ ]={
	{ "Current",			   		D3DTA_CURRENT			},
	{ "Diffuse",			   		D3DTA_DIFFUSE			},
	{ "Specular",			   		D3DTA_SPECULAR		  	},
	{ "Texture",			   		D3DTA_TEXTURE		  	},
	{ "Factor",						D3DTA_TFACTOR		  	},
	{ 0,							0						}
};
xr_token							operation_token					[ ]={
	{ "Disable",				  	D3DTOP_DISABLE				  	},
	{ "SelectArg1",				  	D3DTOP_SELECTARG1			  	},
	{ "SelectArg2",				  	D3DTOP_SELECTARG2			  	},
	{ "Modulate",				  	D3DTOP_MODULATE			  		},
	{ "Modulate2x",				  	D3DTOP_MODULATE2X			  	},
	{ "Modulate4x",				  	D3DTOP_MODULATE4X			  	},
	{ "Add",				  		D3DTOP_ADD			  			},
	{ "AddSigned",				  	D3DTOP_ADDSIGNED			  	},
	{ "AddSigned2x",				D3DTOP_ADDSIGNED2X			  	},
	{ "Subtract",				  	D3DTOP_SUBTRACT			  		},
	{ "AddSmoth",				  	D3DTOP_ADDSMOOTH			  	},
	{ "BlendDiffuseAlpha",			D3DTOP_BLENDDIFFUSEALPHA		},
	{ "BlendTextureAlpha",			D3DTOP_BLENDTEXTUREALPHA		},
	{ "BlendFactorAlpha",			D3DTOP_BLENDFACTORALPHA			},
	{ "BlendTextureAlphaPM",		D3DTOP_BLENDTEXTUREALPHAPM		},
	{ "BlendCurrentAlpha",			D3DTOP_BLENDCURRENTALPHA		},
	{ "Premodulate",				D3DTOP_PREMODULATE			  	},
	{ "ModulateAlpha_AddColor",		D3DTOP_MODULATEALPHA_ADDCOLOR	},
	{ "ModulateColor_AddAlpha",		D3DTOP_MODULATECOLOR_ADDALPHA	},
	{ "ModulateInvAlpha_AddColor",	D3DTOP_MODULATEINVALPHA_ADDCOLOR},
	{ "ModulateInvColor_AddAlpha",	D3DTOP_MODULATEINVCOLOR_ADDALPHA},
	{ 0,							0								}
};
xr_token							blend_token					[ ]={
	{ "Zero",					  	D3DBLEND_ZERO			  	},
	{ "One",					  	D3DBLEND_ONE			  	},
	{ "SrcColor",				  	D3DBLEND_SRCCOLOR		  	},
	{ "InvSrcColor",			  	D3DBLEND_INVSRCCOLOR		},
	{ "SrcAlpha",				  	D3DBLEND_SRCALPHA			},
	{ "InvSrcAlpha",		  		D3DBLEND_INVSRCALPHA		},
	{ "DestColor",				  	D3DBLEND_DESTCOLOR			},
	{ "InvDestColor",				D3DBLEND_INVDESTCOLOR		},
	{ "SrcAlphaSat",			  	D3DBLEND_SRCALPHASAT		},
	{ 0,							0							}
};
//---------------------------------------------------------------------------
AnsiString& GetTokenNameFromVal_AND(DWORD val, AnsiString& res, const xr_token *token_list){
	bool bRes=false;
	for( int i=0; token_list[i].name; i++ )
    	if (token_list[i].id&val) {res = token_list[i].name; bRes=true; break; }
	if (!bRes) res="";
    return res;
}

AnsiString& GetTokenNameFromVal_EQ(DWORD val, AnsiString& res, const xr_token *token_list){
	bool bRes=false;
	for( DWORD i=0; token_list[i].name; i++ )
    	if (token_list[i].id==int(val)) {res = token_list[i].name; bRes=true; break; }
	if (!bRes) res="";
    return res;
}
AnsiString& GetTokenNameFromVal_ARG (DWORD val, AnsiString& res, const xr_token *token_list){
	bool bRes=false;
    val &= ~(D3DTA_COMPLEMENT|D3DTA_ALPHAREPLICATE);
	for( DWORD i=0; token_list[i].name; i++ )
    	if (token_list[i].id==int(val)) {res = token_list[i].name; bRes=true; break; }
	if (!bRes) res="";
    return res;
}

DWORD GetTokenValFromName(AnsiString& val, const xr_token *token_list){
	for( int i=0; token_list[i].name; i++ )
		if( !stricmp(val.c_str(),token_list[i].name) )
			return token_list[i].id;
    return 0;
}
//---------------------------------------------------------------------------


void TfrmOneStage::CheckTCM(){
    if((m_CurStage->tcm&SH_StageDef::tcmScale)||(m_CurStage->tcm&SH_StageDef::tcmRotate)||
       (m_CurStage->tcm&SH_StageDef::tcmScroll)||(m_CurStage->tcm&SH_StageDef::tcmStretch)){
		ebTCM->Font->Style = TFontStyles()<< fsBold;
    }else{
		ebTCM->Font->Style = TFontStyles();
    }
}

void TfrmOneStage::FillMenuFromToken(TRxPopupMenu* menu, const xr_token *token_list ){
	menu->Items->Clear();
	for( int i=0; token_list[i].name; i++ ){
    	TMenuItem* mi = new TMenuItem(0);
      	mi->Caption = token_list[i].name;
        mi->OnClick = pmSetTextClick;
      	menu->Items->Add(mi);
    }
}
//---------------------------------------------------------------------------
__fastcall TfrmOneStage::TfrmOneStage(TComponent* Owner)
    : TForm(Owner)
{
	m_CurStage = 0;
	FillMenuFromToken(pmArgument,argument_token);
	FillMenuFromToken(pmOperation,operation_token);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::ShowStage(SH_StageDef* stage){
	VERIFY(stage);
	m_CurStage = stage;

    bLoadMode = true;
    if (m_CurStage){
        stTname->Caption		= m_CurStage->Tname;
        stTCSrcGeom->Caption	= m_CurStage->Gname;
        rbTCSource->ItemIndex	= m_CurStage->tcs;
        AnsiString temp;
        temp.sprintf			("%1.3f,%1.3f,%1.3f",m_CurStage->tcs_vec_dir.x, m_CurStage->tcs_vec_dir.y, m_CurStage->tcs_vec_dir.z);
        edVector->Text			= temp;
        rbTCAddressing->ItemIndex= m_CurStage->tca;
        // color
        cbCComplArg1->Checked	= m_CurStage->ca1&D3DTA_COMPLEMENT;
        cbCAReplArg1->Checked	= m_CurStage->ca1&D3DTA_ALPHAREPLICATE;
        cbCComplArg2->Checked	= m_CurStage->ca2&D3DTA_COMPLEMENT;
        cbCAReplArg2->Checked	= m_CurStage->ca2&D3DTA_ALPHAREPLICATE;
        stCArg1->Caption		= GetTokenNameFromVal_ARG	(m_CurStage->ca1, stCArg1->Caption, argument_token);
        stCArg2->Caption		= GetTokenNameFromVal_ARG	(m_CurStage->ca2, stCArg2->Caption, argument_token);
        stCOp->Caption			= GetTokenNameFromVal_EQ	(m_CurStage->cop, stCOp->Caption, 	operation_token);
        // alpha
        cbAComplArg1->Checked	= m_CurStage->ca1&D3DTA_COMPLEMENT;
        cbAComplArg2->Checked	= m_CurStage->ca2&D3DTA_COMPLEMENT;
        stAArg1->Caption		= GetTokenNameFromVal_ARG	(m_CurStage->aa1, stAArg1->Caption, argument_token);
        stAArg2->Caption		= GetTokenNameFromVal_ARG	(m_CurStage->aa2, stAArg2->Caption, argument_token);
        stAOp->Caption			= GetTokenNameFromVal_EQ	(m_CurStage->aop, stAOp->Caption, 	operation_token);
    }
    bLoadMode = false;
    CheckTCM();
    Visible=true;
}
//---------------------------------------------------------------------------
void __fastcall TfrmOneStage::HideStage(){
    Visible=false;
}

static TStaticText* temp_stext=0;
void __fastcall TfrmOneStage::ArgumentMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    temp_stext = (TStaticText*)Sender;
	pmArgument->TrackButton = tbLeftButton;
	pmArgument->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::OperationMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    temp_stext = (TStaticText*)Sender;
	pmOperation->TrackButton = tbLeftButton;
	pmOperation->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::stTCSrcGeomMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    temp_stext = (TStaticText*)Sender;
	pmTCSourceGeom->TrackButton = tbLeftButton;
	pmTCSourceGeom->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::stTnameMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
	temp_stext = (TStaticText*)Sender;
	pmTname->TrackButton = tbLeftButton;
	pmTname->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::pmSetTextClick(TObject *Sender)
{
	if (temp_stext){
    	temp_stext->Caption=((TMenuItem*)Sender)->Caption;
	    temp_stext = 0;
        UpdateStageData();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::PictureClick(TObject *Sender)
{
	if (temp_stext){
        char tname[MAX_PATH]="";
        if (FS.GetOpenName(&FS.m_Textures,tname)){
            char name[1024];
            _splitpath( tname, 0, 0, name, 0 );
	    	temp_stext->Caption=name;
        }
	    temp_stext = 0;
        UpdateStageData();
    }
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::UpdateClick(TObject *Sender)
{
	UpdateStageData();
}
//---------------------------------------------------------------------------

void TfrmOneStage::UpdateStageData(){
	if (bLoadMode) return;
    m_CurStage->SetMapName		(stTname->Caption.c_str());
    m_CurStage->SetChannelName	(stTCSrcGeom->Caption.c_str());
    m_CurStage->tcs	= SH_StageDef::ETCSource(rbTCSource->ItemIndex);
    sscanf(edVector->Text.c_str(),"%f,%f,%f",&m_CurStage->tcs_vec_dir.x, &m_CurStage->tcs_vec_dir.y, &m_CurStage->tcs_vec_dir.z);
    m_CurStage->tcs_vec_dir.normalize_safe();
    m_CurStage->tca	= SH_StageDef::ETCAddress(rbTCAddressing->ItemIndex);
    // color
    m_CurStage->ca1	= GetTokenValFromName(stCArg1->Caption,argument_token);
    m_CurStage->ca2	= GetTokenValFromName(stCArg2->Caption,argument_token);
    m_CurStage->cop	= GetTokenValFromName(stCOp->Caption,operation_token);
    m_CurStage->ca1	|= (cbCComplArg1->Checked)?D3DTA_COMPLEMENT:0;
    m_CurStage->ca1	|= (cbCAReplArg1->Checked)?D3DTA_ALPHAREPLICATE:0;
    m_CurStage->ca2	|= (cbCComplArg2->Checked)?D3DTA_COMPLEMENT:0;
    m_CurStage->ca2	|= (cbCAReplArg2->Checked)?D3DTA_ALPHAREPLICATE:0;
    // alpha
    m_CurStage->aa1	= GetTokenValFromName(stAArg1->Caption,argument_token);
    m_CurStage->aa2	= GetTokenValFromName(stAArg2->Caption,argument_token);
    m_CurStage->aop	= GetTokenValFromName(stAOp->Caption,operation_token);
    m_CurStage->aa1	|= (cbAComplArg1->Checked)?D3DTA_COMPLEMENT:0;
    m_CurStage->aa2	|= (cbAComplArg2->Checked)?D3DTA_COMPLEMENT:0;
	if (frmPropertiesShader) frmPropertiesShader->OnModified();
}

void __fastcall TfrmOneStage::rbTCSourceClick(TObject *Sender)
{
	stTCSrcGeom->Enabled 	= (rbTCSource->ItemIndex==0);
    edVector->Enabled 		= (rbTCSource->ItemIndex==3);
    UpdateClick(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TfrmOneStage::ebTCMClick(TObject *Sender)
{
	if (frmShaderModificatorRun(m_CurStage)==mrOk)
		if (frmPropertiesShader) frmPropertiesShader->OnModified();
    CheckTCM();
}
//---------------------------------------------------------------------------

