#include "stdafx.h"
#pragma hdrstop

#include "StatGraph.h"
//---------------------------------------------

CStatGraph::CStatGraph()
{
	Device.seqRender.Add		(this,REG_PRIORITY_LOW-1000);
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
	hGeom.create				(FVF::F_TL0uv,RCache.Vertex.Buffer(),RCache.Index.Buffer());
	mn					= 0;
	mx					= 0;
	max_item_count		= 0;
	lt.set				(0,0);
	rb.set				(0,0);
	grid.set			(0,0);
	grid_color			= 0xFF000000;
	rect_color			= 0xFF000000;
}

CStatGraph::~CStatGraph()
{
	Device.seqRender.Remove		(this);
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	hGeom.destroy				();
}

void CStatGraph::OnDeviceCreate()
{
	hGeom.create				(FVF::F_TL0uv,RCache.Vertex.Buffer(),RCache.Index.Buffer());
}

void CStatGraph::OnDeviceDestroy()
{
	hGeom.destroy				();
}

void CStatGraph::OnRender()
{
	RCache.OnFrameEnd();
	u32			dwOffset,dwCount;
    FVF::TL0uv* pv_start	= (FVF::TL0uv*)RCache.Vertex.Lock(elements.size()*2+8,hGeom->vb_stride,dwOffset);
    FVF::TL0uv* pv			= pv_start;
    // base rect
    pv->set					(lt.x-1,lt.y-1,rect_color); pv++; 	// 0
    pv->set					(rb.x+1,lt.y-1,rect_color); pv++;	// 0
    pv->set					(rb.x+1,lt.y-1,rect_color); pv++;	// 1
    pv->set					(rb.x+1,rb.y+1,rect_color); pv++;   // 1
    pv->set					(rb.x+1,rb.y+1,rect_color); pv++;   // 2
    pv->set					(lt.x-1,rb.y+1,rect_color); pv++;   // 2
    pv->set					(lt.x-1,rb.y+1,rect_color); pv++;   // 3
    pv->set					(lt.x-1,lt.y-1,rect_color); pv++;	// 3
    // grid
    float grid_offs_x		= float(rb.x-lt.x)/float(grid.x+1);
    float grid_offs_y		= float(rb.y-lt.y)/float(grid.y+1);
    for (int g_x=1; g_x<=grid.x; g_x++){
	    pv->set				(iFloor(g_x*grid_offs_x+lt.x),lt.y,grid_color); pv++; 	
	    pv->set				(iFloor(g_x*grid_offs_x+lt.x),rb.y,grid_color); pv++; 	
	}    	
    for (int g_y=1; g_y<=grid.y; g_y++){
	    pv->set				(lt.x,iFloor(g_y*grid_offs_y+lt.y),grid_color); pv++; 	
	    pv->set				(rb.x,iFloor(g_y*grid_offs_y+lt.y),grid_color); pv++; 	
	}    	
	// data
    if (!elements.empty()){
        float elem_offs		= float(rb.x-lt.x)/max_item_count;
        float elem_factor	= float(rb.y-lt.y)/float(mx-mn);
		float base_y		= float(rb.y)+(mn*elem_factor);
        switch (style){
        case stBar:{
            for (ElementsDeqIt it=elements.begin(); it!=elements.end(); it++){
                float X		= float(it-elements.begin())*elem_offs+lt.x;
                float Y0	= base_y;
                float Y1	= base_y - it->data*elem_factor;
                pv->set		(X,Y0,it->color); pv++;
                pv->set		(X,Y1,it->color); pv++;
            }
        }break;
        case stCurve:{
            for (ElementsDeqIt it=elements.begin()+1; it!=elements.end(); it++){
            	ElementsDeqIt it_prev = it-1;
                float X0	= float(it_prev-elements.begin())*elem_offs+lt.x;
                float Y0	= base_y-it_prev->data*elem_factor;
                pv->set		(X0,Y0,it->color); pv++;
                float X1	= float(it-elements.begin())*elem_offs+lt.x;
                float Y1	= base_y-it->data*elem_factor;
                pv->set		(X1,Y1,it->color); pv++;
            }
        }break;
        }
    }

    dwCount 				= u32(pv-pv_start);
    RCache.Vertex.Unlock	(dwCount,hGeom->vb_stride);
    
    RCache.set_Geometry		(hGeom);
    RCache.Render	   		(D3DPT_LINELIST,dwOffset,dwCount/2);
}

