//----------------------------------------------------
// file: BuilderLight.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "Builder.h"
#include "UI_Main.h"

#include "Scene.h"
#include "SceneClassList.h"
#include "SObject2.h"
#include "Light.h"
//----------------------------------------------------



bool SceneBuilder::BuildLightModel(){

	int objcount = m_Scene->ObjectCount(OBJCLASS_LIGHT) + 1;
	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	char lfilename[MAX_PATH];
	strcpy( lfilename, m_Scene->m_FNLights );
	m_LevelPath.Update( lfilename );
	int handle = FS.create( lfilename );

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++)
		if((*i)->ClassID()==OBJCLASS_LIGHT){

			Light *l = (Light*)(*i);
			l->FillD3DParams();
			FS.write( handle, &l->m_D3D, sizeof(l->m_D3D));

			object_progress += object_cost;
			BWindow.SetProgress( object_progress );
		}

	D3DLIGHT7 defaultlight;
	m_Scene->FillDefaultLight( &defaultlight );
	FS.write( handle, &defaultlight, sizeof(defaultlight));

	FS.close(handle);

	return true;
}


//----------------------------------------------------


bool SceneBuilder::VertexLighting(){

	// -- update boxes --
	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++)
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){
			SObject2 *obj = (SObject2*)(*i);
			if( !obj->IsReference() )
				obj->UpdateBox();
		}

	// -- calculate colors --

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;

	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++)
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){

			SObject2 *obj = (SObject2*)(*i);
			if( obj->IsReference() )
				continue;

			SObjMeshIt itm = obj->m_Meshes.begin();
			for(;itm!=obj->m_Meshes.end();itm++){

				if( !itm->m_Ops.m_VertexLighting )
					continue;

				Mesh *mesh = itm->m_Mesh;
				_ASSERTE( mesh );

				IMatrix matrix;
				matrix.set( obj->m_Position );
				matrix.mul2( itm->m_Position );

				mesh->m_Lighting.resize( mesh->m_Points.size() );
				for(int j=0; j<mesh->m_Lighting.size(); j++)
					mesh->m_Lighting[j].m_Valid = false;

				for( j=0; j<mesh->m_Materials.size(); j++)
					for( int k=0; k < mesh->m_Materials[j].m_FaceCount; k++){

						IFColor rescolor;
						IVector realpos;
						IVector realnorm;

						POINT_LIGHT_PARAM lightparam;
						lightparam.diffuse = & mesh->m_Materials[j].m_Diffuse;
						lightparam.ambient = & mesh->m_Materials[j].m_Ambient;
						lightparam.emission = & mesh->m_Materials[j].m_Emission;
						lightparam.rescolor = & rescolor;
						lightparam.position = &realpos;
						lightparam.normal = &realnorm;

						int faceindex = k+mesh->m_Materials[j].m_FaceStart;
						for( int kface = 0; kface<3; kface++){
							int ptindex = mesh->m_Faces[faceindex].p[kface];
							if( !mesh->m_Lighting[ptindex].m_Valid ){

								matrix.transform( realpos, mesh->m_Points[ptindex].m_Point );
								matrix.shorttransform( realnorm, mesh->m_Points[ptindex].m_Normal );

								PointLighting( &lightparam );

								clamp( rescolor.r, 0, 1.f );
								clamp( rescolor.g, 0, 1.f );
								clamp( rescolor.b, 0, 1.f );

								mesh->m_Lighting[ptindex].m_Diffuse.set(
									rescolor.r * 255.f,
									rescolor.g * 255.f,
									rescolor.b * 255.f );

								mesh->m_Lighting[ptindex].m_Valid = true;
								mesh->m_Lighting[ptindex].m_Specular.set( 0, 0, 0 );
							}
						}
					}

				for(j=0; j<mesh->m_Lighting.size(); j++)
					if( !mesh->m_Lighting[j].m_Valid ){
						NConsole.print( "Warning: There are unlit points in %s ...", itm->m_Name );
						break;
					}
			}

			object_progress += object_cost;
			BWindow.SetProgress( object_progress );
		}

	return true;
}


