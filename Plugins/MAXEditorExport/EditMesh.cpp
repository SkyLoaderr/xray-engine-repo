//----------------------------------------------------
// file: StaticMesh.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "EditMesh.h"
#include "EditObject.h"
#include "FS.h"

#include "NetDeviceLog.h"
#include "MeshExpUtility.h"

CEditMesh::~CEditMesh(){
	Clear();
}

void CEditMesh::Clear(){
	m_Points.clear 		();
    m_Adjs.clear		();
	m_Faces.clear		();
    m_VMaps.clear		();
    m_SurfFaces.clear	();
    m_VMRefs.clear		();
}

void CEditMesh::RecomputeBBox(){
	if( m_Points.empty() ){
		m_Box.set(0,0,0, 0,0,0);
		return;
    }
	m_Box.set( m_Points[0], m_Points[0] );
	for(FvectorIt pt=m_Points.begin()+1; pt!=m_Points.end(); pt++)
		m_Box.modify(*pt);
}
//----------------------------------------------------

void CEditMesh::FlipFaces(){
	for(FaceIt f = m_Faces.begin(); f!=m_Faces.end(); f++){
		st_FaceVert v = f->pv[0];
		f->pv[0] = f->pv[2];
		f->pv[2] = v;
	}
}
//----------------------------------------------------------------------------

TriObject *CEditMesh::ExtractTriObject( INode *node, int &deleteIt )
{
	deleteIt = FALSE;
	Object *obj = node->EvalWorldState(0).obj;
	if (obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) { 
		TriObject *tri = (TriObject *) obj->ConvertToType(0, 
			Class_ID(TRIOBJ_CLASS_ID, 0));
		// Note that the TriObject should only be deleted
		// if the pointer to it is not equal to the object
		// pointer that called ConvertToType()
		if (obj != tri) deleteIt = TRUE;
		return tri;
	}else{
		return NULL;
	}
}
//----------------------------------------------------------------------------
bool CEditMesh::ExtractTexName( char *dest, Texmap *map ){
	if( map->ClassID() != Class_ID(BMTEX_CLASS_ID,0) )
		return false;
	BitmapTex *bmap = (BitmapTex*)map;
	_splitpath( bmap->GetMapName(), 0, 0, dest, 0 );
//	_splitpath( bmap->GetMapName(), 0, 0, 0, dest + strlen(dest) );
	return true;
}
//----------------------------------------------------------------------------
bool CEditMesh::ExtractMaterial( st_Surface *surf, StdMat *smtl ){
	NConsole.print( "- Processing material '%s' ...", smtl->GetName() );
	// ---- color info
/*	Color ambient		= smtl->GetAmbient(0);
	Color diffuse		= smtl->GetDiffuse(0);
	Color specular		= smtl->GetSpecular(0);
	Color emission		= smtl->GetSelfIllumColor(0);
	float st			= smtl->GetShinStr(0);
	float power			= smtl->GetShininess(0)*100.f;
	
	dest->m_Mat.ambient.set( ambient.r, ambient.g, ambient.b, 0 );
	dest->m_Mat.diffuse.set( diffuse.r, diffuse.g, diffuse.b, 1 );
	dest->m_Mat.specular.set( st*specular.r, st*specular.g, st*specular.b, 1 );
	dest->m_Mat.emissive.set( emission.r, emission.g, emission.b, 1 );
	dest->m_Mat.power = power;
*/	
	// ------- texture (if exist)
	char tname[1024];
	if( smtl->MapEnabled( ID_AM ) ){
		if( smtl->GetSubTexmap( ID_AM ) ){
			if (!ExtractTexName( tname, smtl->GetSubTexmap( ID_AM ) )) return false;
			surf->textures.push_back(tname);
		}else{
			return false;
		}
	}else 
		if( smtl->MapEnabled( ID_DI ) ){
			if( smtl->GetSubTexmap( ID_DI ) ){
				if (!ExtractTexName( tname, smtl->GetSubTexmap( ID_DI ) )) return false;
				surf->textures.push_back(tname);
			}else{
				return false;
			}
		}else{
			return false;
		}
	surf->sideflag = smtl->GetTwoSided();
	if (surf->sideflag) NConsole.print( "  - material 2-sided");
	m_Parent->GenerateSurfaceName(surf->name,smtl->GetName());
	surf->dwFVF = D3DFVF_XYZ|D3DFVF_NORMAL|(1<<D3DFVF_TEXCOUNT_SHIFT);
	surf->shader = "default";
	surf->vmaps.push_back("?");
	return true;
}
//----------------------------------------------------------------------------

