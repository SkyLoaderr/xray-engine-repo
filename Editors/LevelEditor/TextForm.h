//---------------------------------------------------------------------------
#ifndef TextFormH
#define TextFormH
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
#include "multi_check.hpp"
#include <StdCtrls.hpp>
#include "multi_edit.hpp"
//---------------------------------------------------------------------------
#include <Forms.hpp>
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
#include "ElACtrls.hpp"
#include "ElStatBar.hpp"
#include "ElXPThemedControl.hpp"
#include "MxMenus.hpp"
#include <Menus.hpp>

// refs
class CCustomObject;

typedef void __fastcall (__closure *TOnApplyClick)();
typedef void __fastcall (__closure *TOnCloseClick)(bool& can_close);
typedef bool __fastcall (__closure *TOnCodeIndight)(const AnsiString& src_line, AnsiString& hint);

class TfrmText : public TForm
{
__published:	// IDE-managed Components
	TPanel *paBottomBar;
	TExtBtn *ebOk;
	TExtBtn *ebCancel;
	TFormStorage *fsStorage;
	TElAdvancedMemo *mmText;
	TExtBtn *ebApply;
	TExtBtn *ebLoad;
	TExtBtn *ebSave;
	TElStatusBar *sbStatusPanel;
	TMxPopupMenu *pmTextMenu;
	TExtBtn *ebClear;
	TTimer *tmIdle;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
	void __fastcall mmTextChange(TObject *Sender);
	void __fastcall ebApplyClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ebLoadClick(TObject *Sender);
	void __fastcall ebSaveClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall mmTextKeyUp(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall ebClearClick(TObject *Sender);
	void __fastcall tmIdleTimer(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall FormDeactivate(TObject *Sender);
private:	// User declarations
	AnsiString* 	m_Text;
    TOnApplyClick 	OnApplyClick;
    TOnCloseClick 	OnCloseClick;
    TOnCodeIndight  OnCodeInsight;
	static TfrmText*  form;
    void			OutLineNumber();
public:		// User declarations
	enum{
    	flReadOnly 	= (1<<0),
    	flOurPPMenu	= (1<<1),
    };
public:    
    __fastcall TfrmText(TComponent* Owner);
    static TfrmText* 	__fastcall ShowEditor		(AnsiString& text, LPCSTR caption="Text", u32 flags=0, int lim=0, TOnApplyClick on_apply=0, TOnCloseClick on_close=0, TOnCodeIndight on_insight=0);
    static bool		 	__fastcall ShowModalEditor	(AnsiString& text, LPCSTR caption="Text", u32 flags=0, int lim=0, TOnApplyClick on_apply=0, TOnCloseClick on_close=0, TOnCodeIndight on_insight=0);
    static void 		__fastcall HideEditor		();
    bool 	Modified	(){return mmText->Modified;}
    void 	ApplyEdit	(){ebApplyClick(0);}
    void 	InsertLine	(const AnsiString& line);
};
//---------------------------------------------------------------------------
#endif
