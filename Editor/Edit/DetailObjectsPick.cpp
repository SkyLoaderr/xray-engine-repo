//----------------------------------------------------
// file: DetailObjects.h
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "DetailObjects.h"
#include "UI_Main.h"
#include "Scene.h"
#include "cl_intersect.h"
#include "D3DUtils.h"

void CDetailManager::RayPickSelect(float& distance, Fvector& start, Fvector& direction){
	float			fx,fz;
    Fbox			bbox;
    Fvector 		P;
    float 			dist=flt_max;
    int 			sx=-1, sz=-1;


    for (DWORD z=0; z<m_Header.size_z; z++){
        fz			= fromSlotZ(z);
        for (DWORD x=0; x<m_Header.size_x; x++){
			DSIt slot= m_Slots.begin()+z*m_Header.size_x+x;
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
    if ((sx>=0)||(sz>=0)) m_Selected[sz*m_Header.size_x+sx] = true;
    UI->RedrawScene();
}

int CDetailManager::FrustumSelect(bool flag){
	CFrustum frustum;
    if (!UI->SelectionFrustum(frustum)) return 0;

    int count=0;

    float 			fx,fz;
    Fbox			bbox;
    for (DWORD z=0; z<m_Header.size_z; z++){
        fz			= fromSlotZ(z);
        for (DWORD x=0; x<m_Header.size_x; x++){
            DSIt slot	= m_Slots.begin()+z*m_Header.size_x+x;
            fx			= fromSlotX(x);

            bbox.min.set(fx-DETAIL_SLOT_SIZE_2, slot->y_min, fz-DETAIL_SLOT_SIZE_2);
            bbox.max.set(fx+DETAIL_SLOT_SIZE_2, slot->y_max, fz+DETAIL_SLOT_SIZE_2);
			BYTE mask	= 0xff;
            bool bRes 	= !!frustum.testAABB(bbox.min,bbox.max,mask);
            if (bRes==flag){
	            m_Selected[z*m_Header.size_x+x] = flag;
            	count++;
            }
        }
    }
    UI->RedrawScene();
    return count;
}

int CDetailManager::SelectObjects(bool flag){
	for (BOOLIt it=m_Selected.begin(); it!=m_Selected.end(); it++)
    	*it = flag;
    return m_Selected.size();
}

int CDetailManager::InvertSelection(){
	for (BOOLIt it=m_Selected.begin(); it!=m_Selected.end(); it++)
    	*it = !*it;
    return m_Selected.size();
}

int CDetailManager::SelectionCount(bool testflag){
	int count = 0;
	for (BOOLIt it=m_Selected.begin(); it!=m_Selected.end(); it++)
    	if (*it==testflag) count++;
    return count;
}