void SceneBuilder::PointLighting( POINT_LIGHT_PARAM *param )
{
	_ASSERTE( param );
	_ASSERTE( param->position );
	_ASSERTE( param->normal );
	_ASSERTE( param->ambient );
	_ASSERTE( param->diffuse );
	_ASSERTE( param->emission );
	_ASSERTE( param->rescolor );

	IFColor kcolor,kambient;
	kcolor.set( 0, 0, 0 );
	kambient.set( 0, 0, 0 );

	// add default light (sun)

	kambient.r += m_Scene->m_GlobalAmbient.r;
	kambient.g += m_Scene->m_GlobalAmbient.g;
	kambient.b += m_Scene->m_GlobalAmbient.b;

	float klight = 0;
	IVector fakepoint;
	fakepoint.set( (*param->position) );
	fakepoint.translate( m_Scene->m_GlobalLightDirection, -5000 );

	LIGHT_PICK_PARAM pickparam;
	pickparam.point = param->position;
	pickparam.light = &fakepoint;
	pickparam.pickdist = &klight;

	if( !LightPick( &pickparam ) ){
		IVector L;
		L.inverse( m_Scene->m_GlobalLightDirection );
		float dotp = L.dot( (*param->normal) );
		if( dotp > 0 ){
			kcolor.r += dotp * m_Scene->m_GlobalDiffuse.r;
			kcolor.g += dotp * m_Scene->m_GlobalDiffuse.g;
			kcolor.b += dotp * m_Scene->m_GlobalDiffuse.b;
		}
	}

	// process scene lights

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++)
		if((*i)->ClassID()==OBJCLASS_LIGHT){

			Light *light = (Light*)(*i);

			kambient.r += light->m_Ambient.r;
			kambient.g += light->m_Ambient.g;
			kambient.b += light->m_Ambient.b;

			if( light->m_Directional ){

				IVector L;
				L.inverse( light->m_Direction );
				float dotp = L.dot( (*param->normal ) );
				if( dotp > 0 ){
					kcolor.r += dotp * light->m_Diffuse.r;
					kcolor.g += dotp * light->m_Diffuse.g;
					kcolor.b += dotp * light->m_Diffuse.b;
				}

			} else {

				if( light->m_Attenuation0 <= 0.f &&
					light->m_Attenuation1 <= 0.f &&
					light->m_Attenuation2 <= 0.f )
				{
					NConsole.print( "Warning: invalid light encountered..." );
					break;
				}

				pickparam.point = param->position;
				pickparam.light = &light->m_Position;
				pickparam.pickdist = &klight;

				if( !LightPick( &pickparam ) ){
					IVector L;
					L.sub( light->m_Position, (*param->position) );
					float d = L.lenght();
					if( d < light->m_Range ){
						L.safe_normalize();
						float att = 1 / (
							light->m_Attenuation0 +
							light->m_Attenuation1 * d +
							light->m_Attenuation2 * d * d  );
						float dotp = L.dot( (*param->normal) );
						if( dotp > 0 ){
							kcolor.r += att * dotp * light->m_Diffuse.r;
							kcolor.g += att * dotp * light->m_Diffuse.g;
							kcolor.b += att * dotp * light->m_Diffuse.b;
						}
					}
				}
			}
		}

	param->rescolor->set(
		(kcolor.r * param->diffuse->r + param->emission->r + param->ambient->r * kambient.r ),
		(kcolor.g * param->diffuse->g + param->emission->g + param->ambient->g * kambient.g ),
		(kcolor.b * param->diffuse->b + param->emission->b + param->ambient->b * kambient.b ) );
}

//----------------------------------------------------

