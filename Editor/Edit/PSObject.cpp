//----------------------------------------------------
// file: PSObject.cpp
//----------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "Log.h"
#include "PSObject.h"
#include "ParticleSystem.h"
#include "ui_main.h"
#include "d3dutils.h"
#include "PSLibrary.h"
#include "tlsprite.h"

#define CPSOBJECT_VERSION  				0x0010
//----------------------------------------------------
#define CPSOBJECT_CHUNK_VERSION			0xE110
#define CPSOBJECT_CHUNK_EMITTER			0xE111
#define CPSOBJECT_CHUNK_REFERENCE		0xE112
//----------------------------------------------------

//using namespace PS;

CPSObject::CPSObject( char *name ):SceneObject(){
	Construct();
	strcpy( m_Name, name );
}
//----------------------------------------------------

CPSObject::CPSObject():SceneObject(){
	Construct();
}
//----------------------------------------------------

void CPSObject::Construct(){
	m_ClassID   = OBJCLASS_PS;
	m_Emitter.Reset();
    m_Emitter.m_ConeDirection.set(0.f,1.f,0.f);
    m_Emitter.m_ConeAngle = 1.f;
    m_Shader	= 0;
    m_Particles.clear();
    m_fEmissionResidue = 0;
	m_Definition= 0;
}
//----------------------------------------------------

CPSObject::~CPSObject(){
	if (m_Shader) UI->Device.Shader.Delete(m_Shader);
}
//----------------------------------------------------

bool CPSObject::GetBox( Fbox& box ){
	box.set( m_Emitter.m_Position, m_Emitter.m_Position );
	box.min.x -= PSOBJECT_SIZE;
	box.min.y -= PSOBJECT_SIZE;
	box.min.z -= PSOBJECT_SIZE;
	box.max.x += PSOBJECT_SIZE;
	box.max.y += PSOBJECT_SIZE;
	box.max.z += PSOBJECT_SIZE;
    return true;
}
//----------------------------------------------------

//////////////////////////////////////////////////
// - доделать рождение/умирание без удаления и добавления в массив
//
void CPSObject::RTL_Update(float dT){
	float fTime = UI->Device.m_fTimeGlobal;

	// update all particles that we own
    for (int i=0; i<int(m_Particles.size()); i++){
    	SParticle& P = m_Particles[i];
 		if (fTime>P.m_Time.end){
        	m_Particles.erase(m_Particles.begin()+i);
            i--;
        }
    }

	// calculate how many particles we should create from ParticlesPerSec and time elapsed taking the
	int iParticlesCreated = m_Emitter.CalculateBirth(m_Particles.size(),fTime,dT);

    if (iParticlesCreated){
        float TM	 	= fTime-dT;
        float dT_delta 	= dT/iParticlesCreated;
        // see if actually have any to create
        for (i=0; i<iParticlesCreated; i++, TM+=dT_delta){
            m_Particles.push_back(SParticle());
            SParticle& P = m_Particles.back();

            m_Emitter.GenerateParticle(P,m_Definition,TM);
        }
    }
}

//this draws the particle system in the ogl window
void CPSObject::DrawPS(){
	float fTime = UI->Device.m_fTimeGlobal;
    if (m_Shader) 	UI->Device.Shader.Set(m_Shader);
    else			UI->Device.Shader.Set(UI->Device.m_WireShader);

    CTLSprite m_Sprite;
    int 	mb_samples 	= 1;
    float 	mb_step 	= 0;

	for (PS::ParticleIt P=m_Particles.begin(); P!=m_Particles.end(); P++){
		DWORD 	C;
		float 	sz, angl;
		float 	angle, ang_vel;
        if (m_Definition->m_dwFlag&PS_MOTIONBLUR){
            float T 	= fTime-P->m_Time.start;
            float k 	= T/(P->m_Time.end-P->m_Time.start);
            float k_inv = 1-k;
            mb_samples 	= iFloor(m_Definition->m_BlurSubdiv.start*k_inv+m_Definition->m_BlurSubdiv.end*k+0.5f);
            mb_step 	= m_Definition->m_BlurTime.start*k_inv+m_Definition->m_BlurTime.end*k;
            mb_step /= mb_samples;
        }
        // update
		for (int sample=mb_samples-1; sample>=0; sample--){
            float T 	= fTime-P->m_Time.start-(sample*mb_step);
            if (T<0) continue;
            float mb_v	= 1-float(sample)/float(mb_samples);
            float k 	= T/(P->m_Time.end-P->m_Time.start);
			if ((m_Emitter.m_dwFlag&PS_EM_PLAY_ONCE) && (k>1)) continue;
            float k_inv = 1-k;

            Fvector Pos;
            // this moves the particle using the last known velocity and the time that has passed
            PS::SimulatePosition(Pos,P,T,k);
            // adjust current Color from calculated Deltas and time elapsed.
            PS::SimulateColor(C,P,k,k_inv,mb_v);
            // adjust current Size & Angle
            PS::SimulateSize(sz,P,k,k_inv);

            Fvector D;
			if (m_Definition->m_dwFlag&PS_ALIGNTOPATH){
				Fvector p;
                float PT = T-0.1f;
	            float kk = PT/(P->m_Time.end-P->m_Time.start);
                PS::SimulatePosition(p,P,PT,kk);
				D.sub(Pos,p);
                D.normalize_safe();
	        }else{
            	PS::SimulateAngle(angle,P,T,k,k_inv);
            }

			// Animation
            Fvector2 lt,rb;
            lt.set(0.f,0.f);
            rb.set(1.f,1.f);
	        if (m_Definition->m_dwFlag&PS_FRAME_ENABLED){
				int frame;
				if (m_Definition->m_dwFlag&PS_FRAME_ANIMATE)PS::SimulateAnimation(frame,m_Definition,P,T);
				else										frame = P->m_iAnimStartFrame;
                m_Definition->m_Animation.CalculateTC(frame,lt,rb);
            }
			if (m_Definition->m_dwFlag&PS_ALIGNTOPATH) 	m_Sprite.Render(Pos,C,sz*0.5f,D,lt,rb);
            else                                     	m_Sprite.Render(Pos,C,sz*0.5f,angle,lt,rb);
        }
    }
}
//----------------------------------------------------

