//---------------------------------------------------------------------------
#ifndef ClipEditorH
#define ClipEditorH
#include "mxPlacemnt.hpp"
#include <Classes.hpp>
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include <Forms.hpp>
#include "Gradient.hpp"
#include "ElScrollBar.hpp"
#include "ElXPThemedControl.hpp"
#include "MxMenus.hpp"
#include <Menus.hpp>
#include "PropertiesList.h"
#include "ElPanel.hpp"
#include "ElSplit.hpp"
#include <StdCtrls.hpp>
#include "multi_edit.hpp"
//---------------------------------------------------------------------------

class TClipMaker: public TForm
{
__published:	// IDE-managed Components
	TFormStorage *fsStorage;
	TMxPopupMenu *pmClip;
	TMenuItem *MenuItem1;
	TMenuItem *MenuItem2;
	TMenuItem *MenuItem3;
	TMenuItem *RemoveAll1;
	TPanel *paB;
	TPanel *paBase;
	TPanel *paClipProps;
	TBevel *Bevel20;
	TPanel *paA;
	TPanel *Panel1;
	TMxLabel *MxLabel1;
	TMxLabel *MxLabel2;
	TMxLabel *MxLabel3;
	TBevel *Bevel2;
	TBevel *Bevel4;
	TBevel *Bevel5;
	TBevel *Bevel10;
	TBevel *Bevel11;
	TBevel *Bevel12;
	TMxLabel *MxLabel4;
	TMxLabel *lbBPName0;
	TBevel *Bevel13;
	TMxLabel *lbBPName1;
	TMxLabel *lbBPName2;
	TMxLabel *lbBPName3;
	TExtBtn *ebInsertClip;
	TExtBtn *ebAppendClip;
	TBevel *Bevel14;
	TBevel *Bevel15;
	TBevel *Bevel16;
	TMxLabel *MxLabel5;
	TBevel *Bevel17;
	TMultiObjSpinEdit *seLength;
	TMxLabel *MxLabel7;
	TMxLabel *MxLabel8;
	TBevel *Bevel18;
	TMultiObjSpinEdit *seScale;
	TScrollBox *sbBase;
	TPanel *paFrame;
	TBevel *Bevel6;
	TBevel *Bevel7;
	TBevel *Bevel8;
	TBevel *Bevel1;
	TBevel *Bevel3;
	TPanel *paClips;
	TBevel *Bevel9;
	TGradient *gtClip;
	TGradient *gtBP0;
	TGradient *gtBP3;
	TGradient *gtBP2;
	TGradient *gtBP1;
	void __fastcall ebInsertClipClick(TObject *Sender);
	void __fastcall MenuItem2Click(TObject *Sender);
	void __fastcall RemoveAll1Click(TObject *Sender);
	void __fastcall gtClipPaint(TObject *Sender);
	void __fastcall ebAppendClipClick(TObject *Sender);
	void __fastcall BPOnPaint(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall fsStorageRestorePlacement(TObject *Sender);
	void __fastcall fsStorageSavePlacement(TObject *Sender);
	void __fastcall sbClipsScroll(TObject *Sender, TScrollCode ScrollCode,
          int &ScrollPos);
	void __fastcall paFrameResize(TObject *Sender);
	void __fastcall seLengthChange(TObject *Sender);
	void __fastcall seScaleChange(TObject *Sender);

	void __fastcall ClipDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall ClipDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall ClipMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall SplitterMoved(TObject *Sender);
	void __fastcall ClipResize(TObject *Sender);
	void __fastcall Panel2MouseMove(TObject *Sender, TShiftState Shift, int X,
          int Y);
public:
	struct SClip{
    	TPanel*			panel;
        TSplitter*		splitter;
        AnsiString		cycles[4];
        AStringVec		fxs;
        float			length;
    public:
        				SClip			(const AnsiString& name, TClipMaker* owner, int pos);
        				~SClip			();
    	void			OnMove			();
        int				Left			(){return panel->Left;}
        int				Right			(){return Left()+Width();}
        int				Width			(){return panel->Width+splitter->Width;}
        float			Length			(){return length;}
        LPCSTR			CycleName		(u16 bp){VERIFY(bp<4); return (cycles[bp]=="")?0:cycles[bp].c_str();}
    };
protected:
    TMxLabel* 			m_LB[4];
    CEditableObject* 	m_CurrentObject;
	DEFINE_VECTOR		(SClip*,ClipVec,ClipIt);
    ClipVec				clips;
    SClip*				sel_clip;
    TProperties*		m_ClipProps;
	void 				InsertClip		();
	void 				AppendClip		();
    void				SelectClip		(SClip* clip);
    void				ClipRepaint		();

    void				Clear			();
public:		// User declarations
	__fastcall 			TClipMaker		(TComponent* Owner);

    static TClipMaker*	CreateForm		();
    static void			DestroyForm		(TClipMaker* form);
    
    void				ShowEditor		(CEditableObject* O);
    void				HideEditor		();
};
//---------------------------------------------------------------------------
#endif
