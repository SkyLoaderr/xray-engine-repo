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

static const TColor CLIP_INACTIVE_COLOR		= 0x00787878;
static const TColor CLIP_ACTIVE_COLOR		= 0x00C1F1D1;
static const TColor BP_INACTIVE_COLOR		= 0x00787878;
static const TColor BP_ACTIVE_COLOR			= 0x00C1C1C1;
static const int	SPLITTER_WIDTH			= 2;

TClipMaker::SClip::SClip(const AnsiString& name, TClipMaker* owner, int p)
{
	length				= 10.f;
    int min_size		= 4;
    cycles[0]=cycles[1]=cycles[2]=cycles[3]="";
    // panel
    panel		 		= xr_new<TPanel>(owner->paClips);
    panel->Tag			= (int)this;
    panel->Caption		= name;
    panel->Align		= alLeft;
    panel->Left			= p;
    panel->Width		= min_size;
    panel->Constraints->MinWidth = min_size;
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
    panel->OnResize		= owner->ClipResize;
    panel->Parent 		= owner->paClips;
    // splitter
    splitter			= xr_new<TSplitter>(owner->paClips);
    splitter->Tag		= (int)this;
    splitter->AutoSnap	= false;
//    splitter->Color		= clBlue;                     
    splitter->MinSize	= min_size;
    splitter->Align		= alLeft;
    splitter->Width		= SPLITTER_WIDTH;
//    splitter->Beveled	= true;
    splitter->ResizeStyle= rsUpdate;
    splitter->Constraints->MinWidth = splitter->Width;
    splitter->Constraints->MaxWidth = splitter->Width;
    splitter->Left		= panel->Left+1;
    splitter->OnMoved	= owner->SplitterMoved;
    splitter->Parent 	= owner->paClips;
    splitter->Visible 	= false;
}

TClipMaker::SClip::~SClip()
{
	xr_delete(panel);
    xr_delete(splitter);
};

void TClipMaker::SClip::OnMove()
{
	splitter->Left	= panel->Left+1;
}

IC bool clip_pred(TClipMaker::SClip* x, TClipMaker::SClip* y)
{
	return x->panel->Left<y->panel->Left;
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
//. втулить инициализацию paFrame (размер от скейла)
//.	seScale->Value	= ;
//.	seLength->Value	= ;
	paFrame->Width	= seLength->Value*seScale->Value;
	Show			();
    ClipRepaint		();
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
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::FormCreate(TObject *Sender)
{
	m_ClipProps		= TProperties::CreateForm("Clip Properties",paClipProps,alClient);
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::FormDestroy(TObject *Sender)
{
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
                if (X<Pd->Width/2)	Ps->Left 	= Pd->Left;
                else				Ps->Left  	= Pd->Left+Cd->Width();
                Cs->OnMove		();
			    std::sort		(clips.begin(),clips.end(),clip_pred);
            }
        }
    }
    ClipRepaint		();
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
    // clip props
    PropItemVec		items;
    if (sel_clip){
	    PHelper.CreateCaption(items,"Name",sel_clip->panel->Caption);
    }
	m_ClipProps->AssignItems(items,true);
    // repaint
    ClipRepaint		();
}

static int yyy=0;
void TClipMaker::InsertClip()
{
//. не вставлять если не влазит
	int pos 		= sel_clip?sel_clip->Left():0;
	SClip* clip		= xr_new<SClip>(AnsiString(yyy++),this,pos);
    clips.push_back	(clip);
    std::sort		(clips.begin(),clips.end(),clip_pred);
    SelectClip		(clip);
    ClipRepaint		();
}
//---------------------------------------------------------------------------

