//----------------------------------------------------
// file: BuilderCFModel.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "Landscape.h"
#include "SObject2.h"
//----------------------------------------------------


#pragma pack(push,1)
struct _cform_mesh_header {
	DWORD facecount;
	IVector vcenter;
	float radius;
	IVector vmin;
	IVector vmax;
};
struct _cform_mesh_face {
	IVector n;
	IVector p0;
	IVector p1;
	IVector p2;
};
#pragma pack(pop)


bool SceneBuilder::BuildCollisionModel(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	char coll_filename[MAX_PATH];
	strcpy( coll_filename, Scene.m_FNForms );
	m_LevelPath.Update( coll_filename );
	int handle = FS.create( coll_filename );

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){
			SObject2 *obj = (SObject2*)(*i);
			if( !obj->IsReference() ){
				if( !AddObjectCollisionForm( handle, obj ) ){
					FS.close( handle );
					return false;
				}
			}
			object_progress += object_cost;
			BWindow.SetProgress( object_progress );
		}
	}

	FS.close( handle );
	return true;
}


bool SceneBuilder::AddObjectCollisionForm( int handle, SObject2 *obj ){
	
	_cform_mesh_header header;
	vector<_cform_mesh_face> faces;

	header.facecount = 0;
	header.vmin.set(0,0,0);
	header.vmax.set(0,0,0);

	// calculate bounding box

	bool first_vertex = true;
	SObjMeshIt i = obj->m_Meshes.begin();
	for(;i!=obj->m_Meshes.end();i++){
		if( i->m_Ops.m_CollisionList ){
			Mesh *mesh = i->m_Mesh;
			for( int j=0; j<mesh->m_Points.size(); j++){
				if(first_vertex){
					header.vmin.set( mesh->m_Points[j].m_Point );
					header.vmax.set( mesh->m_Points[j].m_Point );
					first_vertex = false;
				} else {
					header.vmin.x = min( header.vmin.x, mesh->m_Points[j].m_Point.x );
					header.vmin.y = min( header.vmin.y, mesh->m_Points[j].m_Point.y );
					header.vmin.z = min( header.vmin.z, mesh->m_Points[j].m_Point.z );
					header.vmax.x = max( header.vmax.x, mesh->m_Points[j].m_Point.x );
					header.vmax.y = max( header.vmax.y, mesh->m_Points[j].m_Point.y );
					header.vmax.z = max( header.vmax.z, mesh->m_Points[j].m_Point.z );
				}
			}
		}
	}

	if( first_vertex ){
		NConsole.print( "AddObjectCollisionForm(): No vertices found..." );
		return true;
	}

	// calculate center

	header.vcenter.add(header.vmax,header.vmin);
	header.vcenter.mul( 0.5f );
	header.radius = 0;

	// calculate radius

	i = obj->m_Meshes.begin();
	for(;i!=obj->m_Meshes.end();i++){
		if( i->m_Ops.m_CollisionList ){
			Mesh *mesh = i->m_Mesh;
			for( int j=0; j<mesh->m_Points.size(); j++){
				IVector l2;
				l2.sub( mesh->m_Points[j].m_Point, header.vcenter );
				float l = l2.lenght();
				header.radius = max( header.radius, l );
			}
		}
	}

	// add faces
	header.facecount = 0;
	i = obj->m_Meshes.begin();
	for(;i!=obj->m_Meshes.end();i++){
		if( i->m_Ops.m_CollisionList ){
			Mesh *mesh = i->m_Mesh;
			for( int j=0; j<mesh->m_Faces.size(); j++){
				_cform_mesh_face face;
				face.p0.set( mesh->m_Points[mesh->m_Faces[j].p0].m_Point );
				face.p1.set( mesh->m_Points[mesh->m_Faces[j].p1].m_Point );
				face.p2.set( mesh->m_Points[mesh->m_Faces[j].p2].m_Point );
				face.n.mknormal( face.p0, face.p1, face.p2 );
				faces.push_back( face );
				header.facecount++;
			}
		}
	}

	if( faces.size() <= 0 ){
		NConsole.print( "AddObjectCollisionForm(): No faces found..." );
		return false;
	}

	FS.write( handle, &header, sizeof(header) );
	FS.write( handle, faces.begin(), faces.size()*sizeof(_cform_mesh_face));

	faces.clear();

	return true;
}