bool SceneBuilder::TextureLighting(){

	// -- update boxes --
	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++)
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){
			SObject2 *obj = (SObject2*)(*i);
			if( !obj->IsReference() )
				obj->UpdateBox();
		}

	// -- add maps --

	int objcount = m_Scene->ObjectCount(OBJCLASS_SOBJECT2);
	if( objcount <= 0 )
		return true;
	
	float object_cost = 100.f / (float)objcount;
	float object_progress = 0.f;

	int lmap_index = 0;

	i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++)
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){

			SObject2 *obj = (SObject2*)(*i);
			if( obj->IsReference() )
				continue;

			SObjMeshIt itm = obj->m_Meshes.begin();
			for(;itm!=obj->m_Meshes.end();itm++){

				if( !itm->m_Ops.m_TextureLighting )
					continue;

				Mesh *mesh = itm->m_Mesh;
				_ASSERTE( mesh );

				if( mesh->m_LMap.size() != mesh->m_Points.size() ){
					mesh->m_LMap.resize( mesh->m_Points.size() );
					for(int j=0; j<mesh->m_LMap.size(); j++){
						mesh->m_LMap[j].m_Valid = true;
						mesh->m_LMap[j].u = mesh->m_Points[j].m_TexCoord.x;
						mesh->m_LMap[j].v = mesh->m_Points[j].m_TexCoord.y;
					}
				}

				IMatrix matrix;
				matrix.set( obj->m_Position );
				matrix.mul2( itm->m_Position );

				for( int j=0; j<mesh->m_Materials.size(); j++){

					MMaterial *material = mesh->m_Materials.begin() + j;

					IFColor rescolor;
					POINT_LIGHT_PARAM lparam;
					lparam.ambient = & material->m_Ambient;
					lparam.diffuse = & material->m_Diffuse;
					lparam.emission = & material->m_Emission;
					lparam.rescolor = & rescolor;
					
					// -- gen name --
					char tnamebuf[MAX_PATH];
					sprintf( tnamebuf, "light#%s#%d.bmp", m_Scene->m_BOPLMapPrefix, lmap_index );
					lmap_index++;

					// -- create texture --
					material->m_LightMap = new ETexture( LMAP_W, LMAP_H, tnamebuf );
					
					_ASSERTE( material->m_LightMap->m_Ref );
					ETextureCore *map = material->m_LightMap->m_Ref;

					// -------------
					memset( map->m_Pixels.begin(), 0x7f, sizeof(IColor) * 
						map->m_Pixels.size() );

					float du = 1.f / (float)LMAP_W;
					float dv = 1.f / (float)LMAP_H;
					float du1 = 1.f / (float)(LMAP_W+1);
					float dv1 = 1.f / (float)(LMAP_H+1);

					// -- per face cycle (adding light) --
					for( int _face=0; _face<material->m_FaceCount; _face++){

						IVector pts[3];
						IVector normals[3];
						IVector texc[3];

						MFace *face = mesh->m_Faces.begin() +
							_face + material->m_FaceStart;
						for( int k=0; k<3; k++){
							MPoint *pt = mesh->m_Points.begin() + face->p[k];
							matrix.transform( pts[k], pt->m_Point );
							matrix.shorttransform( normals[k], pt->m_Normal );
							texc[k].set(
								mesh->m_LMap[face->p[k]].u,
								mesh->m_LMap[face->p[k]].v,
								0 );
						}

						float _umin = min( min( texc[0].x, texc[1].x ), texc[2].x );
						float _vmin = min( min( texc[0].y, texc[1].y ), texc[2].y );
						float _umax = max( max( texc[0].x, texc[1].x ), texc[2].x );
						float _vmax = max( max( texc[0].y, texc[1].y ), texc[2].y );

						while( _umin < 0.f ){
							_umin += 1.f;
							_umax += 1.f;
						}

						while( _vmin < 0.f ){
							_vmin += 1.f;
							_vmax += 1.f;
						}

						if( _vmax > 1.f || _umax > 1.f ){
							NConsole.print( "Face too big ..." );
							continue;
						}

						IMatrix Tm;
						Tm.identity();
						Tm.i.sub( texc[1], texc[0] );
						Tm.j.sub( texc[2], texc[0] );
						Tm.k.cross( Tm.j, Tm.i );
						Tm.c.set( texc[0] );

						IMatrix Pm;
						Pm.identity();
						Pm.i.sub( pts[1], pts[0] );
						Pm.j.sub( pts[2], pts[0] );
						Pm.k.cross( Pm.j, Pm.i );
						Pm.c.set( pts[0] );

						IMatrix TmI;
						TmI.invert( Tm );

						int _upixmin = floorf( _umin * LMAP_W ) - 1;
						int _vpixmin = floorf( _vmin * LMAP_H ) - 1;
						int _upixmax = floorf( _umax * LMAP_W ) + 1;
						int _vpixmax = floorf( _vmax * LMAP_H ) + 1;

						clamp( _upixmin, 0, (LMAP_W-1) );
						clamp( _upixmax, 0, (LMAP_W-1) );
						clamp( _vpixmin, 0, (LMAP_H-1) );
						clamp( _vpixmax, 0, (LMAP_H-1) );

						for( int _upix=_upixmin; _upix<=_upixmax; _upix++){
							for( int _vpix=_vpixmin; _vpix<=_vpixmax; _vpix++){

								IVector uvpoint;
								uvpoint.set( (_upix+0.5f) * du, (_vpix+0.5f) * dv, 0 );
								TmI.transform( uvpoint );

								if( uvpoint.x < (-0.01f) || uvpoint.y < (-0.01f) )
									continue;
								if( (uvpoint.x + uvpoint.y) > 1.01f )
									continue;

								float k0 = 0.707107f * (1.f - (uvpoint.x + uvpoint.y));
								float k1 = uvpoint.x;
								float k2 = uvpoint.y;

								IVector unormal;
								unormal.set( 0, 0, 0 );

								unormal.translate( normals[0], k0 );
								unormal.translate( normals[1], k1 );
								unormal.translate( normals[2], k2 );
								unormal.safe_normalize();

								Pm.transform( uvpoint );

								lparam.position = & uvpoint;
								lparam.normal = & unormal;
								PointLighting( &lparam );

								IColor *pixel = map->m_Pixels.begin()+
									(_vpix*LMAP_W) + _upix;

								clamp( rescolor.r, 0, 0.98f );
								clamp( rescolor.g, 0, 0.98f );
								clamp( rescolor.b, 0, 0.98f );

								rescolor.r += rndfltn() * 0.005f;
								rescolor.g += rndfltn() * 0.005f;
								rescolor.b += rndfltn() * 0.005f;

								clamp( rescolor.r, 0, 1.f );
								clamp( rescolor.g, 0, 1.f );
								clamp( rescolor.b, 0, 1.f );

								pixel->set(
									rescolor.r * 255.f,
									rescolor.g * 255.f,
									rescolor.b * 255.f );
							}
						}

					}

					material->m_LightMap->DDReinit();
				}

				// -- upgrade lightmap texcoords to remove
				//     filtering shift --

				float _halfpixel_u = 0.5f / (float)LMAP_W;
				float _halfpixel_v = 0.5f / (float)LMAP_H;

				vector<MLMPoint> newlmap;
				newlmap.insert(newlmap.end(),mesh->m_LMap.begin(),mesh->m_LMap.end());

				vector<bool> vmap2;
				vmap2.resize( mesh->m_LMap.size(), false );

				for( int _point=0; _point<newlmap.size(); _point++){
					if( !vmap2[_point] ){
						vmap2[_point] = true;

						bool is_min_u = true;
						bool is_min_v = true;
						bool is_max_u = true;
						bool is_max_v = true;

						for( int _face=0; _face<mesh->m_Faces.size(); _face++)
							for( int k=0; k<3; k++)
								if( mesh->m_Faces[_face].p[k] == _point ){

									int _point1 = mesh->m_Faces[_face].p[(k+1)%3];
									int _point2 = mesh->m_Faces[_face].p[(k+2)%3];

									if( mesh->m_LMap[_point1].u < mesh->m_LMap[_point].u )
										is_min_u = false;
									if( mesh->m_LMap[_point1].v < mesh->m_LMap[_point].v )
										is_min_v = false;

									if( mesh->m_LMap[_point2].u < mesh->m_LMap[_point].u )
										is_min_u = false;
									if( mesh->m_LMap[_point2].v < mesh->m_LMap[_point].v )
										is_min_v = false;

									if( mesh->m_LMap[_point1].u > mesh->m_LMap[_point].u )
										is_max_u = false;
									if( mesh->m_LMap[_point1].v > mesh->m_LMap[_point].v )
										is_max_v = false;
										
									if( mesh->m_LMap[_point2].u > mesh->m_LMap[_point].u )
										is_max_u = false;
									if( mesh->m_LMap[_point2].v > mesh->m_LMap[_point].v )
										is_max_v = false;
								}

						if( is_min_u )
							newlmap[_point].u += _halfpixel_u;
						if( is_min_v )
							newlmap[_point].v += _halfpixel_v;
						if( is_max_u )
							newlmap[_point].u -= _halfpixel_u;
						if( is_max_v )
							newlmap[_point].v -= _halfpixel_v;

					}
				}

				mesh->m_LMap.clear();
				mesh->m_LMap.insert(mesh->m_LMap.end(),newlmap.begin(),newlmap.end());

				newlmap.clear();
				vmap2.clear();
			}

			object_progress += object_cost;
			BWindow.SetProgress( object_progress );
		}

	return true;
}




