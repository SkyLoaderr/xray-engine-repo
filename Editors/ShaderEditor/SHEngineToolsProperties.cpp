//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHEngineTools.h"
#include "Blender.h"
#include "UI_Tools.h"
#include "PropertiesList.h"
#include "xr_tokens.h"
#include "xr_trims.h"
#include "leftbar.h"
#include "folderlib.h"

#define MCSTRING_COUNT 	11
LPCSTR MCString[MCSTRING_COUNT]={"Custom...","-","$null","$base0","$base1","$base2","$base3","$base4","$base5","$base6","$base7"};
//---------------------------------------------------------------------------
xr_token							mode_token					[ ]={
	{ "Programmable",			  	CMatrix::modeProgrammable  	},
	{ "TCM",					  	CMatrix::modeTCM		  	},
	{ "Spherical Reflection",	  	CMatrix::modeS_refl	   		},
	{ "Cube Reflection",			CMatrix::modeC_refl	  		},
	{ 0,							0							}           
};
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::FillMatrix(PropValueVec& values, LPCSTR pref, CMatrix* m)
{
	TProperties* P = Tools.m_Props;

    if (m->dwMode==CMatrix::modeTCM){
	    FILL_PROP_EX(values, pref,	"Scale enabled",	&m->tcm,  	PROP::CreateFlagValue(CMatrix::tcmScale));
	    FILL_PROP_EX(values, pref, 	"Scale U",			&m->scaleU, PROP::CreateWaveValue());
	    FILL_PROP_EX(values, pref, 	"Scale V",			&m->scaleV, PROP::CreateWaveValue());

	    FILL_PROP_EX(values, pref, 	"Rotate enabled",	&m->tcm,  	PROP::CreateFlagValue(CMatrix::tcmRotate));
	    FILL_PROP_EX(values, pref, 	"Rotate",			&m->rotate, PROP::CreateWaveValue());

	    FILL_PROP_EX(values, pref, 	"Scroll enabled",	&m->tcm,  	PROP::CreateFlagValue(CMatrix::tcmScroll));
	    FILL_PROP_EX(values, pref, 	"Scroll U",			&m->scrollU,PROP::CreateWaveValue());
	    FILL_PROP_EX(values, pref, 	"Scroll V",			&m->scrollV,PROP::CreateWaveValue());
    }
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::RefreshProperties()
{
	PropValueVec values;
	if (m_CurrentBlender){
    }
    Tools.m_Props->AssignValues(values,true);
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::ModeOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	TElTreeItem* parent=item->Parent; R_ASSERT(parent);
    R_ASSERT(parent->Tag==PROP_LIST);
    ListValue* V = (ListValue*)parent->Data;
    string128 nm; strcpy(nm,V->GetValue());
    CMatrix* M = FindMatrix(nm,false); R_ASSERT(M);
    V->ApplyValue(nm);
    RefreshProperties();
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::AddMatrixProps(TElTreeItem* item, LPSTR name)
{
    CMatrix* M = FindMatrix(name,true);
    R_ASSERT(M);
//s	Tools.m_Props->BeginEditMode();
//s    TElTreeItem* node = Tools.m_Props->AddTokenItem(item,"Mode",&M->dwMode,mode_token,&ModeOnAfterEdit)->item;
//s    UpdateMatrixModeProps(node,M,M->dwMode);
//s	Tools.m_Props->EndEditMode(item);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::RemoveMatrixProps(TElTreeItem* parent){
    TProperties* P=Tools.m_Props;
	P->BeginEditMode();
	for (TElTreeItem* item=parent->GetLastChild(); item;){
    	TElTreeItem* node=parent->GetPrevChild(item);
        item->Delete();
        item=node;
	}
	P->EndEditMode(0);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MCOnDraw(PropValue* sender, LPVOID draw_val)
{
	AnsiString& V=*(AnsiString*)draw_val;
    VERIFY(V[1]);
	if (V[1]!='$') V="";
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MatrixOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendMatrix());
            AddMatrixProps(item,nm);
        }else{
            strcpy(nm,V->GetValue());
        }
    }else{
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveMatrix(nmm);
            V->ApplyValue(nmm);
            RemoveMatrixProps(item);
        }
    }
}
//------------------------------------------------------------------------------