bool CEditMesh::Convert( INode *node ){
	// prepares & checks
	BOOL bDeleteObj;
	bool bResult = true;
	TriObject *obj = ExtractTriObject( node, bDeleteObj );
	
	if( !obj ){
		NConsole.print( "%s -> Can't convert to TriObject", node->GetName() );
		return false; }
	
	if( obj->mesh.getNumFaces() <=0 ){
		NConsole.print( "%s -> There are no faces ?", node->GetName() );
		if (bDeleteObj) _DELETE(obj);
		return false; }
	
	Mtl *pMtlMain = node->GetMtl();
	DWORD cSubMaterials=0;
	
	if (pMtlMain){
		// There is at least one material. We're in case 1) or 2)
		cSubMaterials = pMtlMain->NumSubMtls();
		if (cSubMaterials < 1){
			// Count the material itself as a submaterial.
			cSubMaterials = 1;
		}
	}
	
	// build normals
	obj->mesh.buildRenderNormals();

	// vertices
	int v_cnt = obj->mesh.getNumVerts();
	m_Points.resize(v_cnt);
	m_Adjs.resize(v_cnt);
	for (int v_i=0; v_i<v_cnt; v_i++){
		Point3* p = obj->mesh.verts+v_i;
		m_Points[v_i].set(p->x,p->y,p->z);
	}

	// faces
	int f_cnt = obj->mesh.getNumFaces();
	m_Faces.resize(f_cnt);
	if (!U.m_ObjectSuppressSmoothGroup&&!U.m_ObjectNoOptimize) 
		m_PNormals.resize(f_cnt*3);
	NConsole.print("m_PNormals.size()=%d",m_PNormals.size());

	m_VMRefs.reserve(f_cnt*3);
	VERIFY(obj->mesh.mapFaces(1));
	st_Surface* surf=0;
	for (int f_i=0; f_i<f_cnt; f_i++){
		Face*	vf = obj->mesh.faces+f_i;
		TVFace* tf = obj->mesh.mapFaces(1) + f_i;
		if (!tf){
			bResult = false;
			NConsole.print( "'%s' hasn't UV mapping!",
				node->GetName(), m_Points.size(), m_Faces.size());
			break;
		}
		for (int k=0; k<3; k++){
			m_Faces[f_i].pv[k].pindex = vf->v[k];
			m_VMRefs.push_back(VMapPtList());
            VMapPtList&	vm_lst = m_VMRefs.back();
			DWORD vm_cnt = 1;
            vm_lst.resize(vm_cnt);
    	    for (int vm_i=0; vm_i<vm_cnt; vm_i++){
        		vm_lst[vm_i].vmap_index	= 0;
            	vm_lst[vm_i].index 		= tf->t[k];
			}
            m_Faces[f_i].pv[k].vmref = m_VMRefs.size()-1;

			// normals
			if (!U.m_ObjectSuppressSmoothGroup&&!U.m_ObjectNoOptimize){
				int g = vf->getSmGroup(); // smooth group
				RVertex* r = obj->mesh.getRVertPtr(vf->v[k]);
				int nbNormals = r->rFlags & NORCT_MASK;
				Point3* n;
				if (nbNormals == 1)
					n = &r->rn.getNormal();
				else{
					for (int j = 0; j < nbNormals; j++) {
						RNormal* rn = &r->ern[j];
						if (rn->getSmGroup() & g) {
							n = &rn->getNormal();
							break;
						}
					}
				}
				if (!n){
					bResult = false;
					NConsole.print("'%s' can't give normals.",node->GetName());
					break;
				}
//				NConsole.print("#%d sm:%d [%3.2f, %3.2f, %3.2f]",k,g,n->x,n->y,n->z);
				m_PNormals[f_i*3+k].set(n->x,n->y,n->z);
			}
		}
		if (pMtlMain){
			int m_id = obj->mesh.getFaceMtlIndex(f_i);
			if (cSubMaterials == 1){
				m_id = 0;
			}else{
				// SDK recommends mod'ing the material ID by the valid # of materials, 
				// as sometimes a material number that's too high is returned.
				m_id %= cSubMaterials;
			}
			surf = m_Parent->CreateSurface(pMtlMain,m_id);
			VERIFY(surf);
		}
		m_SurfFaces[surf].push_back(f_i);
	}

	// vmaps
	if( bResult ){
		int vm_cnt = obj->mesh.getNumTVerts();
		m_VMaps.resize(1);
		st_VMap& VM = m_VMaps.back();
		for (int tx_i=0; tx_i<vm_cnt; tx_i++){
			UVVert* tv = obj->mesh.tVerts + tx_i;
			VM.vm.push_back(Fvector2());
			Fvector2& uv = VM.vm.back();
			uv.set(tv->x,1-tv->y);
		}
	}


	if (bResult ){
		NConsole.print( "Model '%s' contains: %d points, %d faces",
			node->GetName(), m_Points.size(), m_Faces.size());
	}
	
	if( bResult ){
		if( pMtlMain ){
			if( pMtlMain->ClassID() == Class_ID(MULTI_CLASS_ID,0) ){
				NConsole.print( "'%s' -> multi material '%s' ...", node->GetName(), pMtlMain->GetName() );
				MultiMtl *mmtl = (MultiMtl*)pMtlMain;
				
				for (SurfaceIt s_it=m_Parent->FirstSurface(); s_it!=m_Parent->LastSurface(); s_it++){
					if ((*s_it)->pMtlMain!=pMtlMain) continue;
					st_Surface* surf = *s_it;
					if( mmtl->GetSubMtl(surf->mat_id)->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
						StdMat *smtl = (StdMat*)mmtl->GetSubMtl(surf->mat_id);
						if (!ExtractMaterial(surf, smtl)){
							NConsole.print( " * can't extract material!!!");
							bResult = false;
							break;
						}
					} else {
						NConsole.print( "'%s' -> warning: bad submaterial '%s'", 
							node->GetName(), mmtl->GetSubMtl(surf->mat_id)->GetName() );
						bResult = false;
						break;
					}
				}
				//			if (m_Layers.size()!=cnt)
				//				reverse(m_Layers.begin()+cnt,m_Layers.end());
			} else if( pMtlMain->ClassID() == Class_ID(DMTL_CLASS_ID,0) ){
				NConsole.print( "'%s' -> std material '%s' ...", node->GetName(), pMtlMain->GetName() );
				StdMat *smtl = (StdMat*)pMtlMain;
				for (SurfaceIt s_it=m_Parent->FirstSurface(); s_it!=m_Parent->LastSurface(); s_it++){
					if ((*s_it)->pMtlMain!=pMtlMain) continue;
					st_Surface* surf = *s_it;
					if (!ExtractMaterial( surf, smtl )){
						NConsole.print( " * can't extract material!!!");
						bResult = false;
						break;
					}
				}
			} else {
				NConsole.print( "'%s' -> unknown material class...", node->GetName() );
				bResult = false;
			}
		} else {
			NConsole.print( "'%s' -> warning: no material", node->GetName() );
			bResult = false;
		}
	}
	
	if (bResult){
		RecomputeBBox();
		Optimize(U.m_ObjectNoOptimize);
	}
	
	if (bResult){
		NConsole.print( "Model '%s' converted: %d points, %d faces",
			node->GetName(), m_Points.size(), m_Faces.size());
	}
	
	if (bDeleteObj) _DELETE(obj);
	return bResult;
}
//----------------------------------------------------------------------------

