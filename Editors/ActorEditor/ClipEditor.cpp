#include "stdafx.h"
#pragma hdrstop

#include "ClipEditor.h"
#include <ElVCLUtils.hpp>
#include <ElTools.hpp>

#include "ElTree.hpp"
#include "ItemList.h"
#include "motion.h"
#include "skeletoncustom.h"
#include "editobject.h"
#include "UI_Tools.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "mxPlacemnt"
#pragma link "mxPlacemnt"
#pragma link "ExtBtn"
#pragma link "MXCtrls"
#pragma link "Gradient"
#pragma link "ElScrollBar"
#pragma link "ElXPThemedControl"
#pragma link "MxMenus"
#pragma link "ElPanel"
#pragma link "ElSplit"
#pragma link "multi_edit"
#pragma resource "*.dfm"

static const TColor CLIP_INACTIVE_COLOR		= 0x00686868;
static const TColor CLIP_ACTIVE_COLOR		= 0x00C1C1C1;
static const TColor BP_INACTIVE_COLOR		= 0x00686868;
static const TColor BP_ACTIVE_COLOR			= 0x00C1C1C1;

TClipMaker::SClip::SClip(const AnsiString& n, TClipMaker* own, float r_t)
{
	owner				= own;
	run_time			= r_t;
	length				= 10.f;
//    int min_size		= 1;
    cycles[0]=cycles[1]=cycles[2]=cycles[3]="";
    name				= n;
    // panel
    panel		 		= xr_new<TPanel>(owner->paClips);
    panel->Tag			= (int)this;
    panel->Alignment	= taLeftJustify;
    panel->Caption		= " "+name;
    panel->Align		= alNone;
    panel->Left			= PLeft();
    panel->Width		= PWidth();
    panel->Height		= owner->paClips->Height;
//    panel->Constraints->MinWidth = min_size;
    panel->ShowHint		= true;
    panel->Hint			= panel->Caption;
    panel->Color		= CLIP_INACTIVE_COLOR;
    panel->BevelInner	= bvLowered;//bvNone;
    panel->BevelOuter	= bvRaised;//bvNone;
    panel->BorderStyle	= bsNone;
    panel->DragMode		= dmManual;
    panel->OnDragOver	= owner->ClipDragOver;
    panel->OnDragDrop	= owner->ClipDragDrop;
    panel->OnMouseDown	= owner->ClipMouseDown;
    panel->OnMouseMove	= owner->ClipMouseMove;
    panel->OnMouseUp	= owner->ClipMouseUp;
    panel->Parent 		= owner->paClips;
}

TClipMaker::SClip::~SClip()
{
	xr_delete(panel);
};

IC bool clip_pred(TClipMaker::SClip* x, TClipMaker::SClip* y)
{
	return x->run_time<y->run_time;
};


TClipMaker*	TClipMaker::CreateForm()
{
	return xr_new<TClipMaker>((TComponent*)0);
}

void TClipMaker::DestroyForm(TClipMaker* form)
{
	xr_delete(form);
}
    
void TClipMaker::ShowEditor(CEditableObject* O)
{
	m_CurrentObject = O; VERIFY(O);
	Show			();
    RepaintClips	();
    UpdateProperties();
}

void TClipMaker::HideEditor()
{
	Clear			();
	Hide			();
}

void TClipMaker::Clear()
{
	m_CurrentObject = 0;
	for (ClipIt it=clips.begin(); it!=clips.end(); it++)
    	xr_delete	(*it);
    clips.clear		();
    sel_clip		= 0;
}

