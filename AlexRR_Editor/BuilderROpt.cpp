//----------------------------------------------------
// file: BuilderROpt.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "SObject2.h"
//----------------------------------------------------

// maximum distance between 
// objects which can be connected
float g_ConnectDist = 1.f;

// minimum object's size
// to connect with other object
float g_ObjSz = 15.f;      


bool SceneBuilder::OptimizeRenderObjects(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	int elimcount = 0;

	// connecting objects by distance

	while( 1 ){

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

				IAABox box;
				obj->GetBox( box );

				IVector sizes;
				box.getsize( sizes );
				if( sizes.x > g_ObjSz ||
					sizes.y > g_ObjSz ||
					sizes.z > g_ObjSz    )
						continue;

				// searching object to connect to...
				
				IVector c;
				box.center( c );
				float r = 0.5f * sizes.lenght();

				SObject2 *connect_to = 0;

				ObjectIt j = m_Scene->FirstObj();
				for(;j!=m_Scene->LastObj();j++){
					if((*j)->ClassID()==OBJCLASS_SOBJECT2){

						if( i==j )
							continue;

						SObject2 *obj1 = (SObject2*)(*j);
						if( obj1->IsReference() )
							continue;

						IAABox box1;
						obj1->GetBox( box1 );

						IVector sizes1;
						box1.getsize( sizes1 );
						if( sizes1.x > g_ObjSz ||
							sizes1.y > g_ObjSz ||
							sizes1.z > g_ObjSz    )
								continue;

						if( box.intersect( box1 ) ){
							if( VerifyConnectionOptions(obj,obj1) ){
								connect_to = obj1;
								break;
							}
						} else {

							IVector c1;
							box1.center( c1 );
							float r1 = 0.5f * sizes1.lenght();

							c1.sub( c );
							if( c1.lenght() <= (g_ConnectDist+r1+r) ){
								if( VerifyConnectionOptions(obj,obj1) ){
									connect_to = obj1;
									break;
								}
							}

						}

					}
				}

				if( connect_to ){

					// connect & restart...

					if( !ConnectObjects( obj, connect_to ) ){
						NConsole.print( "Failed to connect objects..." );
						return false;
					} else {
						elimcount++;
					}

					m_Scene->FullRemove( connect_to );
					nomorereasons = false;
					break;
				}
				
			}
		}

		if( nomorereasons )
			break;
	}

	NConsole.print(
		"Building: %d object(s) eliminated with first stage...",
		elimcount );

	return true;
}



bool SceneBuilder::ConnectObjects( SObject2 *first, SObject2 *second ){

	first->TranslateToWorld();
	second->TranslateToWorld();

	SObjMeshIt m = second->m_Meshes.begin();
	for(;m!=second->m_Meshes.end();m++){
		first->m_Meshes.push_back(SObject2Mesh());
		first->m_Meshes.back().m_Mesh=new Mesh();
		first->m_Meshes.back().m_Mesh->Copy( (*m).m_Mesh );
		first->m_Meshes.back().m_Position.set( (*m).m_Position );
		first->m_Meshes.back().m_Ops.Copy( (*m).m_Ops );
		first->m_Meshes.back().CopyNames( (*m) );
	}

	first->UpdateBox();
	return true;
}


bool SceneBuilder::VerifyConnectionOptions(
	SObject2 *first, SObject2 *second )
{
	if( first->m_DynamicList ) return false;
	if( second->m_DynamicList ) return false;
	return true;
}


//----------------------------------------------------


bool SceneBuilder::OptimizeRenderObjects2(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	int elimcount = 0;

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){
			
			object_progress += object_cost;
			BWindow.SetProgress( object_progress );

			SObject2 *obj = (SObject2*)(*i);
			if( obj->IsReference() )
				continue;

			if( obj->m_Meshes.size() <= 1 )
				continue;

			obj->TranslateToWorld();

			SObjMeshList tmpmeshes;

			while( !obj->m_Meshes.empty() ){

				Mesh *dest_mesh = 0;
				if( !tmpmeshes.empty() ){
					SObjMeshIt m = tmpmeshes.begin();
					for(;m!=tmpmeshes.end();m++){
						if( VerifyMeshConnect(&(*m),&obj->m_Meshes.front() ) ){
							dest_mesh = m->m_Mesh;
							break;
						}
					}
				}

				if( !dest_mesh ){
					tmpmeshes.push_back(SObject2Mesh());
					tmpmeshes.back().m_Mesh=new Mesh();
					tmpmeshes.back().m_Mesh->Copy( obj->m_Meshes.front().m_Mesh );
					tmpmeshes.back().m_Position.set( obj->m_Meshes.front().m_Position );
					tmpmeshes.back().m_Ops.Copy( obj->m_Meshes.front().m_Ops );
					tmpmeshes.back().CopyNames( obj->m_Meshes.front() );
				} else {
					elimcount++;
					dest_mesh->Append(
						obj->m_Meshes.front().m_Mesh );
				}

				obj->m_Meshes.pop_front();
			}

			while( !tmpmeshes.empty() ){

				obj->m_Meshes.push_back(SObject2Mesh());
				obj->m_Meshes.back().m_Mesh=new Mesh();
				obj->m_Meshes.back().m_Mesh->Copy( tmpmeshes.front().m_Mesh );
				obj->m_Meshes.back().m_Position.set( tmpmeshes.front().m_Position );
				obj->m_Meshes.back().m_Ops.Copy( tmpmeshes.front().m_Ops );
				obj->m_Meshes.back().CopyNames( tmpmeshes.front() );
				
				tmpmeshes.pop_front();

				Mesh *mesh = obj->m_Meshes.back().m_Mesh;
				while(1){
					bool try_collapse_more = false;

					for( int m0=0; m0<mesh->m_Materials.size(); m0++)
						for( int m1=m0+1; m1<mesh->m_Materials.size(); m1++)
							if( mesh->CompareMaterials( m0, m1 ) ){
								mesh->CollapseMaterials( m0, m1 );
								try_collapse_more = true;
								break;
							}

					if( !try_collapse_more )
						break;
				}
			}
		}
	}

	NConsole.print(
		"Building: %d mesh(es) eliminated with second stage...",
		elimcount );

	return true;
}


bool SceneBuilder::VerifyMeshConnect(
	SObject2Mesh *first, SObject2Mesh *second )
{
	if( first->m_Ops.m_TextureLighting != second->m_Ops.m_TextureLighting ) return false;
	if( first->m_Ops.m_VertexLighting != second->m_Ops.m_VertexLighting ) return false;
	if( first->m_Ops.m_Progressive != second->m_Ops.m_Progressive ) return false;
	if( first->m_Ops.m_OGF_specular != second->m_Ops.m_OGF_specular ) return false;
	if( first->m_Ops.m_OGF_light != second->m_Ops.m_OGF_light ) return false;
	if( first->m_Ops.m_OGF_alpha != second->m_Ops.m_OGF_alpha ) return false;
	if( first->m_Ops.m_OGF_fog != second->m_Ops.m_OGF_fog ) return false;
	if( first->m_Ops.m_OGF_ztest != second->m_Ops.m_OGF_ztest ) return false;
	if( first->m_Ops.m_OGF_zwrite != second->m_Ops.m_OGF_zwrite ) return false;

	// !!! FIXME !!!
	//  (more checks)

	return true;
}

//----------------------------------------------------
