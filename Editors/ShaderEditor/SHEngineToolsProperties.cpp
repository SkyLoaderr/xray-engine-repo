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
void __fastcall CSHEngineTools::ModeOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
/*
	TElTreeItem* parent=sender->Owner()->Item()->Parent; R_ASSERT(parent);
    ListValue* V = (ListValue*)parent->Tag;
    R_ASSERT(V->Owner()->Type()==PROP_LIST);
    string128 nm; strcpy(nm,V->GetValue());
    CMatrix* M = FindMatrix(nm,false); R_ASSERT(M);
    V->ApplyValue(nm);
//    DWORD mode=*(DWORD*)edit_val;
//    UpdateMatrixModeProps(sender->item,M,mode);
*/
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::FillMatrixProps(PropItemVec& items, LPCSTR pref, LPSTR name)
{
    CMatrix* M = FindMatrix(name,true);
    R_ASSERT(M);

	PropValue* P=0;
    PHelper.CreateToken(items,PHelper.PrepareKey(pref,"Mode"),&M->dwMode,mode_token,sizeof(M->dwMode));
//    P->SetEvents(ModeOnAfterEdit);

    if (M->dwMode==CMatrix::modeTCM){
	    PHelper.CreateFlag32(items,	PHelper.PrepareKey(pref,"Scale enabled"),	&M->tcm_flags,CMatrix::tcmScale);
		PHelper.CreateWave	(items,	PHelper.PrepareKey(pref,"Scale U"),			&M->scaleU);
		PHelper.CreateWave	(items,	PHelper.PrepareKey(pref,"Scale V"),			&M->scaleV);
	    PHelper.CreateFlag32(items,	PHelper.PrepareKey(pref,"Rotate enabled"),	&M->tcm_flags,CMatrix::tcmRotate);
		PHelper.CreateWave	(items,	PHelper.PrepareKey(pref,"Rotate"),			&M->rotate);
	    PHelper.CreateFlag32(items,	PHelper.PrepareKey(pref,"Scroll enabled"),	&M->tcm_flags,CMatrix::tcmScroll);
		PHelper.CreateWave	(items,	PHelper.PrepareKey(pref,"Scroll U"),		&M->scrollU);
		PHelper.CreateWave	(items,	PHelper.PrepareKey(pref,"Scroll V"),		&M->scrollV);
    }
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MCOnDraw(PropValue* sender, LPVOID draw_val)
{
	AnsiString& V=*(AnsiString*)draw_val;
    VERIFY(V[1]);
	if (V[1]!='$') V="Custom";
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::MatrixOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendMatrix());
//S            AddMatrixProps(sender->item,nm);
        }else{
            strcpy(nm,V->GetValue());
        }
    }else{                       
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveMatrix(nmm);
            V->ApplyValue(nmm);
//S            RemoveMatrixProps(sender->item);
        }
    }
}
//------------------------------------------------------------------------------

void __fastcall CSHEngineTools::FillConstProps(PropItemVec& items, LPCSTR pref, LPSTR name)
{
	CConstant* C = Tools.SEngine.FindConstant(name,true);
    R_ASSERT(C);
    PHelper.CreateWave(items,PHelper.PrepareKey(pref,"R"),&C->_R);
    PHelper.CreateWave(items,PHelper.PrepareKey(pref,"G"),&C->_G);
    PHelper.CreateWave(items,PHelper.PrepareKey(pref,"B"),&C->_B);
    PHelper.CreateWave(items,PHelper.PrepareKey(pref,"A"),&C->_A);
}
//---------------------------------------------------------------------------

void __fastcall CSHEngineTools::ConstOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	ListValue* V = dynamic_cast<ListValue*>(sender); R_ASSERT(V);
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendConstant());
//S            AddConstProps(sender->item,nm);
        }else{
            strcpy(nm,V->GetValue());
        }
    }else{
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveConstant(nmm);
            V->ApplyValue(nmm);
//S            RemoveConstProps(sender->item);
        }
    }
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	TextValue* V = (TextValue*)sender;
    AnsiString* new_name = (AnsiString*)edit_val;
	if (FHelper.NameAfterEdit(fraLeftBar->tvEngine->Selected,V->GetValue(),*new_name))
    	RemoteRenameBlender(V->GetValue(),new_name->c_str());
}
//------------------------------------------------------------------------------