void __fastcall CSHEngineTools::FillConst(PropValueVec& values, LPCSTR pref, CConstant* c)
{
}
void __fastcall CSHEngineTools::AddConstProps(TElTreeItem* item, LPSTR name)
{
/*
	Tools.m_Props->BeginEditMode();
	CConstant* C = Tools.SEngine.FindConstant(name,true);
    R_ASSERT(C);
    TProperties* P=Tools.m_Props;
    P->AddWaveItem(item,"R",&C->_R);
    P->AddWaveItem(item,"G",&C->_G);
    P->AddWaveItem(item,"B",&C->_B);
    P->AddWaveItem(item,"A",&C->_A);
	Tools.m_Props->EndEditMode(item);
*/
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::RemoveConstProps(TElTreeItem* parent){
    TProperties* P=Tools.m_Props;
	P->BeginEditMode();
	for (TElTreeItem* item=parent->GetLastChild(); item;){
    	TElTreeItem* node=parent->GetPrevChild(item);
        item->Delete();
        item=node;
	}
	P->EndEditMode(0);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::ConstOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendConstant());
            AddConstProps(item,nm);
        }else{
            strcpy(nm,V->GetValue());
        }
    }else{
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveConstant(nmm);
            V->ApplyValue(nmm);
            RemoveConstProps(item);
        }
    }
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnAfterEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	TextValue* V = (TextValue*)sender;
    AnsiString* new_name = (AnsiString*)edit_val;
	if (FOLDER::AfterTextEdit(fraLeftBar->tvEngine->Selected,V->GetValue(),*new_name))
    	RemoteRenameBlender(V->GetValue(),new_name->c_str());
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnBeforeEdit(TElTreeItem* item, PropValue* sender, LPVOID edit_val)
{
	FOLDER::BeforeTextEdit(((TextValue*)sender)->GetValue(),*(AnsiString*)edit_val);
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnDraw(PropValue* sender, LPVOID draw_val)
{
	FOLDER::TextDraw(((TextValue*)sender)->GetValue(),*(AnsiString*)draw_val);
}
//------------------------------------------------------------------------------

void CSHEngineTools::UpdateProperties()
{
	if (m_bFreezeUpdate) return;
	TProperties* P=Tools.m_Props;
    P->BeginFillMode();
	if (m_CurrentBlender){ // fill Tree
    	CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        CBlender_DESC* desc=(CBlender_DESC*)data.Pointer();
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];
        TElTreeItem* marker_node=0;
        TElTreeItem* node;

        P->AddMarkerItem(0,"Type",m_CurrentBlender->getComment());
        P->AddTextItem	(0,"Name",(LPSTR)&desc->cName,sizeof(desc->cName),NameOnAfterEdit,NameOnBeforeEdit,NameOnDraw);

        while (!data.Eof()){
            int sz=0;
            type = data.Rdword();     
            data.RstringZ(key);
            switch(type){
            case xrPID_MARKER:
            	marker_node = P->AddMarkerItem(0,key)->item;
            break;
            case xrPID_TOKEN:{
            	xrP_TOKEN* V=(xrP_TOKEN*)data.Pointer();
            	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*V->Count;
                P->AddTokenItem(marker_node,key,&V->IDselected,V->Count,(TokenValue3::Item*)(LPBYTE(data.Pointer()) + sizeof(xrP_TOKEN)));
            }break;
            case xrPID_MATRIX:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddListItemA(marker_node,key,(LPSTR)data.Pointer(),MCSTRING_COUNT,MCString,MatrixOnAfterEdit,0,MCOnDraw)->item;
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddMatrixProps(node,V);
            }break;
            case xrPID_CONSTANT:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddListItemA(marker_node,key,(LPSTR)data.Pointer(),MCSTRING_COUNT,MCString,ConstOnAfterEdit,0,MCOnDraw)->item;
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddConstProps(node,V);
            }break;
            case xrPID_TEXTURE:
            	sz=sizeof(string64);
                P->AddTexture2Item(marker_node,key,(LPSTR)data.Pointer(),sz);
            break;
            case xrPID_INTEGER:{
            	sz=sizeof(xrP_Integer);
                xrP_Integer* V=(xrP_Integer*)data.Pointer();
                P->AddIntItem	(marker_node,key,&V->value,V->min,V->max,1);
            }break;
            case xrPID_FLOAT:{
            	sz=sizeof(xrP_Float);
                xrP_Float* V=(xrP_Float*)data.Pointer();
                P->AddFloatItem	(marker_node,key,&V->value,V->min,V->max,0.01f,2);
            }break;
            case xrPID_BOOL:{
            	sz=sizeof(xrP_BOOL);
                xrP_BOOL* V=(xrP_BOOL*)data.Pointer();
                P->AddBOOLItem	(marker_node,key,&V->value);
            }break;
            default: THROW2("UNKNOWN xrPID_????");
            }
            data.Advance(sz);
        }
        ApplyChanges(true);
    }
    P->EndFillMode();
    P->SetModifiedEvent(Modified);
}
//------------------------------------------------------------------------------


