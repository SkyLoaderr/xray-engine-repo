//---------------------------------------------------------------------------

#ifndef EditLightAnimH
#define EditLightAnimH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include "multi_edit.hpp"
#include "Gradient.hpp"
#include "ElTrackBar.hpp"

//---------------------------------------------------------------------------
// refs
class CLAItem;
//---------------------------------------------------------------------------

class TfrmEditLightAnim : public TForm
{
__published:	// IDE-managed Components
	TPanel *paItemProps;
	TFormStorage *fsStorage;
	TPanel *paColor;
	TPanel *Panel3;
	TElTree *tvItems;
	TPanel *Panel2;
	TExtBtn *ebAddAnim;
	TExtBtn *ebDeleteAnim;
	TMultiObjSpinEdit *seFPS;
	TEdit *edName;
	TBevel *Bevel1;
	TLabel *Label1;
	TLabel *Label2;
	TLabel *Label3;
	TExtBtn *ebSave;
	TExtBtn *ebReload;
	TMultiObjSpinEdit *seFrameCount;
	TPaintBox *pbG;
	TLabel *Label4;
	TMultiObjSpinEdit *sePointer;
	TStaticText *stStartFrame;
	TStaticText *stEndFrame;
	TExtBtn *ExtBtn1;
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall tvItemsDblClick(TObject *Sender);
	void __fastcall tvItemsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvItemsItemFocused(TObject *Sender);
	void __fastcall ebAddAnimClick(TObject *Sender);
	void __fastcall ebDeleteAnimClick(TObject *Sender);
	void __fastcall ebSaveClick(TObject *Sender);
	void __fastcall ebReloadClick(TObject *Sender);
	void __fastcall pbGPaint(TObject *Sender);
	void __fastcall pbGClick(TObject *Sender);
	void __fastcall seFrameCountExit(TObject *Sender);
	void __fastcall seFrameCountLWChange(TObject *Sender, int Val);
	void __fastcall seFrameCountKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall sePointerChange(TObject *Sender);
private:	// User declarations
    void InitItems();
    bool CloseEditLibrary(bool bReload);
	static TfrmEditLightAnim *form;
	static AnsiString m_LastSelection;
    CLAItem* m_CurrentItem;
    void	SetCurrentItem(CLAItem* item);
    void 	GetItemData();
    bool 	bFreezeUpdate;
    void	UpdateView();
    void 	OnModified();
public:		// User declarations
    __fastcall TfrmEditLightAnim(TComponent* Owner);
    static bool FinalClose();
    static void __fastcall ShowEditor();
};
//---------------------------------------------------------------------------
#endif
