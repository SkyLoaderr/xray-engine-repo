//----------------------------------------------------
// file: EditObject.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "d3dutils.h"
#include "bottombar.h"
#include "PropertiesListHelper.h"

static const u32 bone_sel_color	=0xFFFFFFFF;
static const u32 bone_norm_color=0xFFFFFF00;
static const u32 bone_link_color=0xFF909000;
static const float joint_size=0.025f;

bool testRaySphere(float& dist, const Fvector& start, const Fvector& dir, const Fvector& pos, float radius)
{
	Fvector ray2;
	ray2.sub(pos, start);

    float d = ray2.dotproduct(dir);
    if (d>0){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (radius*radius)) && (d>radius) ){
        	if (d<dist){
	            dist = d;
    	        return true;
            }
        }
    }
    return false;
}
/*
bool testRayBox(float& dist, const Fvector& start, const Fvector& dir, const Fbox& pos, float radius)
{
	Fvector ray2;
	ray2.sub(pos, start);

    float d = ray2.dotproduct(dir);
    if (d>0){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (radius*radius)) && (d>radius) ){
        	if (d<dist){
	            dist = d;
    	        return true;
            }
        }
    }
    return false;
}
*/
void CEditableObject::RenderSkeletonSingle(const Fmatrix& parent)
{
	RenderSingle(parent);
    if (fraBottomBar->miDrawObjectBones->Checked) RenderBones(parent);
}

void CEditableObject::RenderBones(const Fmatrix& parent)
{
	if (IsSkeleton()){
        // render
        RCache.set_xform_world(parent);
        Device.SetShader(Device.m_WireShader);
		BoneVec& lst = m_Bones;
        for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
            Fmatrix& M 	= (*b_it)->LTransform();
            Fvector& p1	= M.c;
            u32 c_joint	= (*b_it)->flags.is(CBone::flSelected)?bone_sel_color:bone_norm_color;
            Fvector p2,d; 	d.set	(0,0,1);
            DU.DrawBone	(p1,d,0,joint_size,c_joint);
            if (0){
	            M.transform_dir	(d);
    	        p2.mad			(p1,d,(*b_it)->Length());
        	    DU.DrawLine		(p1,p2,c_joint);
            }
            if ((*b_it)->ParentIndex()>-1){
				Fvector& p2 = lst[(*b_it)->ParentIndex()]->LTransform().c;
        	    DU.DrawLine	(p1,p2,bone_link_color);
            }
			if (fraBottomBar->miDrawBoneAxis->Checked){ 
            	Fmatrix mat; mat.mul(parent,M);
	          	DU.DrawObjectAxis(mat,0.03f);
            }
			if (fraBottomBar->miDrawBoneNames->Checked){ 
            	u32 c = (*b_it)->flags.is(CBone::flSelected)?0xFFFFFFFF:0xFF000000;
            	u32 s = (*b_it)->flags.is(CBone::flSelected)?0xFF000000:0xFF909090;
            	DU.DrawText(p1,(*b_it)->Name(),c,s);
            }
        }
    }
}

void CEditableObject::SkinSelect(bool flag)
{
    BoneVec& lst	= m_Bones;
    for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
        (*b_it)->flags.set(CBone::flSelected,flag);
    }
}

CBone* CEditableObject::SkinRayPick(const Fvector& S, const Fvector& D, const Fmatrix& inv_parent)
{
    BoneVec& lst 	= m_Bones;
    float dist 		= flt_max;
    CBone* sel	 	= 0;
    Fvector start,dir;
    inv_parent.transform_dir	(dir,D);
    inv_parent.transform_tiny	(start,S);
    for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
        Fmatrix& M 	= (*b_it)->LTransform();
        // test joint
        if (testRaySphere(dist,start,dir,M.c,joint_size))
        	sel 	= *b_it;
        // test link
//		if (testRayBox(dist,start,dir,M.c,joint_size)){
//        	sel 	= *b_it;
//      	bLink	= true;
//		}
    }
    if (sel){
    	sel->flags.set(CBone::flSelected,TRUE);
    	return sel;
    }
    return 0;
}

CBone* CEditableObject::SkinBoxPick(const Fbox& box, const Fmatrix& inv_parent)
{
	return 0;
}

 
