#include "stdafx.h"
#pragma hdrstop

#include "ShaderFunction.h"
#include "Shader.h"
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
#pragma link "multi_edit"
#pragma resource "*.dfm"
TfrmShaderFunction *frmShaderFunction=0;
//---------------------------------------------------------------------------
int frmShaderFunctionRun(SH_Function* func){
	frmShaderFunction = new TfrmShaderFunction(0);
    int res = frmShaderFunction->Run(func);
    _DELETE(frmShaderFunction);
    return res;
}
//---------------------------------------------------------------------------
xr_token							function_token					[ ]={
	{ "Sin",					  	SH_Function::fSIN		  	},
	{ "Triangle",				  	SH_Function::fTRIANGLE	   	},
	{ "Square",					  	SH_Function::fSQUARE	  	},
	{ "Saw-Tooth",				  	SH_Function::fSAWTOOTH		},
	{ "Inv Saw-Tooth",			  	SH_Function::fINVSAWTOOTH	},
	{ 0,							0							}
};
//---------------------------------------------------------------------------
void TfrmShaderFunction::FillMenuFromToken(TRxPopupMenu* menu, const xr_token *token_list ){
	menu->Items->Clear();
	for( int i=0; token_list[i].name; i++ ){
    	TMenuItem* mi = new TMenuItem(0);
      	mi->Caption = token_list[i].name;
        mi->OnClick = stFunctionClick;
      	menu->Items->Add(mi);
    }
}
//---------------------------------------------------------------------------
AnsiString& TfrmShaderFunction::GetTokenNameFromVal_EQ(DWORD val, AnsiString& res, const xr_token *token_list){
	bool bRes=false;
	for( DWORD i=0; token_list[i].name; i++ )
    	if (token_list[i].id==int(val)) {res = token_list[i].name; bRes=true; break; }
	if (!bRes) res="";
    return res;
}

DWORD TfrmShaderFunction::GetTokenValFromName(AnsiString& val, const xr_token *token_list){
	for( int i=0; token_list[i].name; i++ )
		if( !stricmp(val.c_str(),token_list[i].name) )
			return token_list[i].id;
    return 0;
}

//---------------------------------------------------------------------------
__fastcall TfrmShaderFunction::TfrmShaderFunction(TComponent* Owner)
    : TForm(Owner)
{
	FillMenuFromToken(pmFunction, function_token);
}
//---------------------------------------------------------------------------


void __fastcall TfrmShaderFunction::DrawGraph()
{
    int w = imDraw->Width-4;
    int h = imDraw->Height-4;

    TRect r; r.Left=0; r.Top=0;  r.Right=w+4; r.Bottom=h+4;
    TCanvas* C=imDraw->Canvas;
    C->Brush->Color = clBlack;
    C->FillRect(r);
    C->Pen->Color = TColor(0x00006600);
    // draw center
    C->Pen->Color = clLime;
    C->Pen->Style = psDot;
    C->MoveTo(2,h/2+2);
    C->LineTo(w+2,h/2+2);
    // draw rect
    C->Pen->Color = TColor(0x00006600);
    C->Pen->Style = psSolid;
    C->MoveTo(0,0);
    C->LineTo(w+3,0);    C->LineTo(w+3,h+3);
    C->LineTo(0,h+3);    C->LineTo(0,0);
    // draw graph
    C->Pen->Color = clYellow;

    float t_cost = 1.f/w;
    float tm = 0;
    float y = m_CurFunc->Calculate(tm) - m_CurFunc->arg[0];
    float yy = h-((h/(m_CurFunc->arg[1]*2))*y + h/2);
    C->MoveTo(2,yy+2);
    for (int t=1; t<w; t++){
    	tm = t*t_cost;
    	y = m_CurFunc->Calculate(tm) - m_CurFunc->arg[0];
        yy = h-((h/(m_CurFunc->arg[1]*2))*y + h/2);
        C->LineTo(t+2,yy+2);
    }
    // draw X-axis
    C->Pen->Color = clGreen;
	float AxisX = h-((h/(m_CurFunc->arg[1]*2))*(-m_CurFunc->arg[0]) + h/2);
    C->MoveTo(2,AxisX+2);
    C->LineTo(w+2,AxisX+2);
}
//---------------------------------------------------------------------------
void __fastcall TfrmShaderFunction::FormKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
    if (Key==VK_ESCAPE) ebCancel->Click();
}

//----------------------------------------------------


void __fastcall TfrmShaderFunction::ebOkClick(TObject *Sender)
{
	UpdateFuncData();
    Close();
    ModalResult = mrOk;
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderFunction::ebCancelClick(TObject *Sender)
{
    Close();
    ModalResult = mrCancel;
}
//---------------------------------------------------------------------------


//----------------------------------------------------
int __fastcall TfrmShaderFunction::Run(SH_Function* func)
{
	VERIFY(func);
	m_CurFunc 	= func;
    m_SaveFunc	= new SH_Function(*func);
    GetFuncData	();
    UpdateFuncData();
    int res 	= ShowModal();
    if (res!=mrOk)CopyMemory(m_CurFunc,m_SaveFunc,sizeof(SH_Function));
    _DELETE(m_SaveFunc);
    return res;
}

static TStaticText* temp_text=0;
void __fastcall TfrmShaderFunction::stFunctionMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    POINT pt;
    GetCursorPos(&pt);
    temp_text = (TStaticText*)Sender;
    pmFunction->TrackButton = tbLeftButton;
	pmFunction->Popup(pt.x,pt.y);
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderFunction::stFunctionClick(TObject *Sender)
{
	if (temp_text) temp_text->Caption=((TMenuItem*)Sender)->Caption;
    temp_text = 0;
    UpdateFuncData();
}
//---------------------------------------------------------------------------

void TfrmShaderFunction::GetFuncData(){
	bLoadMode = true;
	stFunction->Caption = GetTokenNameFromVal_EQ(m_CurFunc->F, stFunction->Caption, function_token);
    seArg1->Value = m_CurFunc->arg[0];
    seArg2->Value = m_CurFunc->arg[1];
    seArg3->Value = m_CurFunc->arg[2];
    seArg4->Value = m_CurFunc->arg[3];
	bLoadMode = false;
}

void TfrmShaderFunction::UpdateFuncData(){
	if (bLoadMode) return;
	m_CurFunc->F = GetTokenValFromName(stFunction->Caption, function_token);
    m_CurFunc->arg[0] = seArg1->Value;
    m_CurFunc->arg[1] = seArg2->Value;
    m_CurFunc->arg[2] = seArg3->Value;
    m_CurFunc->arg[3] = seArg4->Value;
    AnsiString t;
    t.sprintf("%.2f",m_CurFunc->arg[1]+m_CurFunc->arg[0]);  lbMax->Caption = t;
    t.sprintf("%.2f",-m_CurFunc->arg[1]+m_CurFunc->arg[0]);	lbMin->Caption = t;
    t.sprintf("%.2f",m_CurFunc->arg[0]);    				lbCenter->Caption = t;
	DrawGraph();
}
void __fastcall TfrmShaderFunction::seArgExit(TObject *Sender)
{
	UpdateFuncData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderFunction::seArgLWChange(TObject *Sender,
      int Val)
{
	UpdateFuncData();
}
//---------------------------------------------------------------------------

void __fastcall TfrmShaderFunction::seArgKeyDown(TObject *Sender,
      WORD &Key, TShiftState Shift)
{
	if (Key==VK_RETURN) UpdateFuncData();
}
//---------------------------------------------------------------------------

