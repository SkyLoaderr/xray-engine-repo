#include "stdafx.h"
#pragma hdrstop

#include "StatGraph.h"
//---------------------------------------------

CStatGraph::CStatGraph()
{
	Device.seqRender.Add		(this,REG_PRIORITY_LOW-1000);
	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
	OnDeviceCreate();
	mn					= 0;
	mx					= 0;
	max_item_count		= 0;
	lt.set				(0,0);
	rb.set				(0,0);
	grid.set			(0,0);
	grid_color			= 0xFF000000;
	rect_color			= 0xFF000000;

	AppendSubGraph(stCurve);
}

CStatGraph::~CStatGraph()
{
	Device.seqRender.Remove		(this);
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	OnDeviceDestroy();
}

void CStatGraph::OnDeviceCreate()
{
	hGeomLine.create			(FVF::F_TL0uv,RCache.Vertex.Buffer(),RCache.Index.Buffer());
	hGeomTri.create				(FVF::F_TL0uv,RCache.Vertex.Buffer(),RCache.QuadIB);
}

void CStatGraph::OnDeviceDestroy()
{
	hGeomLine.destroy				();
	hGeomTri.destroy				();
}

void CStatGraph::RenderBack	()
{
	// draw back
	u32			dwOffset,dwCount;
	FVF::TL0uv* pv_start				= (FVF::TL0uv*)RCache.Vertex.Lock(4,hGeomTri->vb_stride,dwOffset);
	FVF::TL0uv* pv						= pv_start;
	// base rect
	pv->set					(lt.x,rb.y,back_color); pv++;	// 0
	pv->set					(lt.x,lt.y,back_color); pv++; 	// 1
	pv->set					(rb.x,rb.y,back_color); pv++;	// 2
	pv->set					(rb.x,lt.y,back_color); pv++;	// 3
	// render	
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hGeomTri->vb_stride);
	RCache.set_Geometry		(hGeomTri);
	RCache.Render	   		(D3DPT_TRIANGLELIST,dwOffset,0, dwCount, 0, dwCount/2);	

	//draw rect
	pv_start				= (FVF::TL0uv*)RCache.Vertex.Lock(5,hGeomLine->vb_stride,dwOffset);
	pv						= pv_start;
	// base rect
	pv->set					(lt.x,lt.y,rect_color); pv++;	// 0
	pv->set					(rb.x-1,lt.y,rect_color); pv++; 	// 1
	pv->set					(rb.x-1,rb.y,rect_color); pv++;	// 2
	pv->set					(lt.x,rb.y,rect_color); pv++;	// 3
	pv->set					(lt.x,lt.y,rect_color); pv++;	// 0
	// render	
	dwCount 				= u32(pv-pv_start);
    RCache.Vertex.Unlock	(dwCount,hGeomLine->vb_stride);
    RCache.set_Geometry		(hGeomLine);
    RCache.Render	   		(D3DPT_LINESTRIP,dwOffset,4);
/*
		// draw lines
    pv_start	= (FVF::TL0uv*)RCache.Vertex.Lock(elements.size()*2+8,hGeom->vb_stride,dwOffset);
    pv			= pv_start;
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
	*/
};

void CStatGraph::RenderBars(FVF::TL0uv** ppv, ElementsDeq* pelements)
{
	float elem_offs		= float(rb.x-lt.x)/max_item_count;
	float elem_factor	= float(rb.y-lt.y)/float(mx-mn);
	float base_y		= float(rb.y)+(mn*elem_factor);

	float column_width = elem_offs;
	if (column_width > 1) column_width--;
	for (ElementsDeqIt it=pelements->begin(); it!=pelements->end(); it++)
	{
		float X		= float(it-pelements->begin())*elem_offs+lt.x;
		float Y0	= base_y;
		float Y1	= base_y - it->data*elem_factor;

		if (Y1 > Y0)
		{
			(*ppv)->set		(X,Y1,it->color); (*ppv)++;
			(*ppv)->set		(X,Y0,it->color); (*ppv)++;
			(*ppv)->set		(X+column_width,Y1,it->color); (*ppv)++;
			(*ppv)->set		(X+column_width,Y0,it->color); (*ppv)++;
		}
		else
		{
			(*ppv)->set		(X,Y0,it->color); (*ppv)++;
			(*ppv)->set		(X,Y1,it->color); (*ppv)++;
			(*ppv)->set		(X+column_width,Y0,it->color); (*ppv)++;
			(*ppv)->set		(X+column_width,Y1,it->color); (*ppv)++;
		};
	};
	// render
/*
	dwCount 				= u32(pv-pv_start);
	RCache.Vertex.Unlock	(dwCount,hGeomTri->vb_stride);
	RCache.set_Geometry		(hGeomTri);
	RCache.Render	   		(D3DPT_TRIANGLELIST,dwOffset,0, dwCount, 0, dwCount/2);
*/

	
};

void CStatGraph::RenderLines( FVF::TL0uv** ppv, ElementsDeq* pelements )
{
	float elem_offs		= float(rb.x-lt.x)/max_item_count;
	float elem_factor	= float(rb.y-lt.y)/float(mx-mn);
	float base_y		= float(rb.y)+(mn*elem_factor);

	for (ElementsDeqIt it=pelements->begin()+1; it!=pelements->end(); it++)
	{
		ElementsDeqIt it_prev = it-1;
		float X0	= float(it_prev-pelements->begin())*elem_offs+lt.x;
		float Y0	= base_y-it_prev->data*elem_factor;
		(*ppv)->set		(X0,Y0,it->color); (*ppv)++;
		float X1	= float(it-pelements->begin())*elem_offs+lt.x;
		float Y1	= base_y-it->data*elem_factor;
		(*ppv)->set		(X1,Y1,it->color); (*ppv)++;
	}
/*
	dwCount 				= u32(pv-pv_start);
    RCache.Vertex.Unlock	(dwCount,hGeomLine->vb_stride);
    RCache.set_Geometry		(hGeomLine);
    RCache.Render	   		(D3DPT_LINELIST,dwOffset,dwCount/2);
*/
};

