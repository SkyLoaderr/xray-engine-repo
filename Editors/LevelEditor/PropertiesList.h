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

#include "PropertiesListTypes.h"


class TProperties : public TForm
{
__published:	// IDE-managed Components
	TElTree 			*tvProperties;
	TMxPopupMenu 		*pmEnum;
	TFormStorage 		*fsStorage;
	TMultiObjSpinEdit 	*seNumber;
	TEdit 				*edText;
	TBevel 				*Bevel1;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tvPropertiesClick(TObject *Sender);
	void __fastcall tvPropertiesItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall tvPropertiesMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall seNumberExit(TObject *Sender);
	void __fastcall seNumberKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall edTextExit(TObject *Sender);
	void __fastcall edTextKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall tvPropertiesItemFocused(TObject *Sender);
	void __fastcall edTextDblClick(TObject *Sender);
	void __fastcall tvPropertiesHeaderColumnResize(TObject *Sender,
          int SectionIndex);
	void __fastcall FormDeactivate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall tvPropertiesItemChange(TObject *Sender, TElTreeItem *Item,
          TItemChangeMode ItemChangeMode);
private:	// User declarations
    void __fastcall PMItemClick		(TObject *Sender);
	void __fastcall WaveFormClick	(TElTreeItem* item);
	void __fastcall ColorClick		(TElTreeItem* item);
	void __fastcall VectorClick		(TElTreeItem* item);
	void __fastcall CustomTextClick(TElTreeItem* item);
	void __fastcall CustomAnsiTextClick(TElTreeItem* item);

	Graphics::TBitmap* m_BMCheck;
	Graphics::TBitmap* m_BMDot;
	Graphics::TBitmap* m_BMEllipsis;
    bool bModified;
    AnsiString last_selected_item;
	// LW style inpl editor
    void HideLWNumber();
    void PrepareLWNumber(TElTreeItem* node);
    void ShowLWNumber(TRect& R);
    void ApplyLWNumber();
    void CancelLWNumber();
	// text inplace editor
    void HideLWText();
    void PrepareLWText(TElTreeItem* node);
    void ShowLWText(TRect& R);
    void ApplyLWText();
    void CancelLWText();

    PropValueVec 		m_Values;
    TOnModifiedEvent 	OnModifiedEvent;
    TOnItemFocused      OnItemFocused;
    void 				Modified		(){bModified=true; if (OnModifiedEvent) OnModifiedEvent();}
    void 				ClearParams(TElTreeItem* node=0);
    void 				ApplyEditControl();
    void 				CancelEditControl();

public:		// User declarations
	__fastcall TProperties		        	(TComponent* Owner);
	static TProperties* CreateForm			(TWinControl* parent=0, TAlign align=alNone, TOnModifiedEvent modif=0, TOnItemFocused focused=0);
	static void 	DestroyForm				(TProperties*& props);
    void __fastcall ShowPropertiesModal		();
    void __fastcall ShowProperties			();
    void __fastcall HideProperties			();                                   
    void __fastcall ClearProperties			();
    bool __fastcall IsModified				(){return bModified;}
    void __fastcall ResetModified			(){bModified = false;}
    void __fastcall RefreshForm				(){tvProperties->Repaint();}

    void __fastcall AssignValues			(PropValueVec& values, bool full_expand, const AnsiString& title="Properties"); 
    void __fastcall ResetValues				();
    bool __fastcall IsFocused				(){return tvProperties->Focused()||seNumber->Focused()||edText->Focused();}
    void __fastcall SetModifiedEvent		(TOnModifiedEvent modif=0){OnModifiedEvent=modif;}
    void __fastcall BeginFillMode			(const AnsiString& title="Properties", LPCSTR section=0);
    TElTreeItem* __fastcall AddItem			(TElTreeItem* parent, LPCSTR key, LPVOID value, PropValue* prop); 
    void __fastcall EndFillMode				(bool bFullExpand=true);
    TElTreeItem* __fastcall BeginEditMode	(LPCSTR section=0);
    void __fastcall EndEditMode				(TElTreeItem* expand_node=0);
    void __fastcall GetColumnWidth			(int& c0, int& c1)
    {
    	c0=tvProperties->HeaderSections->Item[0]->Width;
	    c1=tvProperties->HeaderSections->Item[1]->Width;
	}
    void __fastcall SetColumnWidth			(int c0, int c1)
    {
    	tvProperties->HeaderSections->Item[0]->Width=c0;
	    tvProperties->HeaderSections->Item[1]->Width=c1;
	}
    void __fastcall SaveColumnWidth			(TFormStorage* fs)
    {
		fs->WriteInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvProperties->HeaderSections->Item[0]->Width);
		fs->WriteInteger(AnsiString().sprintf("%s_column1_width",Name.c_str()),tvProperties->HeaderSections->Item[1]->Width);
    }
    void __fastcall RestoreColumnWidth		(TFormStorage* fs)
    {
		tvProperties->HeaderSections->Item[0]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvProperties->HeaderSections->Item[0]->Width);
		tvProperties->HeaderSections->Item[1]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column1_width",Name.c_str()),tvProperties->HeaderSections->Item[1]->Width);
    }

    void 			IsUpdating				(bool bVal)
    {
    	tvProperties->IsUpdating = bVal;
    }
    // auxiliary routines
	static IC LPVOID		GetItemData		(TElTreeItem* item){return (void*)item->Tag;}
	static IC bool 			IsItemType		(TElTreeItem* item, EPropType type){return item->Tag&&(((PropValue*)item->Tag)->type==type);}
	static IC EPropType 	GetItemType		(TElTreeItem* item){return item->Tag?((PropValue*)item->Tag)->type:(EPropType)0;}
	static IC LPCSTR	 	GetItemColumn	(TElTreeItem* item, int col){
    	static AnsiString t;
        if (col<item->ColumnText->Count) t=item->ColumnText->Strings[col];
        return t.c_str();
    }
};
//---------------------------------------------------------------------------
#endif
