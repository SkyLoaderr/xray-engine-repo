//----------------------------------------------------
// file: BuilderRModel.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "SObject2.h"
//----------------------------------------------------


#define MT_NORMAL              0
#define MT_HIERRARHY           1
#define MT_PROGRESSIVE         2
#define MT_PROGRESSIVE_FIXED   3
#define MT_SHADOW_FORM         4
#define MT_FIELD_ARROWS        5
#define MT_FIELD_WALLS         6

#define OGF_HEADER			1
#define OGF_TEXTURE1		2
#define OGF_TEXTURE2		3
#define OGF_MATERIAL		4
#define OGF_CHILDREN		5
#define OGF_BBOX			6
#define OGF_VERTICES		7
#define OGF_INDICES			8
#define OGF_P_MAP			9

#define vSpecular			1
#define vLight				2
#define vAlpha				4
#define vFog				8
#define vZTest				16
#define vZWrite				32

#pragma pack( push,1)
struct ogf_header {
	BYTE format_version;	// =2
	BYTE type;				// MT
	WORD flags;				// =0
};
struct ogf_bbox {
	IVector vmin;
	IVector vmax;
};
struct ogf_def_vertex {
	IVector pos;
	IVector norm;
	float texc[2];
};
struct ogf_def_lm_vertex {
	IVector pos;
	IVector norm;
	float texc1[2];
	float texc2[2];
};
struct ogf_lit_vertex {
	IVector pos;
	IVector norm;
	IColor color;
	float texc[2];
};
struct ogf_lit_lm_vertex {
	IVector pos;
	IVector norm;
	IColor color;
	float texc1[2];
	float texc2[2];
};
#pragma pack(pop)

//----------------------------------------------------


bool SceneBuilder::BuildRenderModel(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	char list_filename[MAX_PATH];
	strcpy( list_filename, Scene.m_FNMeshList );
	m_LevelPath.Update( list_filename );
	int listhandle = FS.create( list_filename );
	int file_index = 0;

	FS.writedword( listhandle, 0 );

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){
			SObject2 *obj = (SObject2*)(*i);

			if( !obj->IsReference() ){

				bool add_object = false;
				SObjMeshIt m2 = obj->m_Meshes.begin();
				for(;m2!=obj->m_Meshes.end();m2++)
					if(m2->m_Ops.m_StaticList){
						add_object = true;
						break;
					}

				if( add_object ){

					char filename[MAX_PATH];
					sprintf( filename, "%d.ogf", file_index );
					FS.writestring( listhandle, filename );

					obj->m_OGFCreated = true;
					strcpy( obj->m_OGFName, filename );

					StaticListItem li;
					li.m_Def = obj;
					li.m_FileIndex = file_index;
					strcpy( li.m_OGF_Name,filename );
					m_Statics.push_back( li );

					m_LevelPath.Update( filename );
					int handle = FS.create( filename );

					if( !BuildObjectOGF( handle, file_index, obj ) ){
						FS.close( handle );
						FS.close( listhandle );
						NConsole.print( "BuildRenderModel(): can't build VCN..." );
						return false;
					}

					FS.close( handle );
					file_index ++;
				}
			}

			object_progress += object_cost;
			BWindow.SetProgress( object_progress );
		}
	}


	//int namefill	= 0;
	//char filename[MAX_PATH];

	char *p			= m_Scene->m_BOPAddFiles;
	char *name		= p;

	while( true ){
		if ((*p == '\r') || (*p == '\n') || !*p){

			char c = *p;	*p = 0;

			if (strlen(name))
				FS.writestring( listhandle, name );

			*p = c;
			while ((*p == '\r') || (*p == '\n')){
				p++;
			}

			name = p;		file_index ++;

			if (!*p) break;

		}
		else p++;
	}
	/*
		filename[namefill] = *p;
		if( *p == '\r' || *p == '\n' ){
			while( *p && (*p == '\r' || *p == '\n')) p++;
			filename[namefill] = 0;
			if( namefill > 0 ){
				file_index++;
				FS.writestring( listhandle, filename );
				namefill = 0;
			}
		} if( *(p+1) == 0 ) {
			filename[namefill] = 0;
			if( namefill > 0 ){
				file_index++;
				FS.writestring( listhandle, filename );
				break;
			}
		} else {
			namefill++;
			p++;
		}
	}*/
		
	FS.seek( listhandle, 0 );
	FS.writedword( listhandle, file_index );

	FS.close( listhandle );

	return true;
}


