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

class TClipMaker: public TForm,
	public pureFrame
{
__published:	// IDE-managed Components
	TFormStorage *fsStorage;
	TMxPopupMenu *pmClip;
	TMenuItem *MenuItem1;
	TMenuItem *MenuItem2;
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
	TScrollBox *sbBase;
	TMxPanel *paFrame;
	TBevel *Bevel6;
	TBevel *Bevel7;
	TBevel *Bevel8;
	TBevel *Bevel1;
	TBevel *Bevel3;
	TPanel *paClips;
	TBevel *Bevel9;
	TGradient *gtClip;
	TExtBtn *ebPlay;
	TExtBtn *ebPause;
	TExtBtn *ebStop;
	TExtBtn *ebPrevClip;
	TExtBtn *ebNextClip;
	TBevel *Bevel17;
	TExtBtn *ebClearAll;
	TExtBtn *ebRemoveCurrent;
	TMenuItem *N1;
	TMenuItem *ClearMotions1;
	TMenuItem *ClearAll1;
	TMxPanel *paBP3;
	TMxPanel *paBP2;
	TMxPanel *paBP1;
	TMxPanel *paBP0;
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
	void __fastcall ClipDragOver(TObject *Sender, TObject *Source, int X,
          int Y, TDragState State, bool &Accept);
	void __fastcall ClipDragDrop(TObject *Sender, TObject *Source, int X,
          int Y);
	void __fastcall ClipMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall ebRemoveAllClick(TObject *Sender);
	void __fastcall ebRemoveCurrentClick(TObject *Sender);
	void __fastcall ClearMotions1Click(TObject *Sender);
	void __fastcall ClearAll1Click(TObject *Sender);
	void __fastcall ebPrevClipClick(TObject *Sender);
	void __fastcall ebNextClipClick(TObject *Sender);
	void __fastcall ebPlayClick(TObject *Sender);
	void __fastcall ebPauseClick(TObject *Sender);
	void __fastcall ebStopClick(TObject *Sender);
	void __fastcall ClipMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
	void __fastcall ClipMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall FormShow(TObject *Sender);
public:
	struct SClip{
    	TPanel*			panel;
        AnsiString		cycles[4];
        AStringVec		fxs;
        AnsiString		name;
        float			length;
        float			run_time;
        TClipMaker* 	owner;
    public:
        				SClip			(const AnsiString& name, TClipMaker* owner, float r_t);
        				~SClip			();
        int				PWidth			(){return length*owner->m_Zoom;}
        int				PLeft			(){return run_time*owner->m_Zoom;}
        int				PRight			(){return PLeft()+PWidth();}
        float			Length			(){return length;}
        float			RunTime			(){return run_time;}
        LPCSTR			CycleName		(u16 bp){VERIFY(bp<4); return (cycles[bp]=="")?0:cycles[bp].c_str();}
    };
protected:
    enum{
    	flRT_RepaintClips		= (1<<0),
    	flRT_UpdateProperties	= (1<<1),
    	flRT_UpdateClips		= (1<<2),
    	flRT_Playing			= (1<<3),
    };
    Flags32				m_RTFlags;
	
    TMxLabel* 			m_LB[4];
    CEditableObject* 	m_CurrentObject;
	DEFINE_VECTOR		(SClip*,ClipVec,ClipIt);
    ClipVec				clips;
    SClip*				sel_clip;
    SClip*				play_clip;
    TProperties*		m_ClipProps;
	void 				InsertClip		();
	void 				AppendClip		();
	void 				RemoveAllClips	();
	void				ClearCurrent	();
	void				ClearAll		();
	void				RemoveCurrent	();
    void				SelectClip		(SClip* clip);

    void				RealRepaintClips();
    void				RepaintClips	(bool bForced=false){m_RTFlags.set(flRT_RepaintClips,TRUE); if(bForced) RealRepaintClips(); }
	void 				RealUpdateProperties();
    void				UpdateProperties(bool bForced=false){m_RTFlags.set(flRT_UpdateProperties,TRUE); if(bForced) RealUpdateProperties(); }
	void 				RealUpdateClips	();
    void				UpdateClips		(bool bRepaint=true, bool bForced=false){m_RTFlags.set(flRT_UpdateClips,TRUE); m_RTFlags.set(flRT_RepaintClips,bRepaint); if(bForced) RealUpdateClips(); }

    void				Clear			();

	void __fastcall 	OnZoomChange		(PropValue* V);
    void __fastcall 	OnNameChange		(PropValue* V);
    void __fastcall 	OnClipLengthChange	(PropValue* V);
public:
    float				m_CurrentPlayTime;
    float				m_TotalLength;
    float				m_Zoom;

    void				Play			(){m_RTFlags.set(flRT_Playing,TRUE);}
    void				Stop			(){m_RTFlags.set(flRT_Playing,FALSE);m_CurrentPlayTime=0.f;}
    void				Pause			(){m_RTFlags.set(flRT_Playing,FALSE);}
public:		// User declarations
	__fastcall 			TClipMaker		(TComponent* Owner);

    static TClipMaker*	CreateForm		();
    static void			DestroyForm		(TClipMaker* form);
    
    void				ShowEditor		(CEditableObject* O);
    void				HideEditor		();

	virtual void		OnFrame			(void);
};
//---------------------------------------------------------------------------
#endif