void CPSObject::Render( Fmatrix& parent, ERenderPriority flag ){
    if (flag==rpNormal){
    	// draw emitter
       	DWORD C = 0xFFFFEBAA;
        switch (m_Emitter.m_EmitterType){
        case PS::SEmitter::emPoint:
			DU::DrawLineSphere(m_Emitter.m_Position, PSOBJECT_SIZE/10, C, true);
            break;
        case PS::SEmitter::emCone:
			DU::DrawFaceNormal(m_Emitter.m_Position, m_Emitter.m_ConeDirection, 1, C);
            break;
        case PS::SEmitter::emSphere:
			DU::DrawLineSphere(m_Emitter.m_Position, m_Emitter.m_SphereRadius, C, true);
            break;
        case PS::SEmitter::emBox:   
        	DU::DrawBox(m_Emitter.m_Position,m_Emitter.m_BoxSize,true,C);
            break;
        default: THROW;
    	}
        if( Selected() ){
            Fbox bb; GetBox(bb);
            DWORD clr = Locked()?0xFFFF0000:0xFFFFFFFF;
            DU::DrawSelectionBox(bb,&clr);
        }
    }else if (flag==rpAlphaNormal){
        if (UI->Device.m_Frustum.testSphere(m_Emitter.m_Position,PSOBJECT_SIZE))
            DrawPS();
    }
}
//----------------------------------------------------

void CPSObject::RenderSingle(Fmatrix& parent){
	Render(parent,rpNormal);
	Render(parent,rpAlphaNormal);
}
//----------------------------------------------------

bool CPSObject::FrustumPick(const CFrustum& frustum, const Fmatrix& parent){
    return (frustum.testSphere(m_Emitter.m_Position,PSOBJECT_SIZE))?true:false;
}
//----------------------------------------------------

bool CPSObject::RayPick(float& distance, Fvector& start, Fvector& direction, Fmatrix& parent, SRayPickInfo* pinf){
	Fvector pos,ray2;
    pos.set(m_Emitter.m_Position);
	ray2.sub( pos, start );

    float d = ray2.dotproduct(direction);
    if( d > 0  ){
        float d2 = ray2.magnitude();
        if( ((d2*d2-d*d) < (PSOBJECT_SIZE*PSOBJECT_SIZE)) && (d>PSOBJECT_SIZE) ){
        	if (d<distance){
	            distance = d;
    	        return true;
            }
        }
    }
	return false;
}
//----------------------------------------------------

void CPSObject::Move( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    UI->UpdateScene();
	m_Emitter.m_Position.add( amount );
}
//----------------------------------------------------

void CPSObject::Rotate( Fvector& center, Fvector& axis, float angle ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	Fmatrix m;
	m.rotation			(axis, (axis.y)?-angle:angle);
	m_Emitter.m_Position.sub(center);
	m.transform_tiny	(m_Emitter.m_Position);
	m_Emitter.m_Position.add(center);
	m.transform_dir		(m_Emitter.m_ConeDirection);
    UI->UpdateScene		();
}
//----------------------------------------------------

