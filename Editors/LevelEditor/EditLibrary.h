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

#include "Library.h"
#include <Dialogs.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "mxPlacemnt.hpp"
#include <Menus.hpp>
#include "PropertiesEObject.h"
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
	TExtBtn *ExtBtn1;
	TExtBtn *ebMakeLOD;
    void __fastcall ebSaveClick(TObject *Sender);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebPropertiesClick(TObject *Sender);
    void __fastcall tvObjectsDblClick(TObject *Sender);
    void __fastcall cbPreviewClick(TObject *Sender);
	void __fastcall ebMakeThmClick(TObject *Sender);
	void __fastcall tvObjectsKeyPress(TObject *Sender, char &Key);
	void __fastcall tvObjectsItemFocused(TObject *Sender);
	void __fastcall pbImagePaint(TObject *Sender);
	void __fastcall ebExportDOClick(TObject *Sender);
	void __fastcall ebImportClick(TObject *Sender);
	void __fastcall ebExportHOMClick(TObject *Sender);
	void __fastcall FormCloseQuery(TObject *Sender, bool &CanClose);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ebMakeLODClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
private:	// User declarations
    void InitObjects();
    EImageThumbnail* 		m_Thm;
	static TfrmEditLibrary*	form;
    CSceneObject* 			m_pEditObject;
	static AnsiString 		m_LastSelection;
    static FileMap 			modif_map;
    static bool 			bFinalExit;
    static bool 			bExitResult;
    TfrmPropertiesEObject*  m_Props;
    void					UpdateObjectProperties();
    void					ChangeReference	(LPCSTR new_name);
public:		// User declarations
    void __fastcall 		OnModified		();
    __fastcall 				TfrmEditLibrary	(TComponent* Owner);
    static bool 			FinalClose		();
    static void __fastcall 	OnRender		();
    static void __fastcall 	ZoomObject		();
	static CSceneObject* __fastcall RayPick	(const Fvector& start, const Fvector& direction, SRayPickInfo* pinf);
    static void __fastcall 	ShowEditor		();
    static void __fastcall 	ResetSelected	();
    static void __fastcall 	RefreshSelected	();
};
//---------------------------------------------------------------------------
#endif
