#include "pch.h"
#pragma hdrstop

#include "Builder.h"
#include "SceneClassList.h"
#include "Light.h"
#include "BuildProgress.h"
#include "SObject2.h"
#include "Communicate.h"

#define vSpecular			1
#define vLight				2
#define vAlpha				4
#define vFog				8
#define vZTest				16
#define vZWrite				32

#define vStatic             vFog|vLight|vZTest

static HMODULE hStaticBuilder = 0;

typedef void __cdecl TStart (b_transfer *);
typedef void __cdecl TStop  (void);

void StartBuild(b_transfer *p)
{
    hStaticBuilder = LoadLibrary("xrLBuilder.dll");
    TStart* BStart = (TStart *)GetProcAddress(hStaticBuilder, "buildStart");
    _ASSERTE(BStart);
    BStart(p);
};
void WaitBuild(void)
{
    TStop* BStop = (TStop *)GetProcAddress(hStaticBuilder, "buildWaitComplete");
    _ASSERTE(BStop);
    BStop();
    FreeLibrary(hStaticBuilder);
};

void SetLight(b_light* b, Light* e)
{
    b->type          = e->m_D3D.type;
    b->diffuse.set   (e->m_Diffuse);
    b->specular.set  (e->m_Specular);
    b->ambient.set   (e->m_Ambient);
    b->position.set  (e->m_Position);
    b->direction.set (e->m_Direction);

    b->range          = e->m_Range;
    b->attenuation0   = e->m_Attenuation0;
    b->attenuation1   = e->m_Attenuation1;
    b->attenuation2   = e->m_Attenuation2;
    b->falloff        = e->m_D3D.falloff;
    b->theta          = e->m_D3D.theta;
    b->phi            = e->m_D3D.phi;
}

int FindInTextures(vector<b_texture>& t, DWORD* surface){
    for (DWORD i=0; i<t.size(); i++) if(t[i].pSurface==surface) return i;
    return -1;
}

int FindInMaterials( vector<b_material>& materials, b_material* m){
    for (DWORD i=0; i<materials.size(); i++){
        if( (materials[i].dwTexCount==m->dwTexCount) &&
            (materials[i].shader==m->shader) &&
            (memcmp(materials[i].surfidx,m->surfidx,sizeof(m->surfidx[0])*16)==0) &&
            (memcmp(materials[i].tclass,m->tclass,sizeof(m->tclass[0])*16)==0) &&
            (memcmp(materials[i].script,m->script,sizeof(m->script[0])*16)==0))return i;
    }
    return -1;
}