void CStatGraph::RenderBarLines( FVF::TL0uv** ppv, ElementsDeq* pelements )
{
	float elem_offs		= float(rb.x-lt.x)/max_item_count;
	float elem_factor	= float(rb.y-lt.y)/float(mx-mn);
	float base_y		= float(rb.y)+(mn*elem_factor);

	for (ElementsDeqIt it=pelements->begin()+1; it!=pelements->end(); it++)
	{
		ElementsDeqIt it_prev = it-1;
		float X0	= float(it_prev-pelements->begin())*elem_offs+lt.x+elem_offs;
		float Y0	= base_y-it_prev->data*elem_factor;
		(*ppv)->set		(X0,Y0,it->color); (*ppv)++;
		float X1	= float(it-pelements->begin())*elem_offs+lt.x;
		float Y1	= base_y-it->data*elem_factor;
		(*ppv)->set		(X1,Y1,it->color); (*ppv)++;
		(*ppv)->set		(X1,Y1,it->color); (*ppv)++;
		X1 += elem_offs;
		(*ppv)->set		(X1,Y1,it->color); (*ppv)++;
	}
};

/*
void CStatGraph::RenderPoints( FVF::TL0uv** ppv, ElementsDeq* pelements )
{
	float elem_offs		= float(rb.x-lt.x)/max_item_count;
	float elem_factor	= float(rb.y-lt.y)/float(mx-mn);
	float base_y		= float(rb.y)+(mn*elem_factor);

	for (ElementsDeqIt it=pelements->begin()+1; it!=pelements->end(); it++)
	{
		float X1	= float(it-pelements->begin())*elem_offs+lt.x;
		float Y1	= base_y-it->data*elem_factor;
		(*ppv)->set		(X1,Y1,it->color); (*ppv)++;
	}
};
*/

void CStatGraph::OnRender()
{
	RCache.OnFrameEnd();
	
	RenderBack();

	u32			TriElem = 0;
	u32			LineElem = 0;
	for (SubGraphVecIt it=subgraphs.begin(); it!=subgraphs.end(); it++)
	{
		switch (it->style)
		{
		case stBar: 
			{
				TriElem += it->elements.size()*4;
			}break;
		case stCurve: 
			{
				LineElem += it->elements.size()*2;
			}break;
		case stBarLine: 
			{
				LineElem += it->elements.size()*4;
			}break;
		case stPoint: 
			{
//				TriElem += it->elements.size()*4;
			}break;
		};
	};

	u32			dwOffsetTri = 0, dwOffsetLine = 0,dwCount;
	FVF::TL0uv* pv_Tri_start	= NULL;
	FVF::TL0uv* pv_Tri;
	FVF::TL0uv* pv_Line_start	= NULL;
	FVF::TL0uv* pv_Line;

	if (TriElem)
	{
		pv_Tri_start = (FVF::TL0uv*)RCache.Vertex.Lock(TriElem,hGeomTri->vb_stride,dwOffsetTri);
		pv_Tri = pv_Tri_start;

		pv_Tri = pv_Tri_start;
		for (SubGraphVecIt it=subgraphs.begin(); it!=subgraphs.end(); it++)
		{
			switch(it->style)
			{
			case stBar:		RenderBars		(&pv_Tri, &(it->elements));		break;
//			case stCurve:	RenderLines		(&pv_Line, &(it->elements));	break;
//			case stBarLine:	RenderBarLines	(&pv_Line, &(it->elements));	break;
				//	case stPoint:	RenderPoints(&pv_Line, &(it->elements));		break;
			};
		};
		dwCount 				= u32(pv_Tri-pv_Tri_start);
		RCache.Vertex.Unlock	(dwCount,hGeomTri->vb_stride);
		RCache.set_Geometry		(hGeomTri);
		RCache.Render	   		(D3DPT_TRIANGLELIST,dwOffsetTri,0, dwCount, 0, dwCount/2);
	};

	if (LineElem)
	{
		pv_Line_start = (FVF::TL0uv*)RCache.Vertex.Lock(LineElem,hGeomLine->vb_stride,dwOffsetLine);
		pv_Line = pv_Line_start;

		for (SubGraphVecIt it=subgraphs.begin(); it!=subgraphs.end(); it++)
		{
			switch(it->style)
			{
//			case stBar:		RenderBars		(&pv_Tri, &(it->elements));		break;
			case stCurve:	RenderLines		(&pv_Line, &(it->elements));	break;
			case stBarLine:	RenderBarLines	(&pv_Line, &(it->elements));	break;
				//	case stPoint:	RenderPoints(&pv_Line, &(it->elements));		break;
			};
		};

		dwCount 				= u32(pv_Line-pv_Line_start);
		RCache.Vertex.Unlock	(dwCount,hGeomLine->vb_stride);
		RCache.set_Geometry		(hGeomLine);
		RCache.Render	   		(D3DPT_LINELIST,dwOffsetLine,dwCount/2);
	};
}

