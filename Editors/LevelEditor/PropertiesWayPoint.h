//---------------------------------------------------------------------------
#ifndef PropertiesWayPointH
#define PropertiesWayPointH
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
#include "ElPgCtl.hpp"
#include "ElXPThemedControl.hpp"

// refs
class CCustomObject;
class CWayPoint;

class TfrmPropertiesWayPoint : public TForm
{
__published:	// IDE-managed Components
    TExtBtn *ebOk;
    TExtBtn *ebCancel;
    TPanel *paBottom;
	TElPageControl *pcProps;
	TElTabSheet *tsBasic;
	TPanel *paBasic;
	TLabel *RxLabel1;
	TEdit *edName;
	TRadioGroup *rgType;
	TElTabSheet *tsWayPoint;
	TPanel *paSurfaces;
	TMultiObjCheck *cb00000001;
	TMultiObjCheck *cb00000002;
	TMultiObjCheck *cb00000004;
	TMultiObjCheck *cb00000008;
	TMultiObjCheck *cb00000010;
	TMultiObjCheck *cb00000020;
	TMultiObjCheck *cb00000040;
	TMultiObjCheck *cb00000080;
	TMultiObjCheck *cb00000100;
	TMultiObjCheck *cb00000200;
	TMultiObjCheck *cb00000400;
	TMultiObjCheck *cb00000800;
	TMultiObjCheck *cb00001000;
	TMultiObjCheck *cb00002000;
	TMultiObjCheck *cb00004000;
	TMultiObjCheck *cb00008000;
    void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
    void __fastcall ebCancelClick(TObject *Sender);
    void __fastcall ebOkClick(TObject *Sender);
    void __fastcall FormShow(TObject *Sender);
    void __fastcall OnModified(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall rgTypeClick(TObject *Sender);
private:	// User declarations
	static TfrmPropertiesWayPoint* form;
    bool bLoadMode;
    bool bPointMode;
    list<CCustomObject*>* 	m_Objects;
    vector<CWayPoint*>		m_Points;
    void GetWayInfo     ();
    bool ApplyWayInfo   ();
    void GetPointInfo     ();
    bool ApplyPointInfo   ();
public:		// User declarations
    __fastcall TfrmPropertiesWayPoint(TComponent* Owner);
    static int __fastcall Run(list<CCustomObject*>* pObjects, bool& bChange);
    static bool __fastcall Visible(){return !!form;}
};
//---------------------------------------------------------------------------
#endif
