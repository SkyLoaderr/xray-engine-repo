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

class TfraEmitter : public TFrame
{
__published:	// IDE-managed Components
	TMxLabel *RxLabel20;
	TMxLabel *RxLabel22;
	TExtBtn *ebBirthFunc;
	TMultiObjSpinEdit *seBirthRate;
	TMultiObjSpinEdit *seParticleLimit;
	TPageControl *pcEmitterType;
	TTabSheet *tsPoint;
	TMxLabel *RxLabel42;
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
	TMxLabel *RxLabel1;
	TMultiObjCheck *cbBurst;
	TMxLabel *RxLabel2;
	TMxLabel *RxLabel3;
	TMxLabel *RxLabel5;
	TMxLabel *RxLabel39;
	TMxLabel *RxLabel40;
	TMxLabel *RxLabel41;
	TMxLabel *RxLabel38;
	TMxLabel *RxLabel4;
	TMxLabel *RxLabel35;
	TMxLabel *RxLabel36;
	TMxLabel *RxLabel37;
	TMxLabel *RxLabel6;
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
