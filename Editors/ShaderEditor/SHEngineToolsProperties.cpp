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
void __fastcall CSHEngineTools::UpdateMatrixModeProps(TElTreeItem* item, CMatrix* m, DWORD mode)
{
	TProperties* P = Tools.m_Props;
	P->BeginEditMode();
    if (mode==CMatrix::modeTCM){
        P->AddItem(item,"Scale enabled",	&m->tcm,	PHelper.CreateFlag(CMatrix::tcmScale));
        P->AddItem(item,"Scale U",			&m->scaleU,	PHelper.CreateWave());
        P->AddItem(item,"Scale V",			&m->scaleV,	PHelper.CreateWave());
        P->AddItem(item,"Rotate enabled",	&m->tcm,	PHelper.CreateFlag(CMatrix::tcmRotate));
        P->AddItem(item,"Rotate",			&m->rotate,	PHelper.CreateWave());
        P->AddItem(item,"Scroll enabled",	&m->tcm,	PHelper.CreateFlag(CMatrix::tcmScroll));
        P->AddItem(item,"Scroll U",			&m->scrollU,PHelper.CreateWave());
        P->AddItem(item,"Scroll V",			&m->scrollV,PHelper.CreateWave());
    }else{
        for (TElTreeItem* itm=item->GetLastChild(); itm;){
            TElTreeItem* node=item->GetPrevChild(itm);
            itm->Delete();
            itm=node;
        }
    }
	P->EndEditMode();
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::ModeOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	TElTreeItem* parent=sender->item->Parent; R_ASSERT(parent);
    ListValue* V = (ListValue*)parent->Tag;
    R_ASSERT(V->type==PROP_LIST);
    string128 nm; strcpy(nm,V->GetValue());
    CMatrix* M = FindMatrix(nm,false); R_ASSERT(M);
    V->ApplyValue(nm);
    DWORD mode=*(DWORD*)edit_val;
    UpdateMatrixModeProps(sender->item,M,mode);
}
//---------------------------------------------------------------------------
void __fastcall CSHEngineTools::AddMatrixProps(TElTreeItem* item, LPSTR name)
{
    CMatrix* M = FindMatrix(name,true);
    R_ASSERT(M);
	Tools.m_Props->BeginEditMode();
    TElTreeItem* node = Tools.m_Props->AddItem(item,"Mode",&M->dwMode,PHelper.CreateToken(mode_token,sizeof(M->dwMode),&ModeOnAfterEdit));
    UpdateMatrixModeProps(node,M,M->dwMode);
	Tools.m_Props->EndEditMode(item);
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

void __fastcall CSHEngineTools::MatrixOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	ListValue* V = (ListValue*)sender;
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendMatrix());
            AddMatrixProps(sender->item,nm);
        }else{
            strcpy(nm,V->GetValue());
        }
    }else{                       
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveMatrix(nmm);
            V->ApplyValue(nmm);
            RemoveMatrixProps(sender->item);
        }
    }
}
//------------------------------------------------------------------------------

