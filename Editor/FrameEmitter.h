//---------------------------------------------------------------------------


#ifndef FrameEmitterH
#define FrameEmitterH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "CloseBtn.hpp"
#include "multi_edit.hpp"
#include "RXCtrls.hpp"
#include <ComCtrls.hpp>
#include "multi_check.hpp"
//---------------------------------------------------------------------------
#include "particlesystem.h"

class TfraEmitter : public TFrame
{
__published:	// IDE-managed Components
	TRxLabel *RxLabel20;
	TRxLabel *RxLabel22;
	TExtBtn *ebBirthFunc;
	TMultiObjSpinEdit *seBirthRate;
	TMultiObjSpinEdit *seParticleLimit;
	TPageControl *pcEmitterType;
	TTabSheet *tsPoint;
	TRxLabel *RxLabel42;
	TTabSheet *tsCone;
	TMultiObjSpinEdit *seConeAngle;
	TMultiObjSpinEdit *seConeDirH;
	TMultiObjSpinEdit *seConeDirP;
	TMultiObjSpinEdit *seConeDirB;
	TTabSheet *tsSphere;
	TMultiObjSpinEdit *seSphereRadius;
	TTabSheet *tsBox;
	TMultiObjSpinEdit *seBoxSizeX;
	TMultiObjSpinEdit *seBoxSizeY;
	TMultiObjSpinEdit *seBoxSizeZ;
	TRxLabel *RxLabel1;
	TMultiObjCheck *cbBurst;
	TRxLabel *RxLabel2;
	TRxLabel *RxLabel3;
	TRxLabel *RxLabel5;
	TRxLabel *RxLabel39;
	TRxLabel *RxLabel40;
	TRxLabel *RxLabel41;
	TRxLabel *RxLabel38;
	TRxLabel *RxLabel4;
	TRxLabel *RxLabel35;
	TRxLabel *RxLabel36;
	TRxLabel *RxLabel37;
	TRxLabel *RxLabel6;
	TMultiObjCheck *cbPlayOnce;
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
