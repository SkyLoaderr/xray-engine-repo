//----------------------------------------------------
// file: CEditObjectImport.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditObject.h"
#include "EditMesh.h"
#include "..\scenscan\objectdb.h"
#include <lwhost.h>

extern "C"	LWMessageFuncs	*g_msg;
DEFINE_MAP(void*,int,VMIndexLink,VMIndexLinkIt);

bool CEditObject::Import_LWO(st_ObjectDB *I){
	if (I){
        bool bResult=true;

        // parse lwo object
        {
        	m_Meshes.resize		(1);
            m_Surfaces.resize	(I->nsurfaces);

            // surfaces
            {
                for (int i=0; i<I->nsurfaces; i++){
		            DBSurface* Isf=&I->surf[i];
                    st_Surface* Osf = new st_Surface();
                    m_Surfaces[i] = Osf;
                    if (Isf->name) strcpy(Osf->name,Isf->name); 
					else strcpy(Osf->name,"Default");
                    Osf->sideflag = (Isf->side==3)?TRUE:FALSE;
                    // fill texture layers
					char tex_name[_MAX_FNAME];
					for (int tex_i=0; tex_i<Isf->tex_cnt; tex_i++){
						_splitpath( Isf->textures[tex_i], 0, 0, tex_name, 0 );
						Osf->textures.push_back(tex_name);
						// get vmap refs
						Osf->vmaps.push_back("");
					}
                    if (!bResult) break;
                    if (Osf->textures.empty()){
						g_msg->error("Can't create shader. Textures empty. Invalid surface:. ",Osf->name);
                        bResult = false;
                        break;
                    }
                    
                    Osf->shader = "default";
                
                    Osf->dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|(Isf->tex_cnt<<D3DFVF_TEXCOUNT_SHIFT);
                }
		    }
			if (bResult){
                // mesh 
				do{
                    // create new mesh
                    CEditMesh* MESH=new CEditMesh(this);
                    m_Meshes[0]=MESH;

                    strcpy(MESH->m_Name,"mesh");

                    // parse mesh data
                    // vmaps
                    DBVMap* Ivmap=0;
                    int vmap_count=0;
                    if (I->nvertmaps==0){
                        g_msg->error("Import LWO: Mesh layer must contain UV!",0);
                        bResult=false;
                        break;
                    }

                    // индексы соответствия импортируемых мап 
					static VMIndexLink VMIndices;
				    VMIndices.clear();

					for (int vm_i=0; vm_i<I->nvertmaps; vm_i++){
						Ivmap = &I->vmap[vm_i];
                    	switch(Ivmap->type){
                        case LWVMAP_TXUV:{
                            if (Ivmap->dim!=2){
                                g_msg->error("Import LWO: 'UV Map' must contain 2 value!",0);
                                bResult=false;
                                break;
                            }
                            MESH->m_VMaps.push_back(st_VMap(vmtUV));
                            st_VMap* Mvmap=&MESH->m_VMaps.back();
                            if (Ivmap->name) strcpy(Mvmap->name,Ivmap->name); else strcpy(Mvmap->name,"Default");
                            int vcnt=Ivmap->nverts;
                            // VMap, flip uv
							for (int vm_i=0; vm_i<Ivmap->nverts; vm_i++)
								Mvmap->appendUV(Ivmap->val[0][vm_i],1.f-Ivmap->val[1][vm_i]);
                            // vmap index
                            VMIndices[Ivmap] = vmap_count++;
                        }break;
						case LWVMAP_WGHT:{ 
                            if (Ivmap->dim!=1){
                                g_msg->error("Import LWO: 'Weight' must contain 1 value!",0);
                                bResult=false;
                                break;
                            }
                            MESH->m_VMaps.push_back(st_VMap(vmtWeight));
                            st_VMap* Mvmap=&MESH->m_VMaps.back();
                            if (Ivmap->name) strcpy(Mvmap->name,Ivmap->name); else strcpy(Mvmap->name,"Default");
                            int vcnt=Ivmap->nverts;
                            // VMap
                            Mvmap->copyfrom(*Ivmap->val,vcnt);
                            // vmap index
                            VMIndices[Ivmap] = vmap_count++;
                        }break;
						case LWVMAP_PICK: g_msg->error("Found 'PICK' VMAP. Import failed.",0); bResult = false; break;
						case LWVMAP_MNVW: g_msg->error("Found 'MNVW' VMAP. Import failed.",0); bResult = false; break;
						case LWVMAP_MORF: g_msg->error("Found 'MORF' VMAP. Import failed.",0); bResult = false; break;
						case LWVMAP_SPOT: g_msg->error("Found 'SPOT' VMAP. Import failed.",0); bResult = false; break;
						case LWVMAP_RGB:  g_msg->error("Found 'RGB' VMAP. Import failed.",0);  bResult = false; break;
						case LWVMAP_RGBA: g_msg->error("Found 'RGBA' VMAP. Import failed.",0); bResult = false; break;
                        }
	                    if (!bResult) break;
                    }
                    if (!bResult) break;
                    // points
                    {
                        MESH->m_Points.resize(I->npoints);
                        MESH->m_Adjs.resize(I->npoints);
                        for (int i=0; i<I->npoints; i++){
                            DBPoint& Ipt	= I->pt[i];
                            Fvector& Mpt	= MESH->m_Points[i];
                            INTVec& a_lst	= MESH->m_Adjs[i];
                            Mpt.set			(Ipt.pos[0]);
                            copy			(Ipt.pol,Ipt.pol+Ipt.npols,inserter(a_lst,a_lst.begin()));
                            sort			(a_lst.begin(),a_lst.end());
                        }
                    }
                    if (!bResult) break;
                    // polygons
                    MESH->m_Faces.resize(I->npolygons);
                    MESH->m_VMRefs.reserve(I->npolygons*3);
                    INTVec surf_ids;
                    surf_ids.resize(I->npolygons);
                    for (int p_i=0; p_i<I->npolygons; p_i++){
                        st_Face&		Mpol=MESH->m_Faces[p_i];
                        DBPolygon&   Ipol=I->pol[p_i];
                        if (Ipol.nverts!=3) {
							g_msg->error("Import LWO: Face must contain only 3 vertices!",0);
                        	bResult=false;
                            break;
                        }
                        for (int pv_i=0; pv_i<3; pv_i++){
                        	DBPolVert& 		Ipv=Ipol.v[pv_i];
                            st_FaceVert&  	Mpv=Mpol.pv[pv_i];
                            Mpv.pindex		=Ipv.index;

							MESH->m_VMRefs.push_back(VMapPtSVec());
							VMapPtSVec&	vm_lst = MESH->m_VMRefs.back();
							Mpv.vmref = MESH->m_VMRefs.size()-1;

							// parse uv-map
							int vmp_cnt		=Ipv.nvmaps;
							if (vmp_cnt){
                            	// берем из poly
    							for (int vm_i=0; vm_i<vmp_cnt; vm_i++){
									if (Ipv.vm[vm_i].vmap->type!=LWVMAP_TXUV) continue;
									vm_lst.push_back(st_VMapPt());
									st_VMapPt& pt	= vm_lst.back();
        							pt.vmap_index	= VMIndices[Ipv.vm[vm_i].vmap];// номер моей VMap
            						pt.index 		= Ipv.vm[vm_i].index;
                				}
							}else{
                            	// берем из points
                                DBPoint& Ipt	= I->pt[Mpv.pindex];
                                int vm_cnt		=Ipt.nvmaps;
                                if (!vm_cnt){
	                                g_msg->error("Can't find polygon/point UV-map!",0);
                                    bResult		= false;
                                    break;
                                }                                  
                                for (int vm_i=0; vm_i<vm_cnt; vm_i++){
									if (Ipt.vm[vm_i].vmap->type!=LWVMAP_TXUV) continue;
									vm_lst.push_back(st_VMapPt());
									st_VMapPt& pt	= vm_lst.back();
        	                        pt.vmap_index	= VMIndices[Ipt.vm[vm_i].vmap]; // номер моей VMap
            	                    pt.index 		= Ipt.vm[vm_i].index;
                                }
							}

							// parse weight-map
                            DBPoint& Ipt	= I->pt[Mpv.pindex];
                            int vm_cnt		=Ipt.nvmaps;
                            for (int vm_i=0; vm_i<vm_cnt; vm_i++){
								if (Ipt.vm[vm_i].vmap->type!=LWVMAP_WGHT) continue;
								vm_lst.push_back(st_VMapPt());
								st_VMapPt& pt	= vm_lst.back();
        	                    pt.vmap_index	= VMIndices[Ipt.vm[vm_i].vmap]; // номер моей VMap
            	                pt.index 		= Ipt.vm[vm_i].index;
                            }

						}
                        if (!bResult) break;
						// Ipol.surf->alpha_mode - заполнено как номер моего surface
                        surf_ids[p_i]			= Ipol.sindex;
                    }
                    if (!bResult) break;
					int p_idx=0;
                    for (FaceIt pl_it=MESH->m_Faces.begin(); pl_it!=MESH->m_Faces.end(); pl_it++){
						MESH->m_SurfFaces[m_Surfaces[surf_ids[p_idx]]].push_back(p_idx);
                        p_idx++;
                    }
                    if (!bResult) break;
					MESH->RecomputeBBox();

					// check weight maps
					if (!m_Bones.empty()){
						for (BoneIt b_it=m_Bones.begin(); b_it!=m_Bones.end(); b_it++){
							if ((*b_it)->WMap()[0]&&!MESH->FindVMapByName((*b_it)->WMap(),vmtWeight)){
								g_msg->error("Can't find weight map:",(*b_it)->Name());
								bResult = false;
								break;
							}
						}
					}
                }while(0);
    		}
		}
    	if (!bResult) g_msg->error("Can't parse LWO object.",0);

        return bResult;
    }else
    	g_msg->error("Can't export LWO object file.",0);
    return false;
}