bool SceneBuilder::BuildObjectOGF( int handle, int file_index, SObject2 *obj ){
	
	if( obj->m_Meshes.size() <= 0 )
		return false;

	if( obj->m_Meshes.size() == 1 ){
		Mesh *mesh = obj->m_Meshes.front().m_Mesh;
		if( mesh->m_Materials.size() == 0 )
			return false;
		// no need to build hierarhy
		if( mesh->m_Materials.size() == 1 ){
			Mesh *submesh = new Mesh();
			submesh->Copy( mesh );
			submesh->Transform( obj->m_Meshes.front().m_Position );
			submesh->Transform( obj->m_Position );
			if( !BuildSingleOGF( handle, submesh, & obj->m_Meshes.front().m_Ops )){
				delete submesh;
				return false;
			}
			delete submesh;
			return true;
		}
	}

	// collect subs

	int sub_index = 0;
	SSTRLIST children;

	SObjMeshIt i = obj->m_Meshes.begin();
	for(;i!=obj->m_Meshes.end();i++){
		if( i->m_Ops.m_StaticList ){
			Mesh *mesh = i->m_Mesh;
			for(int j=0;j<mesh->m_Materials.size();j++){

				Mesh *submesh = new Mesh();
				mesh->GetMaterial( submesh, mesh->m_Materials.begin() + j );
				submesh->Transform( i->m_Position );
				submesh->Transform( obj->m_Position );

				char subfilename[MAX_PATH];
				sprintf( subfilename, "%d#%d.ogf", file_index, sub_index );

				children.push_back( SSTR(subfilename) );

				m_LevelPath.Update( subfilename );
				int handle2 = FS.create( subfilename );

				if( !BuildSingleOGF( handle2, submesh, & i->m_Ops ) ){
					FS.close( handle2 );
					delete submesh;
					return false;
				}

				FS.close( handle2 );
				delete submesh;
				sub_index++;
			}
		}
	}

	// write root OGF

	ogf_header header;
	header.format_version = 2;
	header.type = MT_HIERRARHY;
	header.flags = 0;

	FSChunkDef ckheader,cklist,ck;
	FS.initchunk( &ck, handle );
	FS.initchunk( &ckheader, handle );
	FS.initchunk( &cklist, handle );

	FS.w2openchunk( &ckheader, OGF_HEADER );
	FS.write( handle, &header, sizeof(header) );
	FS.w2closechunk( &ckheader );

	FS.w2openchunk( &cklist, OGF_CHILDREN );
	FS.writedword( handle, children.size() );
	for( int k=0; k<children.size(); k++)
		FS.writestring( handle, children[k].filename );
	FS.w2closechunk( &cklist );

	IAABox box;
	obj->GetBox( box );
	FS.w2openchunk(&ck,OGF_BBOX);
	FS.write( handle, &box, sizeof(box) );
	FS.w2closechunk( &ck );

	return true;
}


//----------------------------------------------------