bool SceneBuilder::LightPick( LIGHT_PICK_PARAM *param ){

	_ASSERTE( param );
	_ASSERTE( param->light );
	_ASSERTE( param->pickdist );
	_ASSERTE( param->point );

	IVector ray;
	ray.sub( (*param->light), (*param->point) );

	float dmax = ray.lenght();
	if( dmax <= 0.001f )
		return false;

	ray.mul( 1.f / dmax );

	IVector start;
	start.set( (*param->point) );
	start.translate( ray, 0.01f );

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++)
		if((*i)->ClassID()==OBJCLASS_SOBJECT2){

			SObject2 *obj = (SObject2*)(*i);
			if( obj->IsReference() )
				continue;

			IAABox box;
			if( !obj->GetBox( box ) )
				continue;
			if( !box.Pick( start, ray ) )
				continue;

			SObjMeshIt itm = obj->m_Meshes.begin();
			for(;itm!=obj->m_Meshes.end();itm++){

				if( !itm->m_Ops.m_CastShadows )
					continue;

				Mesh *mesh = itm->m_Mesh;
				_ASSERTE( mesh );

				IMatrix matrix;
				matrix.set( obj->m_Position );
				matrix.mul2( itm->m_Position );

				for( int _face = 0; _face<mesh->m_Faces.size(); _face++){

					IVector facepoints[3];
					for( int k=0; k<3; k++)
						matrix.transform( facepoints[k], 
							mesh->m_Points[mesh->m_Faces[_face].p[k]].m_Point );

					IVector facenormal;
					facenormal.mknormal(
						facepoints[0],
						facepoints[1],
						facepoints[2] );

					if( PickFace(
						param->pickdist,
						start, ray,
						facenormal,
						facepoints[0],
						facepoints[1],
						facepoints[2] ) )
					{
						if( (*param->pickdist) <= dmax )
							return true;
					}
				}


			}
		}
	
	
	return false;
}