__fastcall TClipMaker::TClipMaker(TComponent* Owner) : TForm(Owner)
{
    DEFINE_INI		(fsStorage);
    m_LB[0]    		= lbBPName0;
    m_LB[1]    		= lbBPName1;
    m_LB[2]    		= lbBPName2;
    m_LB[3]    		= lbBPName3;
    m_TotalLength	= 0.f;
    m_Zoom			= 4.f;
    m_CurrentPlayTime=0.f;
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::FormCreate(TObject *Sender)
{
	m_ClipProps		= TProperties::CreateForm("Clip Properties",paClipProps,alClient);
	Device.seqFrame.Add	(this);
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::FormDestroy(TObject *Sender)
{
	Device.seqFrame.Remove(this);
	Clear			();
	TProperties::DestroyForm(m_ClipProps);
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ClipDragOver(TObject *Sender,
      TObject *Source, int X, int Y, TDragState State, bool &Accept)
{
    Accept = false;
	TElTreeDragObject* obj 	= dynamic_cast<TElTreeDragObject*>(Source);
    if (obj){
        TPanel* A			= dynamic_cast<TPanel*>(Sender); VERIFY(A);
        if (A->Tag!=-1){
            TElTree* tv		= dynamic_cast<TElTree*>(obj->Control);
            if (tv->SelectedCount){
                for (TElTreeItem* item = tv->GetNextSelected(0); item; item = tv->GetNextSelected(item)){
                    ListItem* prop		= (ListItem*)item->Tag;
					if (prop&&(prop->Type()==emMotion)){
                        Accept			= true;
                    }
                }
            }
        }
    }else{
		TPanel* P 	= dynamic_cast<TPanel*>(Source);
        if (P&&(Sender!=Source)){
            Accept=true;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ClipDragDrop(TObject *Sender,
      TObject *Source, int X, int Y)
{
	TElTreeDragObject* obj 	= dynamic_cast<TElTreeDragObject*>(Source);
    if (obj){
        TPanel* Pd			= dynamic_cast<TPanel*>(Sender); VERIFY(Pd);
        if (Pd->Tag!=-1){
	        SClip* Cd		= (SClip*)Pd->Tag;
            TElTree* tv		= dynamic_cast<TElTree*>(obj->Control);
            if (tv->SelectedCount){
                for (TElTreeItem* item = tv->GetNextSelected(0); item; item = tv->GetNextSelected(item)){
                    ListItem* prop	= (ListItem*)item->Tag; VERIFY(prop);
                    CSMotion* SM 	= (CSMotion*)prop->m_Object;
                    LPCSTR mname 	= SM->Name();
                    if (!SM->m_Flags.is(esmFX)){
                    	if (SM->m_BoneOrPart==BI_NONE){
	                        Cd->cycles[0]=mname;
	                        Cd->cycles[1]=mname;
	                        Cd->cycles[2]=mname;
	                        Cd->cycles[3]=mname;
                        }else{
	                        Cd->cycles[SM->m_BoneOrPart]=mname;
                        }
                    }
                }
            }
        }
    }else{
        if (Sender!=Source){
            TPanel* Pd		= dynamic_cast<TPanel*>(Sender); VERIFY(Pd);
            if (Pd->Tag==-1){
            	Pd			= clips.empty()?0:clips.back()->panel;
            }
            if (Pd){
                SClip* Cd		= (SClip*)Pd->Tag;
                TPanel* Ps 		= dynamic_cast<TPanel*>(Source); VERIFY(Ps);
                SClip* Cs		= (SClip*)Ps->Tag;
                if (X<Pd->Width/2)	Cs->run_time= Cd->run_time-EPS_L;
                else				Cs->run_time= Cd->run_time+EPS_L;
            }
        }
    }
    UpdateClips		();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::OnNameChange(PropValue* V)
{
	VERIFY(sel_clip);
    sel_clip->panel->Caption	= " "+sel_clip->name;
}
//------------------------------------------------------------------------------

void __fastcall TClipMaker::OnClipLengthChange(PropValue* V)
{
	UpdateClips		();
}
//------------------------------------------------------------------------------

void __fastcall TClipMaker::OnZoomChange(PropValue* V)
{
	UpdateClips		();
}
//---------------------------------------------------------------------------

void TClipMaker::RealUpdateProperties()
{
	m_RTFlags.set		(flRT_UpdateProperties,FALSE);
    // clip props
    PropItemVec		items;
    PropValue* V	= 0;
	PHelper.CreateCaption		(items,"Length",				FloatTimeToStrTime(m_TotalLength)+" s");
    V=PHelper.CreateFloat		(items,"Zoom",					&m_Zoom,			1.f,1000.f,0.1f,1);
    V->OnChangeEvent			= OnZoomChange;
    if (sel_clip){
	    V=PHelper.CreateAText	(items,"Current Clip\\Name",	&sel_clip->name);
        V->OnChangeEvent		= OnNameChange;
	    V=PHelper.CreateFloat	(items,"Current Clip\\Length",	&sel_clip->length,	0.f,10000.f,0.1f,1);
        V->OnChangeEvent		= OnClipLengthChange;
        for (u32 k=0; k<4; k++){
            LPCSTR mname		= sel_clip->CycleName(k);	
            CSMotion* SM		= m_CurrentObject->FindSMotionByName(mname);
            SBonePart* BP		= (k<m_CurrentObject->BoneParts().size())?&m_CurrentObject->BoneParts()[k]:0;
            if (BP)				PHelper.CreateCaption(items,FHelper.PrepareKey("Current Clip\\Cycles",BP->alias.c_str()), SM?SM->Name():"-");//SM->m_Flags.is(esmStopAtEnd)?"Stop at end":"Looped" );
		}            
    }
	m_ClipProps->AssignItems(items,true);
}
//---------------------------------------------------------------------------

void TClipMaker::SelectClip(SClip* clip)
{
    for (int k=0; k<paClips->ControlCount; k++){
        TPanel* P = dynamic_cast<TPanel*>(paClips->Controls[k]);
        if (P)		P->Color 			= CLIP_INACTIVE_COLOR;
    }
    if (clip)		clip->panel->Color 	= CLIP_ACTIVE_COLOR;
    sel_clip		= clip;
    RepaintClips	();
    UpdateProperties();
}

static int yyy=0;
void TClipMaker::InsertClip()
{
	SClip* clip		= xr_new<SClip>(AnsiString(yyy++),this,sel_clip?sel_clip->RunTime():0);
    clips.push_back	(clip);
    UpdateClips		();
    SelectClip		(clip);
    RepaintClips	();
}
//---------------------------------------------------------------------------

void TClipMaker::AppendClip()
{
	SClip* clip		= xr_new<SClip>(AnsiString(yyy++),this,sel_clip?sel_clip->RunTime()+sel_clip->Length():0);
    clips.push_back	(clip);
    SelectClip		(clip);
    UpdateClips		();
    RepaintClips	();
}
//---------------------------------------------------------------------------

void TClipMaker::RemoveAllClips()
{
	SelectClip(0);
	for (ClipIt it=clips.begin(); it!=clips.end(); it++)
    	xr_delete(*it);
    clips.clear		();
    RepaintClips	();
}
//---------------------------------------------------------------------------

void TClipMaker::RemoveCurrent()
{
	if (sel_clip){
        clips.erase	(std::find(clips.begin(),clips.end(),sel_clip));
        xr_delete	(sel_clip);
        SelectClip	(0);
	    RepaintClips();
    }
}
//---------------------------------------------------------------------------

void TClipMaker::ClearCurrent()
{
	if (sel_clip){
    	for (int k=0; k<4; k++) sel_clip->cycles[k]="";
	    RepaintClips();
    }
}
//---------------------------------------------------------------------------

void TClipMaker::ClearAll()
{
	for (ClipIt it=clips.begin(); it!=clips.end(); it++)
    	for (int k=0; k<4; k++) (*it)->cycles[k]="";
    RepaintClips();
}
//---------------------------------------------------------------------------

static BOOL g_resizing	= FALSE;
static int 	g_X_prev	= 0;
void __fastcall TClipMaker::ClipMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPanel* P = dynamic_cast<TPanel*>(Sender);
    if (P){
        if (Button==mbRight){
            TPoint pt; pt.x = X; pt.y = Y;
            pt=P->ClientToScreen(pt);
            pmClip->Popup(pt.x,pt.y-10);
        }else if (Button==mbLeft){
	        SelectClip	((SClip*)P->Tag);
            float p 	= float(X)/P->Width;
            if ((p>0.75f)&&((P->Width-X)<10)){
                g_resizing	= TRUE;
                g_X_prev 	= X;
            }else{
				if (P->Tag!=-1) P->BeginDrag(false, 2);
            }
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ClipMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
    VERIFY				(sel_clip);
	TPanel* P 			= dynamic_cast<TPanel*>(Sender);
    float p 			= float(X)/P->Width;
	if ((p>0.75f)&&((P->Width-X)<10)){
    	P->Cursor 		= crHSplit;
    }else{
    	if (!g_resizing)
	    	P->Cursor 	= crDefault;
    }
    if (g_resizing){
        float dx	= float(X-g_X_prev)/m_Zoom;
        if (!fis_zero(dx)){
    	    sel_clip->length += dx;
            if (sel_clip->length<EPS_L) sel_clip->length=EPS_L;
    		g_X_prev 	= X;
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ClipMouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
	if (Button==mbLeft){
        g_resizing	= FALSE;
        UpdateClips	();
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::MenuItem2Click(TObject *Sender)
{
	RemoveCurrent	();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ClearMotions1Click(TObject *Sender)
{
	ClearCurrent	();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::RemoveAll1Click(TObject *Sender)
{
	RemoveAllClips	();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ClearAll1Click(TObject *Sender)
{
	ClearAll();	
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::gtClipPaint(TObject *Sender)
{
	TCanvas* canvas 	= gtClip->GetCanvas();
    canvas->Font->Name 	= "MS Sans Serif";
    canvas->Font->Style	= TFontStyles();
	canvas->Pen->Color 	= clBlack;
    canvas->Pen->Width	= 1;
    canvas->Pen->Style	= psSolid;  
	for (ClipIt it=clips.begin(); it!=clips.end(); it++){
        canvas->MoveTo	((*it)->PLeft(), 0);
        canvas->LineTo	((*it)->PLeft(), 6);
        TRect R 		= TRect((*it)->PLeft()+1, 1, (*it)->PRight(), 15);
		AnsiString s	= AnsiString().sprintf("%2.1f",(*it)->RunTime());
        float dx		= 2.f;
        float dy		= canvas->TextHeight(s);
        R.Left -= dx; 	R.Right -= dx;
        R.Top  = 21-dy; R.Bottom = R.Top+dy;
        canvas->TextRect(R,R.Left,R.Top,s);
	}
    if (!clips.empty()){
        canvas->MoveTo	(clips.back()->PRight()-1, 0);
        canvas->LineTo	(clips.back()->PRight()-1, 6);
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::BPOnPaint(TObject *Sender)
{
	TGradient* bp 		= dynamic_cast<TGradient*>(Sender); VERIFY(bp);
    CEditableObject* O	= 	m_CurrentObject;
    if (O&&(bp->Tag<(int)O->BoneParts().size())){
        TCanvas* canvas 	= bp->GetCanvas();
        canvas->Font->Name 	= "MS Sans Serif";
        canvas->Font->Style	= TFontStyles();
        canvas->Font->Color = clBlack;
        canvas->Pen->Color 	= clBlack;
        canvas->Pen->Style	= psSolid;
        canvas->Brush->Style= bsSolid;
        CSMotion* SM_prev	= 0;
        for (ClipIt it=clips.begin(); it!=clips.end(); it++){
            LPCSTR mname	= (*it)->CycleName(bp->Tag);	
            CSMotion* SM	= O->FindSMotionByName(mname);
            TRect R 		= TRect((*it)->PLeft(), 1, (*it)->PRight()-1, 15);
            if (SM){
		        canvas->Pen->Width	= 1;
                canvas->Rectangle	(R);
                R.Top				+= 1;
                R.Bottom			-= 1;
                R.Left				+= 1;
                R.Right				-= 1;
                canvas->Brush->Color= (*it==sel_clip)?BP_ACTIVE_COLOR:BP_INACTIVE_COLOR;
                canvas->TextRect	(R,R.Left,R.Top,SM->Name());
	            SM_prev				= SM;
            }else if (SM_prev){
		        canvas->Pen->Width	= 1;
                canvas->MoveTo		((*it)->PLeft()+1,13);
                canvas->LineTo		(R.Right,13);
                canvas->LineTo		(R.Width()>5?R.Right-5:R.Right-R.Width(),8);
                R.Top				+= 1;
                R.Bottom			-= 1;
                R.Left				+= 1;
                R.Right				-= 1;
            }
        }
    }
}
//---------------------------------------------------------------------------

void TClipMaker::RealRepaintClips()
{
/*
	PostMessage			(paFrame->Handle,WM_SETREDRAW,FALSE,0);
*/    
    m_RTFlags.set		(flRT_RepaintClips,FALSE);
	// update panel size
    m_TotalLength		= 0.f;
	for (ClipIt it=clips.begin(); it!=clips.end(); it++){
    	(*it)->panel->Left 	= (*it)->run_time*m_Zoom;
    	(*it)->panel->Width = (*it)->length*m_Zoom;
        m_TotalLength	+= (*it)->length;
    }
	paFrame->Width		= m_TotalLength*m_Zoom;
    // repaint
    gtClip->Repaint		();
    gtBP0->Repaint		();
    gtBP1->Repaint		();
    gtBP2->Repaint		();
    gtBP3->Repaint		();          
	// set BP name                   
    CEditableObject* O	= m_CurrentObject;
    u32 k				= 0;
    if (O){
	    BPVec& bps 		= O->BoneParts();
        for (; k<bps.size(); k++)
        	m_LB[k]->Caption = bps[k].alias;
    }
	for (; k<4; k++)	m_LB[k]->Caption	= "-";
    UpdateProperties	();
/*                  
	PostMessage			(paFrame->Handle,WM_SETREDRAW,TRUE,0);
//    paFrame->Repaint	();
	TRect R = paFrame->ClientRect;
    InvalidateRect		(paFrame->Handle,&R,TRUE);
*/
}
//---------------------------------------------------------------------------

void TClipMaker::RealUpdateClips()
{
	m_RTFlags.set	(flRT_UpdateClips,FALSE);
    m_TotalLength	= 0.f;
    std::sort		(clips.begin(),clips.end(),clip_pred);
	for (ClipIt it=clips.begin(); it!=clips.end(); it++){
    	(*it)->run_time	= m_TotalLength;
    	(*it)->panel->Left 	= (*it)->run_time*m_Zoom;
    	(*it)->panel->Width = (*it)->length*m_Zoom;
        m_TotalLength	+= (*it)->length;
    }
    RepaintClips	();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::fsStorageRestorePlacement(TObject *Sender)
{
	m_ClipProps->RestoreParams(fsStorage);
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::fsStorageSavePlacement(TObject *Sender)
{
	m_ClipProps->SaveParams(fsStorage);
}
//---------------------------------------------------------------------------

void TClipMaker::OnFrame()
{
	if (m_RTFlags.is(flRT_UpdateClips))
    	RealUpdateClips();
	if (m_RTFlags.is(flRT_RepaintClips))
    	RealRepaintClips();
    if (m_RTFlags.is(flRT_UpdateProperties))
    	RealUpdateProperties();
    if (m_RTFlags.is(flRT_Playing)){
	    m_CurrentPlayTime+=Device.fTimeDelta;
    	if (m_CurrentPlayTime>m_TotalLength) m_CurrentPlayTime-=m_TotalLength;
//. playing
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebInsertClipClick(TObject *Sender)
{
	InsertClip		();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebAppendClipClick(TObject *Sender)
{
	AppendClip		();	
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebRemoveAllClick(TObject *Sender)
{
	RemoveAllClips	();
}
//---------------------------------------------------------------------------


void __fastcall TClipMaker::ebRemoveCurrentClick(TObject *Sender)
{
	RemoveCurrent	();	
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebPrevClipClick(TObject *Sender)
{
	if (sel_clip){
		ClipIt it = std::find(clips.begin(),clips.end(),sel_clip);
        if (it!=clips.begin()){
	        it--;
            SelectClip(*it);
    	}    
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebNextClipClick(TObject *Sender)
{
	if (sel_clip){
		ClipIt it = std::find(clips.begin(),clips.end(),sel_clip);
        if (it!=clips.end()){
	        it++;
	        if (it!=clips.end())
	            SelectClip(*it);
    	}    
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebPlayClick(TObject *Sender)
{
    Play		();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebPauseClick(TObject *Sender)
{
    Pause		();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ebStopClick(TObject *Sender)
{
    Stop		();
}
//---------------------------------------------------------------------------

