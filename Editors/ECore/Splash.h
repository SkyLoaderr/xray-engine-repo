//---------------------------------------------------------------------------

#ifndef SplashH
#define SplashH
//---------------------------------------------------------------------------
#include "MXCtrls.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TSplashForm : public TForm
{
__published:	// IDE-managed Components
    TImage*		Image1;
	TMxLabel*	lbStatus;
private:	// User declarations
	static TSplashForm*	form;
public:		// User declarations
    __fastcall 			TSplashForm		(TComponent* Owner);

    static void			CreateSplash	(u32 inst, LPCSTR res_splash_name);
    static void			DestroySplash	();

    static void 		SetStatus		(LPSTR log){ if (form&&log){ form->lbStatus->Caption = log; form->lbStatus->Repaint(); };}
};
//---------------------------------------------------------------------------
#endif