void __fastcall CSHEngineTools::AddConstProps(TElTreeItem* item, LPSTR name)
{
	Tools.m_Props->BeginEditMode();
	CConstant* C = Tools.SEngine.FindConstant(name,true);
    R_ASSERT(C);
    TProperties* P=Tools.m_Props;
    P->AddItem(item,"R",&C->_R,PHelper.CreateWave());
    P->AddItem(item,"G",&C->_G,PHelper.CreateWave());
    P->AddItem(item,"B",&C->_B,PHelper.CreateWave());
    P->AddItem(item,"A",&C->_A,PHelper.CreateWave());
	Tools.m_Props->EndEditMode(item);
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

void __fastcall CSHEngineTools::ConstOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	ListValue* V = dynamic_cast<ListValue*>(sender); R_ASSERT(V);
	LPSTR nm=(LPSTR)edit_val;	VERIFY(nm&&nm[0]);

	if (*nm!='$'){
        if (*V->GetValue()=='$'){
            strcpy(nm,AppendConstant());
            AddConstProps(sender->item,nm);
        }else{
            strcpy(nm,V->GetValue());
        }
    }else{
        if (*V->GetValue()!='$'){
        	string128 nmm; strcpy(nmm,V->GetValue());
            RemoveConstant(nmm);
            V->ApplyValue(nmm);
            RemoveConstProps(sender->item);
        }
    }
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnAfterEdit(PropValue* sender, LPVOID edit_val)
{
	TextValue* V = (TextValue*)sender;
    AnsiString* new_name = (AnsiString*)edit_val;
	if (FOLDER::AfterTextEdit(fraLeftBar->tvEngine->Selected,V->GetValue(),*new_name))
    	RemoteRenameBlender(V->GetValue(),new_name->c_str());
}
//------------------------------------------------------------------------------
void __fastcall CSHEngineTools::NameOnBeforeEdit(PropValue* sender, LPVOID edit_val)
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
    P->BeginFillMode("Engine shader");
	if (m_CurrentBlender){ // fill Tree
    	CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        CBlender_DESC* desc=(CBlender_DESC*)data.Pointer();
        data.Advance(sizeof(CBlender_DESC));
        DWORD type;
        char key[255];
        TElTreeItem* marker_node=0;
        TElTreeItem* node;

        P->AddItem	(0,"Type",(void*)m_CurrentBlender->getComment(),PHelper.CreateMarker());
        P->AddItem	(0,"Name",(LPSTR)&desc->cName,PHelper.CreateText(sizeof(desc->cName),NameOnAfterEdit,NameOnBeforeEdit,NameOnDraw));

        while (!data.Eof()){
            int sz=0;
            type = data.Rdword();     
            data.RstringZ(key);
            switch(type){
            case xrPID_MARKER:
            	marker_node = P->AddItem(0,key,0,PHelper.CreateMarker());
            break;
            case xrPID_TOKEN:{
            	xrP_TOKEN* V=(xrP_TOKEN*)data.Pointer();
            	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*V->Count;
                P->AddItem(marker_node,key,&V->IDselected,PHelper.CreateToken3(V->Count,(TokenValue3::Item*)(LPBYTE(data.Pointer()) + sizeof(xrP_TOKEN))));
            }break;
            case xrPID_MATRIX:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddItem(marker_node,key,(LPSTR)data.Pointer(),PHelper.CreateListA(MCSTRING_COUNT,MCString,MatrixOnAfterEdit,0,MCOnDraw));
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddMatrixProps(node,V);
            }break;
            case xrPID_CONSTANT:{
            	sz=sizeof(string64);
                TElTreeItem* node=P->AddItem(marker_node,key,(LPSTR)data.Pointer(),PHelper.CreateListA(MCSTRING_COUNT,MCString,ConstOnAfterEdit,0,MCOnDraw));
                LPSTR V=(LPSTR)data.Pointer();
				if (V&&V[0]&&(*V!='$')) AddConstProps(node,V);
            }break;
            case xrPID_TEXTURE:
            	sz=sizeof(string64);
                P->AddItem(marker_node,key,(LPSTR)data.Pointer(),PHelper.CreateTexture2(sz));
            break;
            case xrPID_INTEGER:{
            	sz=sizeof(xrP_Integer);
                xrP_Integer* V=(xrP_Integer*)data.Pointer();
                P->AddItem	(marker_node,key,&V->value,PHelper.CreateS32(V->min,V->max,1));
            }break;
            case xrPID_FLOAT:{
            	sz=sizeof(xrP_Float);
                xrP_Float* V=(xrP_Float*)data.Pointer();
                P->AddItem	(marker_node,key,&V->value,PHelper.CreateFloat(V->min,V->max,0.01f,2));
            }break;
            case xrPID_BOOL:{
            	sz=sizeof(xrP_BOOL);
                xrP_BOOL* V=(xrP_BOOL*)data.Pointer();
                P->AddItem	(marker_node,key,&V->value,PHelper.CreateBOOL());
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

 