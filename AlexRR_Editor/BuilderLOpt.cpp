//----------------------------------------------------
// file: BuilderLOpt.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "SObject2.h"
//----------------------------------------------------


void SceneBuilder::CMAP_Create( CURRENT_TMAP *map, int& nameindex ){
	
	char tname[MAX_PATH];
	sprintf( tname, "%s#%s#%d", "lm", m_Scene->m_BOPLMapPrefix, nameindex );
	nameindex++;

	_ASSERTE( map );
	map->map = new ETexture( LMAP_W * LMAP_QW, LMAP_H * LMAP_QH, tname );
	map->wfill = 0;
	map->hfill = 0;
}

void SceneBuilder::CMAP_Validate( CURRENT_TMAP *map ){
	_ASSERTE( map );
	_ASSERTE( map->map );
	map->map->DDReinit();
}

void SceneBuilder::CMAP_Delete( CURRENT_TMAP *map ){
	_ASSERTE( map );
	SAFE_DELETE( map->map );
}

bool SceneBuilder::CMAP_Add( CURRENT_TMAP *map, ETexture *t, int& nameindex ){

	_ASSERTE( map );
	_ASSERTE( map->map );
	_ASSERTE( t );

	// -- ensure map is valid --

	if( map->hfill >= LMAP_QH ){
		CMAP_Validate( map );
		CMAP_Delete( map );
		CMAP_Create( map, nameindex );
	} else if( map->hfill == (LMAP_QH-1) ){
		if( map->wfill >= LMAP_QW ){
			CMAP_Validate( map );
			CMAP_Delete( map );
			CMAP_Create( map, nameindex );
		}
	}

	if( t->width() != LMAP_W )
		return false;

	if( t->height() != LMAP_H )
		return false;

	// -- append to free place --

	_ASSERTE( map->map->m_Ref );
	ETextureCore *mapcore = map->map->m_Ref;

	_ASSERTE( t->m_Ref );
	ETextureCore *tcore = t->m_Ref;

	int x_start = LMAP_W * map->wfill;
	int y_start = LMAP_H * map->hfill;

	if( map->wfill >= LMAP_QW ){
		map->wfill = 0;
		x_start = 0;
		map->hfill++;
		y_start += LMAP_H;
	}

	map->prev_wfill = map->wfill;
	map->prev_hfill = map->hfill;

	for( int i=0; i<LMAP_H; i++)
		memcpy(
			mapcore->m_Pixels.begin() + x_start + ((i+y_start) * mapcore->m_Width),
			tcore->m_Pixels.begin() + (i * tcore->m_Width),
			tcore->m_Width * sizeof(IColor) );

	map->wfill++;
	if( map->wfill >= LMAP_QW ){
		map->wfill = 0;
		map->hfill++;
	}

	return true;
}



bool SceneBuilder::OptimizeLightmaps(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	int mapindex = 0;
	
	CURRENT_TMAP map;
	CMAP_Create( &map, mapindex );

	// create lightmaps and place objects into it

	object_progress = 0.f;
	BWindow.SetProgress( object_progress );

	bool nomorereasons = true;

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){

			object_progress += object_cost;
			BWindow.SetProgress( object_progress );

			SObject2 *obj = (SObject2*)(*i);
			if( obj->IsReference() )
				continue;

			SObjMeshIt itm = obj->m_Meshes.begin();
			for(;itm!=obj->m_Meshes.end();itm++){

				if( !itm->m_Ops.m_TextureLighting )
					continue;

				Mesh *mesh = itm->m_Mesh;
				_ASSERTE( mesh );

				for( int j=0; j<mesh->m_Materials.size(); j++){

					MMaterial *material = mesh->m_Materials.begin() + j;
					_ASSERTE( material->m_LightMap );

					if( CMAP_Add( &map, material->m_LightMap, mapindex ) ){
						
						SAFE_DELETE( material->m_LightMap );
						material->m_LightMap = new ETexture( map.map );

						for( int _face=0; _face<material->m_FaceCount; _face++){
							for( int k=0; k<3; k++){
								int _point = mesh->m_Faces[material->m_FaceStart+_face].p[k];
								mesh->m_LMap[_point].m_Valid = false;
							}
						}

						float qu = 1.f / (float)LMAP_QW;
						float qv = 1.f / (float)LMAP_QH;
						float off_u = qu * map.prev_wfill;
						float off_v = qv * map.prev_hfill;

						for( _face=0; _face<material->m_FaceCount; _face++){
							for( int k=0; k<3; k++){

								int _point = mesh->m_Faces[material->m_FaceStart+_face].p[k];
								if( !mesh->m_LMap[_point].m_Valid ){
									mesh->m_LMap[_point].m_Valid = true;
									mesh->m_LMap[_point].u = mesh->m_LMap[_point].u * qu + off_u;
									mesh->m_LMap[_point].v = mesh->m_LMap[_point].v * qv + off_v;
								}
							}
						}

					}
				}

				mesh->FillD3DPoints();
			}
		}
	}

	CMAP_Validate( &map );
	CMAP_Delete( &map );

	NConsole.print(
		"Building: %d lightmap(s) left...",
		mapindex );

	return true;
}


