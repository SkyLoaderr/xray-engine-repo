//----------------------------------------------------
// file: EditObjectExport.cpp
/* error codes
101             Layer 
102             Pivot
103				is also a bad parent layer
104             Parent
105             Layer Flags
106             Point   
107             VMap
108             VMap Value
109             Polygon
110             Poly Tag
111             Surface 
*/
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"

#include <lwsurf.h>
#include <lwmodeler.h>
#include <lwimage.h>

extern "C" LWSurfaceFuncs	*g_surff;
extern "C" LWObjectFuncs	*g_objfunc;
extern "C" LWStateQueryFuncs*g_query;
extern "C" LWTextureFuncs	*g_txfunc;
extern "C" LWItemInfo		*g_iteminfo;
extern "C" LWImageList		*g_imglist;


bool CEditableObject::Export_LW(LWObjectImport *lwi)
{
	LWFVector pos = { 0.0f };
	
	DEFINE_VECTOR(LWPntID,LWPntIDVec,LWPntIDIt);

	string128 obj_name, obj_ext;
	_splitpath(lwi->filename,0,0,obj_name,obj_ext);
	strcat(obj_name,obj_ext);

	for (SurfaceIt s_it=m_Surfaces.begin(); s_it!=m_Surfaces.end(); s_it++){
		(*s_it)->surf_id = g_surff->create(obj_name,(*s_it)->_Name());
		LWTextureID T=g_surff->getTex((*s_it)->surf_id,SURF_COLR);
		T = g_txfunc->create(TRT_SCALAR,(*s_it)->_Texture(),0,0);
		LWTLayerID	L=g_txfunc->layerAdd(T,TLT_IMAGE);
		LWImageID	I=g_imglist->load((*s_it)->_Texture());
		g_txfunc->setParam( L, TXTAG_IMAGE, &I );
		int tp=TXPRJ_UVMAP;
		g_txfunc->setParam( L, TXTAG_PROJ, (void*)&tp );
	}

	LWPntIDVec pntID;
	for (EditMeshIt mesh_it=FirstMesh(); mesh_it!=LastMesh(); mesh_it++){
		CEditableMesh* MESH=*mesh_it;
		pntID.resize(MESH->GetVertexCount());
		lwi->layer( lwi->data, mesh_it-FirstMesh(), MESH->GetName() );
		lwi->pivot( lwi->data, pos );

		// create points
		for (FvectorIt point_it=MESH->m_Points.begin(); point_it!=MESH->m_Points.end(); point_it++)
			if ( !( pntID[point_it-MESH->m_Points.begin()] = lwi->point( lwi->data, (float*)point_it ))) R_ASSERT(0);

		// create polygons
		for (SurfFacesPairIt sf_it=MESH->m_SurfFaces.begin(); sf_it!=MESH->m_SurfFaces.end(); sf_it++){
			CSurface* SURFACE	= sf_it->first;
			INTVec& lst			= sf_it->second;
			for (INTIt i_it=lst.begin(); i_it!=lst.end(); i_it++){
				st_Face& FACE	= MESH->m_Faces[*i_it];
				LWPntID vID[3];
				for (int k=0; k<3; k++){ 
		            st_FaceVert& FV = FACE.pv[k];
					// point
					vID[k]=pntID[FV.pindex];
				}
				LWPolID polID = lwi->polygon( lwi->data, LWPOLTYPE_FACE, 0, 3, vID );
				lwi->polTag( lwi->data, polID, LWPTAG_SURF, SURFACE->_Name() );
				for (k=0; k<3; k++){ 
		            st_FaceVert& FV = FACE.pv[k];
					// uv map
           			VMapPtSVec& vm_ptvec 	= MESH->m_VMRefs[FV.vmref];
					for (int t=0; t<int(vm_ptvec.size()); t++){
	           			st_VMapPt& vm_pt 	= MESH->m_VMRefs[FV.vmref][t];
						R_ASSERT(vm_pt.vmap_index<int(MESH->m_VMaps.size()),"- VMap index out of range");
						// select vmap
						st_VMap& vmap		= *MESH->m_VMaps[vm_pt.vmap_index];
						lwi->vmap(lwi->data, (vmap.type==vmtUV)?LWVMAP_TXUV:LWVMAP_WGHT, vmap.dim, (vmap.name&&vmap.name[0])?vmap.name:"texture");
						// append vmap_pt
						R_ASSERT(vm_pt.index<vmap.size(),"- VMap point index out of range");
						float* vm=0;
						if (vmap.type==vmtUV){ 
							vm=(float*)&vmap.getUV(vm_pt.index);
							vm[1]=1.f-vm[1];
						}else{
							vm=(float*)&vmap.getW(vm_pt.index);
						}
						if (vmap.polymap)	lwi->vmapPDV(lwi->data,vID[k],polID,vm);
						else				lwi->vmapVal(lwi->data,vID[k],vm);
					}
				}
			}
		}
	}

	lwi->result = LWOBJIM_OK;
	lwi->done( lwi->data );

	return true;
}
