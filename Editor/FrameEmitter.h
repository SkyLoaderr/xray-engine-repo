//---------------------------------------------------------------------------


#ifndef FrameEmitterH
#define FrameEmitterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "multi_edit.hpp"
#include <ComCtrls.hpp>
#include "multi_check.hpp"
//---------------------------------------------------------------------------
#include "particlesystem.h"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "ElPgCtl.hpp"
#include "ElXPThemedControl.hpp"
#include <ExtCtrls.hpp>

class TfraEmitter : public TFrame
{
__published:	// IDE-managed Components
	TMxLabel *RxLabel20;
	TMxLabel *RxLabel22;
	TExtBtn *ebBirthFunc;
	TMultiObjSpinEdit *seBirthRate;
	TMultiObjSpinEdit *seParticleLimit;
	TMxLabel *RxLabel1;
	TMultiObjCheck *cbBurst;
	TMxLabel *RxLabel6;
	TMultiObjCheck *cbPlayOnce;
	TElPageControl *pcEmitterType;
	TElTabSheet *tsPoint1;
	TPanel *Panel3;
	TElTabSheet *tsCone;
	TPanel *Panel1;
	TElTabSheet *tsSphere;
	TPanel *Panel5;
	TElTabSheet *tsBox;
	TPanel *Panel4;
	TMxLabel *RxLabel42;
	TMxLabel *RxLabel4;
	TMxLabel *RxLabel35;
	TMxLabel *RxLabel36;
	TMxLabel *RxLabel37;
	TMultiObjSpinEdit *seConeAngle;
	TMultiObjSpinEdit *seConeDirH;
	TMultiObjSpinEdit *seConeDirP;
	TMultiObjSpinEdit *seConeDirB;
	TMxLabel *RxLabel38;
	TMultiObjSpinEdit *seSphereRadius;
	TMxLabel *RxLabel39;
	TMxLabel *RxLabel40;
	TMxLabel *RxLabel41;
	TMultiObjSpinEdit *seBoxSizeX;
	TMultiObjSpinEdit *seBoxSizeY;
	TMultiObjSpinEdit *seBoxSizeZ;
private:	// User declarations
public:		// User declarations
	__fastcall TfraEmitter(TComponent* Owner);
    void GetInfoFirst(const PS::SEmitterDef& E);
    void GetInfoNext(const PS::SEmitterDef& E);
    void SetInfo(PS::SEmitterDef& E);
};
//---------------------------------------------------------------------------
extern PACKAGE TfraEmitter *fraEmitter;
//---------------------------------------------------------------------------
#endif
