//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "DetailObjects.h"
#include "Scene.h"
#include "cl_intersect.h"
#include "bottombar.h"
#include "ui_main.h"
#include "frustum.h"

int EDetailManager::RaySelect(bool flag, float& distance, Fvector& start, Fvector& direction)
{
// box selected only

	if (!fraBottomBar->miDrawDOSlotBoxes->Checked) return 0;

	float			fx,fz;
    Fbox			bbox;
    Fvector 		P;
    float 			dist=flt_max;
    int 			sx=-1, sz=-1;

    int count = 0;

    for (DWORD z=0; z<dtH.size_z; z++){
        fz			= fromSlotZ(z);
        for (DWORD x=0; x<dtH.size_x; x++){
			DetailSlot* slot= dtSlots+z*dtH.size_x+x;
            fx		= fromSlotX(x);
            bbox.min.set(fx-DETAIL_SLOT_SIZE_2, slot->y_min, fz-DETAIL_SLOT_SIZE_2);
            bbox.max.set(fx+DETAIL_SLOT_SIZE_2, slot->y_max, fz+DETAIL_SLOT_SIZE_2);
            if (bbox.Pick2(start,direction,P)){
            	float d = start.distance_to_sqr(P);
                if (d<dist){
                	dist = d;
                    sx=x; sz=z;
                }
            }
        }
    }
    if ((sx>=0)||(sz>=0)){
    	m_Selected[sz*dtH.size_x+sx] = flag;
        count++;
    }
    UI.RedrawScene();
    return count;
}

int EDetailManager::FrustumSelect(bool flag)
{
// box selected only

	if (!fraBottomBar->miDrawDOSlotBoxes->Checked) return 0;
	CFrustum frustum;
    if (!UI.SelectionFrustum(frustum)) return 0;

    int count=0;

    float 			fx,fz;
    Fbox			bbox;
    for (DWORD z=0; z<dtH.size_z; z++){
        fz			= fromSlotZ(z);
        for (DWORD x=0; x<dtH.size_x; x++){
            DetailSlot* slot = dtSlots+z*dtH.size_x+x;
            fx			= fromSlotX(x);

            bbox.min.set(fx-DETAIL_SLOT_SIZE_2, slot->y_min, fz-DETAIL_SLOT_SIZE_2);
            bbox.max.set(fx+DETAIL_SLOT_SIZE_2, slot->y_max, fz+DETAIL_SLOT_SIZE_2);
			BYTE mask	= 0xff;
            bool bRes 	= !!frustum.testAABB(bbox.min,bbox.max,mask);
            if (bRes==flag){
	            m_Selected[z*dtH.size_x+x] = flag;
            	count++;
            }
        }
    }
    UI.RedrawScene();
    return count;
}

int EDetailManager::SelectObjects(bool flag){
//	for (int i=0; i<m_Selected.size(); i++)
//    	m_Selected[i] = flag;
	for (BYTEIt it=m_Selected.begin(); it!=m_Selected.end(); it++)
    	*it = flag;
    return m_Selected.size();
}

int EDetailManager::InvertSelection(){
	if (!fraBottomBar->miDrawDOSlotBoxes->Checked) return 0;
//	for (int i=0; i<m_Selected.size(); i++)
//    	m_Selected[i] = m_Selected[i];
	for (BYTEIt it=m_Selected.begin(); it!=m_Selected.end(); it++)
    	*it = !*it;
    return m_Selected.size();
}

int EDetailManager::SelectionCount(bool testflag){
	if (!fraBottomBar->miDrawDOSlotBoxes->Checked) return 0;
	int count = 0;
//	for (int i=0; i<m_Selected.size(); i++)
//    	if (m_Selected[i]==testflag) count++;
	for (BYTEIt it=m_Selected.begin(); it!=m_Selected.end(); it++)
    	if (*it==testflag) count++;
    return count;
}

