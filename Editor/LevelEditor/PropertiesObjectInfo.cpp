#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Texture.h"
#include "ui_main.h"
#include "scene.h"
#include "main.h"
#include "motion.h"
//---------------------------------------------------------------------------
// info
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsInfoShow(TObject *Sender)
{
	if (!m_LibObject) return;
	bLoadMode = true;
// Set up info
    paBB->Visible = true;

    const Fvector& P=m_LibObject->t_vPosition;
    sePositionX->Value = P.x;
    sePositionY->Value = P.y;
    sePositionZ->Value = P.z;
    const Fvector& R=m_LibObject->t_vRotate;
    seRotateX->Value = rad2deg(R.x);
    seRotateY->Value = rad2deg(R.y);
    seRotateZ->Value = rad2deg(R.z);
    const Fvector& S=m_LibObject->t_vScale;
    seScaleX->Value = S.x;
    seScaleY->Value = S.y;
    seScaleZ->Value = S.z;

	seScaleX->Enabled = m_LibObject->IsDynamic();
	seScaleY->Enabled = m_LibObject->IsDynamic();
	seScaleZ->Enabled = m_LibObject->IsDynamic();


    Fbox& BB = m_LibObject->GetBox();
    for (int col=1; col<5; col++){
        Fvector p;
        switch(col){
        case 1: p.set(BB.min);break;
        case 2: p.set(BB.max);break;
        case 3: BB.getsize(p);break;
        case 4: BB.getcenter(p);break;
        }
        for (int row=0; row<3; row++){
            AnsiString n; n.sprintf("%.3f", p[row]);
            sgBB->Cells[col][row] = n;
        }
    }
	bLoadMode = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::seTransformChange(TObject *Sender)
{
	OnModified(Sender);
}
//---------------------------------------------------------------------------

