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
        for(b_it=lst.begin(); b_it!=lst.end(); b_it++){
            Fmatrix M 	= (*b_it)->LTransform();
            M.mulB		(parent);
			u32 c 		= (*b_it)->flags.is(CBone::flSelected)?0xffffffff:0xff0000ff;
            switch ((*b_it)->shape.type){
            case SBoneShape::stBox: 	DU.DrawOBB		(M,(*b_it)->shape.box,c);	break;
            case SBoneShape::stSphere:	DU.DrawSphere   (M,(*b_it)->shape.sphere,c);break;
            case SBoneShape::stCylinder:DU.DrawCylinder (M,(*b_it)->shape.cylinder.m_center,(*b_it)->shape.cylinder.m_direction,(*b_it)->shape.cylinder.m_height,(*b_it)->shape.cylinder.m_radius,c);break;
            }
        }
    }
}

CBone* CEditableObject::PickBone(const Fvector& S, const Fvector& D, const Fmatrix& inv_parent)
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
    }
    SelectBone		(sel);
    return sel;
}

void CEditableObject::SelectBone(CBone* b)
{
    BoneVec& lst 	= m_Bones;
    for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
        (*b_it)->flags.set		(CBone::flSelected,FALSE);
    if (b)			b->flags.set(CBone::flSelected,TRUE);
}
 
int CEditableObject::GetSelectedBones(BoneVec& sel_bones)
{
    BoneVec& lst 	= m_Bones;
    for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++)
        if ((*b_it)->flags.is(CBone::flSelected)) sel_bones.push_back(*b_it);
    return sel_bones.size();
}

#include "MgcCont3DMinBox.h"
void ComputeOBB(Fobb &B, FvectorVec& V)
{
    if (V.size()<3) { B.invalidate(); return; }
    Mgc::Box3	BOX		= Mgc::MinBox(V.size(), (const Mgc::Vector3*) V.begin());
    B.m_rotate.i.set	(BOX.Axis(0));
    B.m_rotate.j.set	(BOX.Axis(1));
    B.m_rotate.k.set	(BOX.Axis(2));
    B.m_translate.set	(BOX.Center());
    B.m_halfsize.set	(BOX.Extents()[0],BOX.Extents()[1],BOX.Extents()[2]);
}
//----------------------------------------------------

#include "MgcCont3DMinSphere.h"
BOOL	f_valid		(float f)
{
	return _finite(f) && !_isnan(f);
}
BOOL	SphereValid	(FvectorVec& geom, Fsphere& test)
{
	if (!f_valid(test.P.x) || !f_valid(test.R))	{
		Msg	("*** Attention ***: invalid sphere: %f,%f,%f - %f",test.P.x,test.P.y,test.P.z,test.R);
	}

	Fsphere	S	=	test;
	S.R			+=	EPS_L;
	for (FvectorIt I = geom.begin(); I!=geom.end(); I++)
		if (!S.contains(*I))	return FALSE;
	return TRUE;
}
void ComputeSphere(Fsphere &B, FvectorVec& V)
{
    if (V.size()<3) 	{ B.P.set(0,0,0); B.R=0.f; return; }

	// 1: calc first variation
	Fsphere	S1;
    Fsphere_compute		(S1,V.begin(),V.size());
	BOOL B1				= SphereValid(V,S1);
    
	// 2: calc ordinary algorithm (2nd)
	Fsphere	S2;
	Fbox bbox;
    bbox.invalidate		();
	for (FvectorIt I=V.begin(); I!=V.end(); I++)	bbox.modify(*I);
	bbox.grow			(EPS_L);
	bbox.getsphere		(S2.P,S2.R);
	S2.R = -1;
	for (I=V.begin(); I!=V.end(); I++)	{
		float d = S2.P.distance_to_sqr(*I);
		if (d>S2.R) S2.R=d;
	}
	S2.R = _sqrt (_abs(S2.R));
	BOOL B2				= SphereValid(V,S2);

	// 3: calc magic-fm
	Mgc::Sphere _S3		= Mgc::MinSphere(V.size(), (const Mgc::Vector3*) V.begin());
	Fsphere	S3;
	S3.P.set			(_S3.Center().x,_S3.Center().y,_S3.Center().z);
	S3.R				= _S3.Radius();
	BOOL B3				= SphereValid(V,S3);

	// select best one
	if (B1 && (S1.R<S2.R)){		// miniball or FM
		if (B3 && (S3.R<S1.R)){ // FM wins
        	B.set	(S3);
		}else{					// MiniBall wins
        	B.set	(S1);
		}
	}else{						// base or FM
		if (B3 && (S3.R<S2.R)){	// FM wins
        	B.set	(S3);
		}else{					// Base wins :)
        	R_ASSERT(B2);
        	B.set	(S2);
		}
	}
}
//----------------------------------------------------