bool SceneBuilder::BuildSingleOGF( int handle, Mesh *mesh, ObjectOptionPack *ops ){

	_ASSERTE( mesh );
	_ASSERTE( mesh->m_Materials.size() == 1 );

	FSChunkDef ck;
	FS.initchunk( &ck, handle );

	// header

	ogf_header header;
	header.format_version = 2;
	header.type = MT_NORMAL;
	header.flags = 0;
	
	if( ops->m_OGF_alpha )
		header.flags |= vAlpha;
	if( ops->m_OGF_fog )
		header.flags |= vFog;
	if( ops->m_OGF_light )
		header.flags |= vLight;
	if( ops->m_OGF_specular )
		header.flags |= vSpecular;
	if( ops->m_OGF_ztest )
		header.flags |= vZTest;
	if( ops->m_OGF_zwrite )
		header.flags |= vZWrite;

	FS.w2openchunk(&ck,OGF_HEADER);
	FS.write( handle, &header, sizeof(header) );
	FS.w2closechunk( &ck );

	// bounding box

	IAABox box;
	mesh->GetBox( box );
	FS.w2openchunk(&ck,OGF_BBOX);
	FS.write( handle, &box, sizeof(box) );
	FS.w2closechunk( &ck );

	// point list

	FS.w2openchunk(&ck,OGF_VERTICES);
	if( ops->m_VertexLighting ){
		if( ops->m_TextureLighting && m_Scene->m_BOPLightMaps ){

			_ASSERTE( !mesh->m_LMap.empty() );
			_ASSERTE( !mesh->m_Lighting.empty() );
			FS.writedword( handle, 0x002 | 0x010 | 0x040 | 0x200 );
			FS.writedword( handle, mesh->m_Points.size() );
			for( int i=0; i<mesh->m_Points.size(); i++){
				ogf_lit_lm_vertex llvertex;
				llvertex.pos.set( mesh->m_Points[i].m_Point );
				llvertex.norm.set( mesh->m_Points[i].m_Normal );
				llvertex.color.set( mesh->m_Lighting[i].m_Diffuse );
				llvertex.texc1[0] = mesh->m_Points[i].m_TexCoord.x;
				llvertex.texc1[1] = mesh->m_Points[i].m_TexCoord.y;
				llvertex.texc2[0] = mesh->m_LMap[i].u;
				llvertex.texc2[1] = mesh->m_LMap[i].v;
				FS.write( handle, &llvertex, sizeof(llvertex) );
			}

		} else {

			_ASSERTE( !mesh->m_Lighting.empty() );
			FS.writedword( handle, 0x002 | 0x010 | 0x040 | 0x100 );
			FS.writedword( handle, mesh->m_Points.size() );
			for( int i=0; i<mesh->m_Points.size(); i++){
				ogf_lit_vertex lvertex;
				lvertex.pos.set( mesh->m_Points[i].m_Point );
				lvertex.norm.set( mesh->m_Points[i].m_Normal );
				lvertex.color.set( mesh->m_Lighting[i].m_Diffuse );
				lvertex.texc[0] = mesh->m_Points[i].m_TexCoord.x;
				lvertex.texc[1] = mesh->m_Points[i].m_TexCoord.y;
				FS.write( handle, &lvertex, sizeof(lvertex) );
			}

		}
	} else {
		if( ops->m_TextureLighting && m_Scene->m_BOPLightMaps ){

			_ASSERTE( !mesh->m_LMap.empty() );
			FS.writedword( handle, 0x002 | 0x010 | 0x200 );
			FS.writedword( handle, mesh->m_Points.size() );
			for( int i=0; i<mesh->m_Points.size(); i++){
				ogf_def_lm_vertex lmvertex;
				lmvertex.pos.set( mesh->m_Points[i].m_Point );
				lmvertex.norm.set( mesh->m_Points[i].m_Normal );
				lmvertex.texc1[0] = mesh->m_Points[i].m_TexCoord.x;
				lmvertex.texc1[1] = mesh->m_Points[i].m_TexCoord.y;
				lmvertex.texc2[0] = mesh->m_LMap[i].u;
				lmvertex.texc2[1] = mesh->m_LMap[i].v;
				FS.write( handle, &lmvertex, sizeof(lmvertex) );
			}

		} else {

			FS.writedword( handle, 0x002 | 0x010 | 0x100 );
			FS.writedword( handle, mesh->m_Points.size() );
			for( int i=0; i<mesh->m_Points.size(); i++){
				ogf_def_vertex vertex;
				vertex.pos.set( mesh->m_Points[i].m_Point );
				vertex.norm.set( mesh->m_Points[i].m_Normal );
				vertex.texc[0] = mesh->m_Points[i].m_TexCoord.x;
				vertex.texc[1] = mesh->m_Points[i].m_TexCoord.y;
				FS.write( handle, &vertex, sizeof(vertex) );
			}

		}
	}
	FS.w2closechunk( &ck );

	// faces
	
	FS.w2openchunk(&ck,OGF_INDICES);
	FS.writedword( handle, mesh->m_Faces.size()*3 );
	for( int i=0; i<mesh->m_Faces.size(); i++)
		for( int k=0; k<3; k++)
			FS.writeword( handle, mesh->m_Faces[i].p[k] );
	FS.w2closechunk( &ck );

	// material

	MMaterial *material = mesh->m_Materials.begin();
	_ASSERTE( material );

	FS.w2openchunk(&ck,OGF_MATERIAL);
	FS.write( handle, &material->m_Ambient, sizeof(material->m_Ambient) );
	FS.write( handle, &material->m_Diffuse, sizeof(material->m_Diffuse) );
	FS.write( handle, &material->m_Specular, sizeof(material->m_Specular) );
	FS.write( handle, &material->m_Emission, sizeof(material->m_Emission) );
	FS.write( handle, &material->m_Power, sizeof(material->m_Power) );
	FS.w2closechunk( &ck );

	// base texture

	FS.w2openchunk(&ck,OGF_TEXTURE1);
	
	char texname[MAX_PATH]="null";
	char shadername[MAX_PATH]="default";

	if( material->m_Texture && 
		material->m_Texture->name() &&
		material->m_Texture->name()[0] )
	{
		_splitpath( material->m_Texture->name(), 0, 0, texname, 0 );
		AddUniqueTexName( material->m_Texture->name() );
	}

	if( ops->m_TextureLighting && m_Scene->m_BOPLightMaps ){

		_ASSERTE( material->m_LightMap );
		_ASSERTE( material->m_LightMap->name() );
		_ASSERTE( material->m_LightMap->name()[0] );
		_ASSERTE( !mesh->m_LMap.empty() );

		strcpy( shadername, "lmap" );
		strcat(texname,",");
		_splitpath( material->m_LightMap->name(), 0, 0, texname+strlen(texname), 0 );
		AddUniqueTexName( material->m_LightMap->name() );
	}

	FS.writestring(handle, texname );
	FS.writestring(handle, shadername );

	FS.w2closechunk( &ck );
	
	return true;
}
