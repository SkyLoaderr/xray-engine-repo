#include "stdafx.h"
#include "xr_area.h"
#include "xr_collide_form.h"
#include "xr_object.h"

/*
BOOL CObjectSpace::BBoxCollide(CCFModel *object, const Fmatrix& M_bbox ){
	VERIFY				(object);	
	VERIFY				(object->owner);

	collide_list.clear	( );
	q_tris.clear		( );

	/*
	if ( GetNearest(object, 0) ){
		CCFModel*		target;

		for ( NL_IT nl_item=nearest_list.begin(); nl_item!=nearest_list.end(); nl_item++ ){
			target			= *nl_item;

			if (!object->owner->OnNear(target))
			{
				nearest_list.erase(nl_item);
				nl_item--;
			}
		}
		
		// set raypick mode
		XRC.BBoxMode(BBOX_TRITEST);
		// test models
		for (nl_item=nearest_list.begin(); nl_item!=nearest_list.end(); nl_item++){
			target			= *nl_item;

			Fmatrix &M_target= target->owner?target->owner->mTransform:precalc_identity;
			XRC.BBoxCollide( M_target, &target->model, M_bbox, object->s_box );

			if ( !XRC.GetBBoxContactCount() ) continue;

			if (!object->owner->OnCollide(target)) {
				nearest_list.erase(nl_item);
				nl_item--;
				continue;
			}
			
			collide_list.push_back(target);

			Fmatrix *pM = (target->owner)?&(target->owner->mTransform):0;
			for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
				AddToTrisList( pM, &target->model.tris[XRC.BBoxContact[i].id] );
		}// nearest cycle
	}
	//XRC.BBoxMode(BBOX_TRITEST);
	//XRC.BBoxCollide( precalc_identity, &Static, M_bbox, object->s_box );
	//if ( !XRC.GetBBoxContactCount() ) return 0;
	//collide_list.push_back(0);
	//for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
	//	AddToTrisList( &Static.tris[XRC.BBoxContact[i].id] );
	//return q_tris.size();
}
*/

BOOL CObjectSpace::BBoxCollide( const Fbox& s_box, const Fmatrix& M_bbox ){
	collide_list.clear	( );
	q_tris.clear		( );

	/*
	if ( GetNearest(M_bbox.c, s_box.getradius()) ){
		CCFModel*		target;

	// set raypick mode
		XRC.BBoxMode(BBOX_TRITEST);//|(bFirstContact?BBOX_ONLYFIRST:0)|(bCulling?BBOX_CULL:0));
//		XRC.BBoxMode(BBOX_TRITEST|BBOX_CULL);
	// test models
		for (NL_IT nl_item=nearest_list.begin(); nl_item!=nearest_list.end(); nl_item++){
			target			= *nl_item;

			Fmatrix &M_target= target->owner?target->owner->mTransform:precalc_identity;
			XRC.BBoxCollide( M_target, &target->model, M_bbox, s_box );

			if ( !XRC.GetBBoxContactCount() ) continue;

			collide_list.push_back(target);

			Fmatrix *pM = (target->owner)?&(target->owner->mTransform):0;
			for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
				AddToTrisList( pM, &target->model.tris[XRC.BBoxContact[i].id] );
			
		}// nearest cycle
	}
	*/

	XRC.BBoxMode	(BBOX_TRITEST);
	XRC.BBoxCollide	(precalc_identity, &Static, M_bbox, s_box );

	if ( !XRC.GetBBoxContactCount() ) return 0;
	collide_list.push_back(0);

	for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
		AddToTrisList( &Static.tris[XRC.BBoxContact[i].id] );
	return q_tris.size();
}

BOOL CObjectSpace::BBoxCollide( const Fvector &start, const Fvector &dir, float radius, float range, BOOL bCulling, BOOL bTriTest){
	Fvector D,N,R;
	Fmatrix M_bbox;
	D.normalize			(dir);
// select normal
	if(1-fabsf(D.y)<EPS)N.set(1,0,0);
	else            	N.set(0,1,0);
    R.crossproduct		(N,D);	R.normalize();
    N.crossproduct		(D,R);	N.normalize();
	M_bbox.set			(R,N,D,start);

	// set bbox params
	Fvector box_radius, box_center;
	box_radius.set		(radius,radius,range/2);
	box_center.set		(0,0,box_radius.z);
	
	collide_list.clear	( );
	q_tris.clear		( );

	/*
	if ( GetNearest(M_bbox.c, (radius>range/2)?radius:range/2) ){
		CCFModel*		target;
		XRC.BBoxMode((bTriTest?BBOX_TRITEST:0)|(bCulling?BBOX_CULL:0));
	// test models
		for (NL_IT nl_item=nearest_list.begin(); nl_item!=nearest_list.end(); nl_item++){
			target			= *nl_item;

			Fmatrix &M_target= target->owner?target->owner->mTransform:precalc_identity;
			XRC.BBoxCollide( M_target, &target->model, M_bbox, box_center, box_radius );

			if ( !XRC.GetBBoxContactCount() ) continue;

			collide_list.push_back(target);

			Fmatrix *pM = (target->owner)?&(target->owner->mTransform):0;
			for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
				AddToTrisList( pM, &target->model.tris[XRC.BBoxContact[i].id] );
			
		}// nearest cycle
	}
	*/
	XRC.BBoxMode((bTriTest?BBOX_TRITEST:0)|(bCulling?BBOX_CULL:0));
	XRC.BBoxCollide(precalc_identity, &Static, M_bbox, box_center, box_radius );

	if ( !XRC.GetBBoxContactCount() ) return 0;
	collide_list.push_back(0);

	for (DWORD i=0; i<XRC.GetBBoxContactCount(); i++)
			AddToTrisList(&Static.tris[XRC.BBoxContact[i].id] );
	return q_tris.size();
}
