//----------------------------------------------------
// file: BuilderFakeCF.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "SObject2.h"
//----------------------------------------------------


// FIRST CANDIDATE TO REMOVE ............. !!!


#pragma pack( push,1 )
struct static_maps_file_header_t{
	float square_size;
	long cx,cz;
};
#define CELL_FLAG_CAN_WALK 0x00000001
struct static_maps_file_node_t{
	DWORD flags;
	float height;
};
#pragma pack( pop )

#define MIN_NODE_HEIGHT (-1000.f)
#define MAX_NODE_HEIGHT (+1000.f)

//----------------------------------------------------


bool SceneBuilder::PickCFCell( int x, int z, float *top, float *bottom ){
/*
	float top_alpha = 0;
	bool top_picked = false;
	float half_x = m_Scene->m_BOPVisCellSize.x * 0.5f;
	float half_z = m_Scene->m_BOPVisCellSize.z * 0.5f;

	IVector top_start;
	top_start.x = x * m_Scene->m_BOPVisCellSize.x + half_x;
	top_start.z = z * m_Scene->m_BOPVisCellSize.z + half_z;
	top_start.y = 10000;

	IVector top_dir;
	top_dir.set( 0, -1, 0 );

	ObjectIt it = m_Scene->FirstObj();
	for(;it!=m_Scene->LastObj();it++){
		if((*it)->ClassID()==OBJCLASS_SOBJECT2){

			SObject2 *obj = (SObject2*)(*it);
			if( obj->IsReference() )
				continue;

			IAABox box;
			obj->GetBox( box );

			box.vmin.x -= m_Scene->m_BOPVisCellSize.x;
			box.vmin.z -= m_Scene->m_BOPVisCellSize.z;
			box.vmax.x += m_Scene->m_BOPVisCellSize.x;
			box.vmax.z += m_Scene->m_BOPVisCellSize.z;

			if( top_start.x < box.vmin.x ||
				top_start.z < box.vmin.z ||
				top_start.x > box.vmax.x ||
				top_start.z > box.vmax.z )
					continue;

			SObjMeshIt itm = obj->m_Meshes.begin();
			for(;itm!=obj->m_Meshes.end();itm++){

				if( !itm->m_Ops.m_CollisionList )
					continue;

				IMatrix matrix;
				matrix.set( obj->m_Position );
				matrix.mul2( itm->m_Position );

				float topdist = 0.f;
				if( itm->m_Mesh->Pick( &topdist, top_start, top_dir, matrix ) ){
					if( top_picked ){
						top_alpha = min(top_alpha,topdist);
					} else {
						top_picked = true;
						top_alpha = topdist;
					}
				}
			}
		}
	}

	if( top_picked ){
		(*top) = 10000.f - top_alpha;
		(*bottom) = (*top);
		return true;
	}

	return false;
*/
#pragma message("AlexMX: убран тест слотов за ненадобностью......")
	return true;
}
//----------------------------------------------------

bool SceneBuilder::BuildFakeCollision(){

	if( m_Scene->m_BOPVisCellSize.x != m_Scene->m_BOPVisCellSize.z ){
		NConsole.print( "In order to build fake collision cell must be square..." );
		return false;
	}

	float slot_cost = 100.f / (float)(m_VMap.X()*m_VMap.Z());
	float cur_progress = 0;

	static_maps_file_header_t header;
	header.square_size = m_Scene->m_BOPVisCellSize.x;
	header.cx = m_VMap.X();
	header.cz = m_VMap.Z();

	if( header.cx <= 0 || header.cz <= 0 )
		return false;

	char cmfilename[MAX_PATH];
	strcpy(cmfilename,"static.maps");  // !!!!!!!!! FIXME
	m_LevelPath.Update( cmfilename );
	int handle = FS.create( cmfilename );
	FS.write( handle, &header, sizeof(header));

	float half_x = m_Scene->m_BOPVisCellSize.x * 0.5f;
	float half_z = m_Scene->m_BOPVisCellSize.z * 0.5f;

	IVector top_direction,bot_direction;
	top_direction.set(0,-1,0);
	bot_direction.set(0,1,0);
	
	for( int i=0; i<m_VMap.X(); i++)
		for( int j=0; j<m_VMap.Z(); j++){

			static_maps_file_node_t cell;

			float ftop = 0, fbottom = 0;
			if( PickCFCell( i, j, &ftop, &fbottom ) ){
				cell.height = ftop;
				cell.flags = CELL_FLAG_CAN_WALK;
			} else {
				cell.height = MIN_NODE_HEIGHT;
				cell.flags = 0;
			}

			FS.write( handle, &cell, sizeof(cell));

			cur_progress += slot_cost;
			BWindow.SetProgress( cur_progress );
		}

	FS.close( handle );
	return true;
}
