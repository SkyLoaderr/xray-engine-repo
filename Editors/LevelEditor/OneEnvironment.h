//---------------------------------------------------------------------------

#ifndef OneEnvironmentH
#define OneEnvironmentH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>
#include "CSPIN.h"

#include "multi_color.hpp"
#include "multi_edit.hpp"
#include "MXCtrls.hpp"

//refs
struct xr_token;
struct st_Environment;

class TfrmOneEnvironment : public TForm
{
__published:	// IDE-managed Components
	TGroupBox *gbEnv;
	TLabel *RxLabel29;
	TLabel *RxLabel30;
	TLabel *RxLabel31;
	TMultiObjColor *mcAmbColor;
	TLabel *RxLabel32;
	TMultiObjColor *mcFogColor;
	TMultiObjSpinEdit *seViewDistance;
	TMultiObjSpinEdit *seFogness;
	TLabel *RxLabel1;
	TMultiObjColor *mcSkyColor;
	TMultiObjSpinEdit *seSkyFogness;
	void __fastcall UpdateClick(TObject *Sender);
	void __fastcall mcAmbColorMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
	void __fastcall seFognessLWChange(TObject *Sender, int Val);
	void __fastcall seFognessKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
public:
	st_Environment* m_CurEnv;
    bool bLoadMode;
    void UpdateEnvData();
public:		// User declarations
    __fastcall TfrmOneEnvironment(TComponent* Owner);
    void ShowEnv(st_Environment* env);
    void HideEnv();
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmOneEnvironment *frmOneEnvironment;
//---------------------------------------------------------------------------
#endif