bool SceneBuilder::RemoteStaticBuild()
{
    VERIFY(0);

    b_transfer  TSData;
/*    if (bStaticRemoteInProgress) return false;

	int objcount = m_Scene->ObjectCount(OBJCLASS_NONE);
	if( objcount <= 0 )	return true;
	float object_cost = 100.f / (float)objcount;

// Build Options
	TSData.m_bTesselate         = Scene.m_bTesselate;
	TSData.m_bConvertProgressive= Scene.m_bConvertProgressive;
	TSData.m_bLightMaps         = Scene.m_bLightMaps;

	TSData.m_maxedge            = Scene.m_maxedge;
	TSData.m_VB_maxSize         = Scene.m_VB_maxSize*1024; // input in kB unit
	TSData.m_VB_maxVertices     = Scene.m_VB_maxVertices;
	TSData.m_pixels_per_meter   = Scene.m_pixels_per_meter;
	TSData.m_maxsize            = Scene.m_maxsize;
	TSData.m_relevance          = Scene.m_relevance;
	TSData.m_viewdist           = Scene.m_viewdist;
// lights
    int cur_light = 0;
	TSData.light_count  = m_Scene->ObjectCount(OBJCLASS_LIGHT);
    if (TSData.light_count){
    	TSData.lights   = new b_light[TSData.light_count];
    }
// shaders
    TSData.shader_count = 1;
	TSData.shaders      = new b_shader[TSData.shader_count];
    strcpy(TSData.shaders[0].name,"default");
// scripts
	TSData.script_count = 1;
	TSData.scripts      = new b_script[TSData.script_count];
    strcpy(TSData.scripts[0].script,"");
// materials & vertices & uvmap & faces
    vector<b_material>  materials;
    vector<b_vertex>    vertices;
    vector<b_face>      faces;
    vector<b_texture>   textures;

	ObjectIt i = m_Scene->FirstObj();
	for(;i!=m_Scene->LastObj();i++){
        switch((*i)->ClassID()){
        case OBJCLASS_LIGHT:
            SetLight(&(TSData.lights[cur_light]),(Light*)(*i));
            cur_light++;
            break;
        case OBJCLASS_SOBJECT2:{
            SObject2 *obj = (SObject2*)(*i);
            if (!obj->m_DynamicList){
                SObjMeshIt m2 = obj->m_Meshes.begin();
                for(;m2!=obj->m_Meshes.end();m2++){
                    // get mesh options
                    Mesh* mesh = (*m2)->m_Mesh;
                    {
                        int point_offs  = vertices.size();
                        //render mode
                        DWORD dwRMode = vStatic;
                        // add all points & uvmaps
                        // temporary
                        b_vertex vertex;
                        for(DWORD point_idx=0;point_idx<mesh->m_Points.size();point_idx++){
                            vertex.set(mesh->m_Points[point_idx].m_Point.x,
                                       mesh->m_Points[point_idx].m_Point.y,
                                       mesh->m_Points[point_idx].m_Point.z);
                            vertices.push_back(vertex);
                        }

                        // add all materials
                        for(DWORD mtl_idx=0;mtl_idx<mesh->m_Materials.size();mtl_idx++){
                            MMaterial* mm       = &(mesh->m_Materials[mtl_idx]);
                            b_material          new_m;
                            ZeroMemory(&new_m,sizeof(new_m));
                            // fill current material
                            ETextureCore* e_tex = mm->m_Texture->m_Ref;
                            int tex_idx = FindInTextures(textures, (DWORD*)e_tex->m_Pixels.begin());
                            // add used texture to save
                            AddUniqueTexName    ( mm->m_Texture->name() );
                            if (tex_idx<0){
                                b_texture       tex;
                                tex.dwWidth     = e_tex->m_Width;
                                tex.dwHeight    = e_tex->m_Height;
                                tex.bHasAlpha   = e_tex->m_AlphaPresent;
                                tex.pSurface    = (DWORD*)e_tex->m_Pixels.begin();
                                strcpy          (tex.name,mm->m_Texture->name());
                                textures.push_back(tex);
                                tex_idx         = textures.size()-1;
                            }
                            // set texture
                            new_m.dwTexCount    = 1;            // temporary
                            new_m.shader        = 0;            // temporary
                            new_m.tclass[0]     = TEXT2CLSID("STD"); // temporary
                            new_m.script[0]     = 0;            // temporary
                            new_m.surfidx[0]    = tex_idx;

                            int mtl_offs        = FindInMaterials(materials,&new_m);
                            if (mtl_offs==-1){
                                mtl_offs        = materials.size();
                                materials.push_back(new_m);
                            }
                            // fill faces data
                            for(DWORD face_idx=mm->m_FaceStart; face_idx<(mm->m_FaceStart+mm->m_FaceCount); face_idx++){
                                b_face          new_f;
                                new_f.dwRMode  = (textures[tex_idx].bHasAlpha)?(dwRMode|vAlpha):(dwRMode|vZWrite);
                                new_f.dwMaterial = mtl_offs;
                                new_f.v[0]     = point_offs+mesh->m_Faces[face_idx].p0;
                                new_f.v[1]     = point_offs+mesh->m_Faces[face_idx].p1;
                                new_f.v[2]     = point_offs+mesh->m_Faces[face_idx].p2;
                                // temporary
                                new_f.t[0][0].u = mesh->m_Points[mesh->m_Faces[face_idx].p0].m_TexCoord.x;
                                new_f.t[0][0].v = mesh->m_Points[mesh->m_Faces[face_idx].p0].m_TexCoord.y;
                                new_f.t[0][1].u = mesh->m_Points[mesh->m_Faces[face_idx].p1].m_TexCoord.x;
                                new_f.t[0][1].v = mesh->m_Points[mesh->m_Faces[face_idx].p1].m_TexCoord.y;
                                new_f.t[0][2].u = mesh->m_Points[mesh->m_Faces[face_idx].p2].m_TexCoord.x;
                                new_f.t[0][2].v = mesh->m_Points[mesh->m_Faces[face_idx].p2].m_TexCoord.y;
                                faces.push_back(new_f);
                            }
                        }// end material parse
                    }//
                }// end mesh parse
            }// if !dynamic
        }break; // end 'case OBJCLASS_SOBJECT2:'
        }// end switch
		frmBuildProgress->AddProgress( object_cost );
	}// end scene cycle

// materials
	TSData.mtl_count    = materials.size();
	TSData.material     = new b_material[TSData.mtl_count];
    CopyMemory(TSData.material,materials.begin(),sizeof(b_material)*TSData.mtl_count);

// vertices
	TSData.vertex_count = vertices.size();
	TSData.vertices     = new b_vertex[TSData.vertex_count];
    CopyMemory(TSData.vertices,vertices.begin(),sizeof(b_vertex)*TSData.vertex_count);

// faces
	TSData.face_count   = faces.size();
	TSData.faces        = new b_face[TSData.face_count];
    CopyMemory(TSData.faces,faces.begin(),sizeof(b_face)*TSData.face_count);
// textures
    TSData.tex_count    = textures.size();
    TSData.textures     = new b_texture[TSData.tex_count];
    CopyMemory(TSData.textures,textures.begin(),sizeof(b_texture)*TSData.tex_count);
*/

    StartBuild(&TSData);

    bStaticRemoteInProgress = true;

// static
    _DELETEARRAY(TSData.vertices);
    _DELETEARRAY(TSData.faces);
    _DELETEARRAY(TSData.material);
    _DELETEARRAY(TSData.shaders);
    _DELETEARRAY(TSData.scripts);
    _DELETEARRAY(TSData.textures);
    _DELETEARRAY(TSData.lights);
    return true;
}

bool SceneBuilder::RemoteStaticFinalize()
{
    if (bStaticRemoteInProgress) WaitBuild();
    bStaticRemoteInProgress = false;
    return true;
}