void TClipMaker::AppendClip()
{
//. не вставлять если не влазит
	int pos 		= sel_clip?sel_clip->Right():(clips.empty()?0:clips.back()->Right());
	SClip* clip		= xr_new<SClip>(AnsiString(yyy++),this,pos);
    clips.push_back	(clip);
    std::sort		(clips.begin(),clips.end(),clip_pred);
    SelectClip		(clip);
    ClipRepaint		();
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

void __fastcall TClipMaker::ClipMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
    TPanel* P = dynamic_cast<TPanel*>(Sender);
    if (Button==mbRight){
        if (P){
            TPoint pt; pt.x = X; pt.y = Y;
            pt=P->ClientToScreen(pt);
            pmClip->Tag = (int)P->Tag;
            pmClip->Popup(pt.x,pt.y-10);
        }
    }else if (Button==mbLeft){
        SelectClip	((SClip*)P->Tag);
        P->BeginDrag(false, 2);
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::MenuItem2Click(TObject *Sender)
{
    SClip*		C = (SClip*)pmClip->Tag; VERIFY(C);
    if (sel_clip==C)  SelectClip(0);
    clips.erase(std::find(clips.begin(),clips.end(),C));
    xr_delete	(C);
    ClipRepaint	();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::RemoveAll1Click(TObject *Sender)
{
	SelectClip(0);
	for (ClipIt it=clips.begin(); it!=clips.end(); it++)
    	xr_delete(*it);
    clips.clear();
    ClipRepaint	();
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
    float t				= 0.f;
	for (ClipIt it=clips.begin(); it!=clips.end(); it++){
        canvas->MoveTo	((*it)->Left(), 1);
        canvas->LineTo	((*it)->Left(), 10);
        TRect R 		= TRect((*it)->Left()+1, 1, (*it)->Right(), 15);
//        canvas->TextRect(R,R.Left,R.Top,AnsiString().sprintf("%s",FloatTimeToStrTime(t)));
        canvas->TextRect(R,R.Left,R.Top,AnsiString().sprintf("%2.1f",t));
		t				+= (*it)->Length();
	}
    if (!clips.empty()){
        canvas->MoveTo	(clips.back()->Right(), 1);
        canvas->LineTo	(clips.back()->Right(), 10);
//        canvas->TextOut	(clips.back()->Right()+1,1,AnsiString().sprintf("%s",FloatTimeToStrTime(t)));
        canvas->TextOut	(clips.back()->Right()+1,1,AnsiString().sprintf("%2.1f",t));
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::ClipResize(TObject *Sender)
{
//.
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::SplitterMoved(TObject *Sender)
{
	SClip* clip		= (SClip*)((TComponent*)Sender)->Tag;
    clip->length	= float(clip->panel->Width+SPLITTER_WIDTH)/float(seScale->Value);
    ClipRepaint		();            
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
        canvas->Pen->Width	= 1;
        canvas->Pen->Style	= psSolid;
        canvas->Brush->Style= bsSolid;
        for (ClipIt it=clips.begin(); it!=clips.end(); it++){
            canvas->Brush->Color= (*it==sel_clip)?BP_ACTIVE_COLOR:BP_INACTIVE_COLOR;
            LPCSTR mname	= (*it)->CycleName(bp->Tag);	
            CSMotion* SM	= O->FindSMotionByName(mname);
            if (SM){
                TRect R 			= TRect((*it)->Left(), 1, (*it)->Left()+(*it)->Width(), 15);
                canvas->Rectangle	(R);
                R.Top				+= 1;
                R.Bottom			-= 1;
                R.Left				+= 1;
                R.Right				-= 1;
                canvas->TextRect	(R,R.Left,R.Top,SM->Name());
            }
        }
    }
}
//---------------------------------------------------------------------------

void TClipMaker::ClipRepaint()
{
	// update panle size
	for (ClipIt it=clips.begin(); it!=clips.end(); it++){
    	(*it)->panel->Width = ((*it)->length*seScale->Value-SPLITTER_WIDTH);
    }
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

void __fastcall TClipMaker::sbClipsScroll(TObject *Sender,
      TScrollCode ScrollCode, int &ScrollPos)
{
	if (paBase->Width<paFrame->Width){
        paFrame->Left	= -ScrollPos;
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::paFrameResize(TObject *Sender)
{
    if (paBase->Width<paFrame->Width){
	    sbClips->Max		= paFrame->Width-paBase->Width;
    	sbClips->LargeChange= iFloor(paBase->Width/(float(paFrame->Width)/float(paBase->Width)));
    	sbClips->PageSize	= iFloor(float(sbClips->Max)/5.f);
    	sbClips->Enabled	= true;
    }else{
    	sbClips->Enabled	= false;
    }
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::seLengthChange(TObject *Sender)
{
	float ot = 0.f;
	for (ClipIt it=clips.begin(); it!=clips.end(); it++)
    	ot += (*it)->length;
    int min_w = ot*seScale->Value;
    int new_w = seLength->Value*seScale->Value;
    if (new_w<min_w){
    	new_w 			= min_w;
		seLength->Value = float(min_w)/seScale->Value;
    }
	paFrame->Width		= new_w;
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::seScaleChange(TObject *Sender)
{
	paFrame->Width	= seLength->Value*seScale->Value;
    ClipRepaint		();
}
//---------------------------------------------------------------------------

void __fastcall TClipMaker::Panel2MouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
	TPanel* P 	= dynamic_cast<TPanel*>(Sender); VERIFY(P);
    float x 	= float(X)/P->Width;
    int xx		= 0;
//	if (x<0.1f){
//    	P->Cursor	= crHSplit;
//		xx		= -1;
//	}else 
	if (x>0.9f){
    	P->Cursor	= crHSplit;
		xx			= 0;
    }else{
    	P->Cursor	= crDefault;
    }
    if (P->Cursor==crHSplit){
    	if (Shift.Contains(ssLeft))
	    	P->Width+= X-P->Tag;
        else
	    	P->Tag	= X;
    }
//	if ()
}
//---------------------------------------------------------------------------

