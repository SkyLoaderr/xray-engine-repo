//---------------------------------------------------------------------------
#ifndef PropertiesListH
#define PropertiesListH
//---------------------------------------------------------------------------

#include "ElTree.hpp"
#include "ElTreeStdEditors.hpp"
#include "ElXPThemedControl.hpp"
#include "multi_edit.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>
#include <ExtCtrls.hpp>
#include "ElTreeAdvEdit.hpp"
#include <Mask.hpp>

#include "PropertiesListHelper.h"
#include "ElBtnCtl.hpp"
#include "ElPopBtn.hpp"
#include "ExtBtn.hpp"

class TProperties : public TForm
{
__published:	// IDE-managed Components
	TElTree 			*tvProperties;
	TMxPopupMenu 		*pmEnum;
	TFormStorage 		*fsStorage;
	TMultiObjSpinEdit 	*seNumber;
	TEdit 				*edText;
	TBevel 				*Bevel1;
	TElPopupButton *pbExtBtn;
	TMxPopupMenu *pmSystem;
	TMenuItem *ExpandAll1;
	TMenuItem *N1;
	TMenuItem *CollapseAll1;
	TMenuItem *N2;
	TMenuItem *miDrawThumbnails;
	TMxPopupMenu *pmItems;
	TMenuItem *ExpandSelected1;
	TMenuItem *CollapseSelected1;
	TMenuItem *N3;
	TPanel *paButtons;
	TBevel *Bevel2;
	TExtBtn *ebLightAnimationEditor;
	TExtBtn *ExtBtn1;
	void __fastcall 	FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall 	tvPropertiesClick(TObject *Sender);
	void __fastcall 	tvPropertiesItemDraw(TObject *Sender, TElTreeItem *Item, TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall 	tvPropertiesMouseDown(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall 	seNumberExit(TObject *Sender);
	void __fastcall 	seNumberKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall 	edTextExit(TObject *Sender);
	void __fastcall 	edTextKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall 	tvPropertiesItemFocused(TObject *Sender);
	void __fastcall 	edTextDblClick(TObject *Sender);
	void __fastcall 	tvPropertiesHeaderColumnResize(TObject *Sender, int SectionIndex);
	void __fastcall 	FormDeactivate(TObject *Sender);
	void __fastcall 	FormShow(TObject *Sender);
	void __fastcall 	tvPropertiesItemChange(TObject *Sender, TElTreeItem *Item, TItemChangeMode ItemChangeMode);
	void __fastcall 	FormDestroy(TObject *Sender);
	void __fastcall 	pbExtBtnClick(TObject *Sender);
	void __fastcall 	fsStorageRestorePlacement(TObject *Sender);
	void __fastcall 	fsStorageSavePlacement(TObject *Sender);
	void __fastcall 	CollapseAll1Click(TObject *Sender);
	void __fastcall 	ExpandAll1Click(TObject *Sender);
	void __fastcall 	miDrawThumbnailsClick(TObject *Sender);
	void __fastcall 	tvPropertiesMouseUp(TObject *Sender, TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall 	tvPropertiesMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
	void __fastcall ExpandSelected1Click(TObject *Sender);
	void __fastcall CollapseSelected1Click(TObject *Sender);
	void __fastcall ebLightAnimationEditorClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
private:	// User declarations
    void __fastcall 	PMItemClick		(TObject *Sender);
	void __fastcall 	WaveFormClick	(TElTreeItem* item);
	void __fastcall 	ColorClick		(TElTreeItem* item);
	void __fastcall 	VectorClick		(TElTreeItem* item);
	void __fastcall 	CustomTextClick(TElTreeItem* item);
	void __fastcall 	CustomAnsiTextClick(TElTreeItem* item);

	TElTreeItem* 		m_FirstClickItem;

	Graphics::TBitmap* 	m_BMCheck;
	Graphics::TBitmap* 	m_BMDot;
	Graphics::TBitmap* 	m_BMEllipsis;
    bool 				bModified;
    AnsiString 			last_selected_item;
	// LW style inpl editor
    void 				HideLWNumber	();
    void 				PrepareLWNumber(TElTreeItem* node);
    void 				ShowLWNumber	(TRect& R);
    void 				ApplyLWNumber	();
    void 				CancelLWNumber	();
	// text inplace editor
    void 				ExecTextEditor	(PropItem* prop);
    void 				HideLWText		();
    void 				PrepareLWText	(TElTreeItem* node);
    void 				ShowLWText		(TRect& R);
    void 				ApplyLWText	();
    void 				CancelLWText	();
    // Ext Button
    void 				ShowExtBtn		(TRect& R);
    void 				HideExtBtn		();

    PropItemVec 		m_Items;
    TOnModifiedEvent 	OnModifiedEvent;
    TOnItemFocused      OnItemFocused;
    TOnCloseEvent		OnCloseEvent;
    void 				Modified				(){bModified=true; if (OnModifiedEvent) OnModifiedEvent();}
    void 				ClearParams				(TElTreeItem* node=0);
    void 				ApplyEditControl		();
    void 				CancelEditControl		();

	void 				OutBOOL					(BOOL val, TCanvas* Surface, const TRect& R, bool bEnable);
	void 				OutText					(LPCSTR text, TCanvas* Surface, TRect R, bool bEnable, TGraphic* g=0, bool bArrow=false);
public:		// User declarations
	__fastcall TProperties		        		(TComponent* Owner);
	static TProperties* CreateForm				(TWinControl* parent=0, TAlign align=alNone, TOnModifiedEvent modif=0, TOnItemFocused focused=0, TOnCloseEvent close=0);
	static TProperties* CreateModalForm			(bool bShowButtonsBar=true, TOnModifiedEvent modif=0, TOnItemFocused focused=0, TOnCloseEvent close=0);
	static void 		DestroyForm				(TProperties*& props);
    int __fastcall 		ShowPropertiesModal		();
    void __fastcall 	ShowProperties			();
    void __fastcall 	HideProperties			();
    void __fastcall 	ClearProperties			();
    bool __fastcall 	IsModified				(){return bModified;}
    void __fastcall 	ResetModified			(){bModified = false;}
    void __fastcall 	RefreshForm				();

    void __fastcall		SelectItem				(const AnsiString& full_name);
    void __fastcall 	AssignItems				(PropItemVec& values, bool full_expand, const AnsiString& title="Properties");
    void __fastcall 	ResetItems				();
    bool __fastcall 	IsFocused				(){return tvProperties->Focused()||seNumber->Focused()||edText->Focused();}
    void __fastcall 	SetModifiedEvent		(TOnModifiedEvent modif=0){OnModifiedEvent=modif;}
    void __fastcall 	BeginFillMode			(const AnsiString& title="Properties", LPCSTR section=0);
    TElTreeItem* __fastcall AddItem				(TElTreeItem* parent, LPCSTR key, LPVOID value, PropValue* prop);
    void __fastcall 	EndFillMode				(bool bFullExpand=true);
    TElTreeItem* __fastcall BeginEditMode		(LPCSTR section=0);
    void __fastcall 	EndEditMode				(TElTreeItem* expand_node=0);
    void __fastcall 	GetColumnWidth			(int& c0, int& c1)
    {
    	c0=tvProperties->HeaderSections->Item[0]->Width;
	    c1=tvProperties->HeaderSections->Item[1]->Width;
	}
    void __fastcall 	SetColumnWidth			(int c0, int c1)
    {
    	tvProperties->HeaderSections->Item[0]->Width=c0;
	    tvProperties->HeaderSections->Item[1]->Width=c1;
	}
    void __fastcall 	SaveParams				(TFormStorage* fs)
    {
		fs->WriteInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvProperties->HeaderSections->Item[0]->Width);
		fs->WriteInteger(AnsiString().sprintf("%s_column1_width",Name.c_str()),tvProperties->HeaderSections->Item[1]->Width);
		fs->WriteInteger(AnsiString().sprintf("%s_draw_thm",Name.c_str()),miDrawThumbnails->Checked);
    }
    void __fastcall 	RestoreParams			(TFormStorage* fs)
    {                                      	
		tvProperties->HeaderSections->Item[0]->Width 	= fs->ReadInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvProperties->HeaderSections->Item[0]->Width);
		tvProperties->HeaderSections->Item[1]->Width 	= fs->ReadInteger(AnsiString().sprintf("%s_column1_width",Name.c_str()),tvProperties->HeaderSections->Item[1]->Width);
        miDrawThumbnails->Checked						= fs->ReadInteger(AnsiString().sprintf("%s_draw_thm",Name.c_str()),false);
        RefreshForm			();
    }

    void 				LockUpdating			(){ tvProperties->IsUpdating = true; }
    void 				UnlockUpdating			(){ tvProperties->IsUpdating = false; }

    // auxiliary routines
	static IC LPVOID	GetItemData				(TElTreeItem* item){return (void*)item->Tag;}
	static IC bool 		IsItemType				(TElTreeItem* item, EPropType type){return item->Tag&&(((PropItem*)item->Tag)->Type()==type);}
	static IC EPropType GetItemType				(TElTreeItem* item){return item->Tag?((PropItem*)item->Tag)->Type():(EPropType)0;}
	static IC LPCSTR	GetItemColumn			(TElTreeItem* item, int col){
    	static AnsiString t;
        if (col<item->ColumnText->Count) t=item->ColumnText->Strings[col];
        return t.c_str();
    }
};
//---------------------------------------------------------------------------
#endif
