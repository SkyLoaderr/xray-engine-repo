//----------------------------------------------------
// file: EditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "motion.h"
#include "bone.h"

bool CEditableObject::BoneRayPick(const Fvector& S, const Fvector& D, const Fmatrix& inv_parent)
{
    BoneVec& lst = m_Bones;
    for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
        Fmatrix& M 	= (*b_it)->LTransform();
        Fvector& p1	= M.c;
        u32 c 		= (*b_it)->flags.is(CBone::flSelected)?bone_sel_color:bone_norm_color;
        DU.DrawRomboid	(p1,0.025,c);
        if ((*b_it)->ParentIndex()>-1){
            Fvector& p2 = lst[(*b_it)->ParentIndex()]->LTransform().c;
            DU.DrawLine	(p1,p2,c);
        }
        if (fraBottomBar->miDrawBoneAxis->Checked) DU.DrawObjectAxis(M,0.03f);
        if (fraBottomBar->miDrawBoneNames->Checked) DU.DrawText(p1,(*b_it)->Name());
    }
}

bool CEditableObject::BoneBoxPick(const Fbox& box, const Fmatrix& inv_parent)
{
}


 