void CSHEngineTools::UpdateProperties()
{
	if (m_bFreezeUpdate) return;

	PropItemVec items;
	if (m_CurrentBlender){ // fill Tree
    	AnsiString marker_text="";
    
    	CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        CBlender_DESC* desc=(CBlender_DESC*)data.Pointer();
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];

        PHelper.CreateCaption(items,"Type",m_CurrentBlender->getComment());
        PropValue* V	= PHelper.CreateText(items,"Name",desc->cName,sizeof(desc->cName));
        V->SetEvents	(NameOnAfterEdit,FHelper.NameBeforeEdit); V->Owner()->SetEvents(FHelper.NameDraw);

        while (!data.Eof()){
            int sz=0;
            type = data.Rdword();     
            data.RstringZ(key);
            switch(type){
            case xrPID_MARKER:
	            marker_text = key;
            break;
            case xrPID_TOKEN:{
            	xrP_TOKEN* V=(xrP_TOKEN*)data.Pointer();
            	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*V->Count;
                PHelper.CreateToken3(items,PHelper.PrepareKey(marker_text.c_str(),key),&V->IDselected,V->Count,(TokenValue3::Item*)(LPBYTE(data.Pointer()) + sizeof(xrP_TOKEN)));
            }break;
            case xrPID_MATRIX:{
            	sz				= sizeof(string64);
                LPSTR V			= (LPSTR)data.Pointer();
                PropValue* P	= PHelper.CreateListA(items,PHelper.PrepareKey(marker_text.c_str(),key),V,MCSTRING_COUNT,MCString);
                AnsiString pref = AnsiString(PHelper.PrepareKey(marker_text.c_str(),"Custom "))+key;
				if (V&&V[0]&&(*V!='$')) FillMatrixProps(items,pref.c_str(),V);
                P->SetEvents	(MatrixOnAfterEdit);
                P->Owner()->SetEvents(MCOnDraw);
            }break;
            case xrPID_CONSTANT:{
            	sz=sizeof(string64);
            	sz				= sizeof(string64);
                LPSTR V			= (LPSTR)data.Pointer();
                PropValue* P	= PHelper.CreateListA(items,PHelper.PrepareKey(marker_text.c_str(),key),V,MCSTRING_COUNT,MCString);
                AnsiString pref = AnsiString(PHelper.PrepareKey(marker_text.c_str(),"Custom "))+key;
				if (V&&V[0]&&(*V!='$')) FillConstProps(items,pref.c_str(),V);
                P->SetEvents	(ConstOnAfterEdit);
                P->Owner()->SetEvents(MCOnDraw);
            }break;
            case xrPID_TEXTURE:
            	sz=sizeof(string64);
                PHelper.CreateTexture2(items,PHelper.PrepareKey(marker_text.c_str(),key),(LPSTR)data.Pointer(),sz);
            break;
            case xrPID_INTEGER:{
            	sz=sizeof(xrP_Integer);
                xrP_Integer* V=(xrP_Integer*)data.Pointer();
                PHelper.CreateS32(items,PHelper.PrepareKey(marker_text.c_str(),key),&V->value,V->min,V->max,1);
            }break;
            case xrPID_FLOAT:{
            	sz=sizeof(xrP_Float);
                xrP_Float* V=(xrP_Float*)data.Pointer();
                PHelper.CreateFloat(items,PHelper.PrepareKey(marker_text.c_str(),key),&V->value,V->min,V->max,0.01f,2);
            }break;
            case xrPID_BOOL:{
            	sz=sizeof(xrP_BOOL);
                xrP_BOOL* V=(xrP_BOOL*)data.Pointer();
                PHelper.CreateBOOL(items,PHelper.PrepareKey(marker_text.c_str(),key),&V->value);
            }break;
            default: THROW2("UNKNOWN xrPID_????");
            }
            data.Advance(sz);
        }
        ApplyChanges(true);
    }
    Tools.m_Props->AssignItems(items,true);
    Tools.m_Props->SetModifiedEvent(Modified);
}
//------------------------------------------------------------------------------

 