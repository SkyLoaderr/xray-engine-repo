//---------------------------------------------------------------------------

#ifndef EditLibraryH
#define EditLibraryH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ImgList.hpp>

#include "CustomObject.h"
#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include <Menus.hpp>
//---------------------------------------------------------------------------
// refs
class TObjectPreview;
class EImageThumbnail;
//---------------------------------------------------------------------------

class TfrmEditLibrary : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel1;
    TExtBtn *ebCancel;
    TElTree *tvObjects;
	TExtBtn *ebMakeThm;
	TFormStorage *fsStorage;
	TExtBtn *ebProperties;
	TCheckBox *cbPreview;
	TPanel *paImage;
	TPaintBox *pbImage;
	TExtBtn *ebSave;
	TExtBtn *ebExportDO;
	TExtBtn *ebImport;
	TExtBtn *ebExportHOM;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebPropertiesClick(TObject *Sender);
    void __fastcall tvObjectsDblClick(TObject *Sender);
//    void __fastcall ebEditObjectClick(TObject *Sender);
//    void __fastcall ebApplyEditClick(TObject *Sender);
//    void __fastcall ebCancelEditClick(TObject *Sender);
    void __fastcall cbPreviewClick(TObject *Sender);
	void __fastcall ebMakeThmClick(TObject *Sender);
	void __fastcall tvObjectsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvObjectsItemFocused(TObject *Sender);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall ebExportDOClick(TObject *Sender);
	void __fastcall ebImportClick(TObject *Sender);
	void __fastcall ebExportHOMClick(TObject *Sender);
private:	// User declarations
    void InitObjects();
    EImageThumbnail* m_Thm;
    bool CloseEditLibrary(bool bReload);
	static TfrmEditLibrary *form;
    CEditableObject* m_SelectedObject;
	static AnsiString m_LastSelection;
    static FileMap modif_map;
public:		// User declarations
    __fastcall TfrmEditLibrary(TComponent* Owner);
    static bool FinalClose();
    static void __fastcall OnRender();
    static void __fastcall ZoomObject();
    static void __fastcall OnModified();
	static CEditableObject* __fastcall RayPick(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf);
    static void __fastcall ShowEditor();
    static void __fastcall HideEditor();
    static void __fastcall ResetSelected();
    static void __fastcall RefreshSelected();
};
//---------------------------------------------------------------------------
#endif