#include "MgcCont3DCylinder.h"
void ComputeCylinder(Fcylinder& C, Fobb& B, FvectorVec& V)
{
    if (V.size()<3) 	{ C.invalidate(); return; }
    // pow(area,(3/2))/volume
    // 2*Pi*R*H+2*Pi*R*R
	
	Fvector axis;
    float max_hI   	= flt_min;
    float min_hI   	= flt_max;
    float max_rI   	= flt_min;
    float max_hJ   	= flt_min;
    float min_hJ   	= flt_max;
    float max_rJ   	= flt_min;
    float max_hK   	= flt_min;
    float min_hK   	= flt_max;
    float max_rK   	= flt_min;
    Fvector axisJ 		= B.m_rotate.j;
    Fvector axisI 		= B.m_rotate.i;
    Fvector axisK 		= B.m_rotate.k;
    Fvector& c			= B.m_translate;
    for (FvectorIt I=V.begin(); I!=V.end(); I++){
        Fvector tmp;
    	Fvector pt		= *I;
    	Fvector pt_c;	pt_c.sub(pt,c);

    	float pI		= axisI.dotproduct(pt);
        min_hI			= _min(min_hI,pI);
        max_hI			= _max(max_hI,pI);
        tmp.mad			(c,axisI,axisI.dotproduct(pt_c));
        max_rI			= _max(max_rI,tmp.distance_to(pt));
        
    	float pJ		= axisJ.dotproduct(pt);
        min_hJ			= _min(min_hJ,pJ);
        max_hJ			= _max(max_hJ,pJ);
        tmp.mad			(c,axisJ,axisJ.dotproduct(pt_c));
        max_rJ			= _max(max_rJ,tmp.distance_to(pt));

    	float pK		= axisK.dotproduct(pt);
        min_hK			= _min(min_hK,pK);
        max_hK			= _max(max_hK,pK);
        tmp.mad			(c,axisK,axisK.dotproduct(pt_c));
        max_rK			= _max(max_rK,tmp.distance_to(pt));
    }

    float hI			= (max_hI-min_hI);
    float hJ			= (max_hJ-min_hJ);
    float hK			= (max_hK-min_hK);
    float vI			= hI*M_PI*_sqr(max_rI);
    float vJ			= hJ*M_PI*_sqr(max_rJ);
    float vK			= hK*M_PI*_sqr(max_rK);
//    vI					= pow(2*M_PI*max_rI*hI+2*M_PI*_sqr(max_rI),3/2)/vI;
//    vJ					= pow(2*M_PI*max_rJ*hJ+2*M_PI*_sqr(max_rJ),3/2)/vJ;
//    vK					= pow(2*M_PI*max_rK*hK+2*M_PI*_sqr(max_rK),3/2)/vK;
    // pow(area,(3/2))/volume
    // 2*Pi*R*H+2*Pi*R*R
    
    if (vI<vJ){
    	if (vI<vK){
        	//vI;
            C.m_direction.set	(axisI);
            C.m_height			= hI;
            C.m_radius			= max_rI;
        }else{
        	// vK
            C.m_direction.set	(axisK);
            C.m_height			= hK;
            C.m_radius			= max_rK;
        }
    }else {
        //vJ < vI
     	if (vJ<vK){
        	// vJ
            C.m_direction.set	(axisJ);
            C.m_height			= hJ;
            C.m_radius			= max_rJ;
        } else {
            //vK
            C.m_direction.set	(axisK);
            C.m_height			= hK;
            C.m_radius			= max_rK;
        }
    }
    
    C.m_center.set		(B.m_translate);
//    C.m_direction.set	(B.m_rotate.k);
//    C.m_height			= B.m_halfsize.z*2.f;
//    C.m_radius			= _max(B.m_halfsize.x,B.m_halfsize.y);
//	B.m_rotate.k
/*
    if (V.size()<3) { B.invalidate(); return; }
    Mgc::Cylinder CYL	= Mgc::ContCylinder(V.size(), (const Mgc::Vector3*) V.begin());
    B.m_center.set		(CYL.Center());
    B.m_direction.set	(CYL.Direction());
    B.m_height			= CYL.Height();
    B.m_radius			= CYL.Radius();
*/
}


bool CEditableObject::GenerateBoneShapes(bool bSelOnly)
{
	R_ASSERT(IsSkeleton());
    xr_vector<FvectorVec>	bone_points;
	bone_points.resize		(m_Bones.size());
    for(EditMeshIt mesh_it=FirstMesh();mesh_it!=LastMesh();mesh_it++){
        CEditableMesh* MESH = *mesh_it;
        // generate vertex offset
        if (!MESH->m_LoadState.is(CEditableMesh::LS_SVERTICES)) MESH->GenerateSVertices();
		FaceVec& _faces		= MESH->GetFaces();
        for (FaceIt f_it=_faces.begin(); f_it!=_faces.end(); f_it++){
            st_Face& face 	= *f_it;
            for (int k=0; k<3; k++){
                st_FaceVert& 	fv = face.pv[k];
                st_SVert& 		sv = MESH->m_SVertices[fv.pindex];
		        bone_points[sv.bone0].push_back(sv.offs0);
            }
        }
    }

    BoneVec& lst 	= m_Bones;    
    for(BoneIt b_it=lst.begin(); b_it!=lst.end(); b_it++){
    	if (bSelOnly&&!(*b_it)->flags.is(CBone::flSelected)) continue;
        ComputeOBB		((*b_it)->shape.box,bone_points[b_it-lst.begin()]);
        ComputeSphere	((*b_it)->shape.sphere,bone_points[b_it-lst.begin()]);
        ComputeCylinder	((*b_it)->shape.cylinder,(*b_it)->shape.box,bone_points[b_it-lst.begin()]);
    }
    return true;
}
 