void CPSObject::LocalRotate(Fvector& axis, float angle ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
	Fmatrix m;
	m.rotation			(axis, (axis.y)?-angle:angle);
	m.transform_tiny	(m_Emitter.m_ConeDirection);
    UI->UpdateScene		();
}
//----------------------------------------------------
void CPSObject::Scale( Fvector& center, Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    if (m_Emitter.m_EmitterType==PS::SEmitter::emBox){
		m_Emitter.m_BoxSize.add(amount);
	    if (m_Emitter.m_BoxSize.x<EPS) m_Emitter.m_BoxSize.x=EPS;
	    if (m_Emitter.m_BoxSize.y<EPS) m_Emitter.m_BoxSize.y=EPS;
    	if (m_Emitter.m_BoxSize.z<EPS) m_Emitter.m_BoxSize.z=EPS;
    }
    if (m_Emitter.m_EmitterType==PS::SEmitter::emSphere){
		m_Emitter.m_SphereRadius += amount.magnitude();
    	if (m_Emitter.m_SphereRadius<EPS) m_Emitter.m_SphereRadius=EPS;
    }
    UI->UpdateScene();
}

void CPSObject::LocalScale( Fvector& amount ){
	if (Locked()){
    	Log->DlgMsg(mtInformation,"Object %s - locked.", GetName());
        return;
    }
    if (m_Emitter.m_EmitterType==PS::SEmitter::emBox){
		m_Emitter.m_BoxSize.add(amount);
	    if (m_Emitter.m_BoxSize.x<EPS) m_Emitter.m_BoxSize.x=EPS;
	    if (m_Emitter.m_BoxSize.y<EPS) m_Emitter.m_BoxSize.y=EPS;
    	if (m_Emitter.m_BoxSize.z<EPS) m_Emitter.m_BoxSize.z=EPS;
    }
    if (m_Emitter.m_EmitterType==PS::SEmitter::emSphere){
		m_Emitter.m_SphereRadius += amount.magnitude();
    	if (m_Emitter.m_SphereRadius<EPS) m_Emitter.m_SphereRadius=EPS;
    }
    UI->UpdateScene();
}
//----------------------------------------------------

bool CPSObject::Compile(PS::SDef* source){
	if (!source) return false;
	VERIFY(!m_Shader&&!m_Definition);
	m_Definition= source;
    if (source->m_ShaderName[0]&&source->m_TextureName[0]) 	m_Shader = UI->Device.Shader.Create(source->m_ShaderName,source->m_TextureName);
    else    												m_Shader = 0;
    m_Emitter.Compile(&source->m_DefaultEmitter);
    return true;
}
//----------------------------------------------------

bool CPSObject::Compile(LPCSTR name){
	PS::SDef* source = PSLib->FindPS(name);
    return Compile(source);
}
//----------------------------------------------------

void CPSObject::UpdateEmitter(PS::SEmitterDef* E){
    m_Emitter.Compile(E);
}
//----------------------------------------------------

void CPSObject::Play(){
	m_Emitter.Play();
}
//----------------------------------------------------

void CPSObject::Stop(){
	m_Emitter.Stop();
    m_Particles.clear();
}
//----------------------------------------------------

bool CPSObject::Load(CStream& F){
	DWORD version = 0;

    R_ASSERT(F.ReadChunk(CPSOBJECT_CHUNK_VERSION,&version));
    if( version!=CPSOBJECT_VERSION ){
        Log->DlgMsg( mtError, "PSObject: Unsupported version.");
        return false;
    }
    
	SceneObject::Load(F);

	char buf[1024];
    R_ASSERT(F.FindChunk(CPSOBJECT_CHUNK_REFERENCE));
    F.RstringZ(buf);
	Compile(buf);
    if (!m_Definition){
        Log->DlgMsg( mtError, "CPSObject: '%s' not found in library", buf );
        return false;
    }

    // читать обязательно после компиляции
    R_ASSERT(F.ReadChunk(CPSOBJECT_CHUNK_EMITTER,&m_Emitter));
    
    return true;
}
//----------------------------------------------------

void CPSObject::Save(CFS_Base& F){
	SceneObject::Save(F);

	F.open_chunk	(CPSOBJECT_CHUNK_VERSION);
	F.Wword			(CPSOBJECT_VERSION);
	F.close_chunk	();

    VERIFY(m_Definition);
	F.open_chunk	(CPSOBJECT_CHUNK_REFERENCE);
    F.WstringZ		(m_Definition->m_Name);
	F.close_chunk	();

    F.write_chunk	(CPSOBJECT_CHUNK_EMITTER,&m_Emitter,sizeof(m_Emitter));
}
//----------------------------------------------------

