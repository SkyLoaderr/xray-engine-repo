#include "stdafx.h"
#include "ObjectAnimator.h"
#include "motion.h"
#include "xr_ini.h"
 
bool st_AnimParam::Update(float dt){ 
	t+=dt; 
	if (t>max_t){ 
		if (bLoop){ t=min_t; return true; }
		else	  { t=max_t; return false;}
	}
	return true;
}
void st_AnimParam::Set(COMotion* M, bool _loop){ 
	t=0; 
	bLoop = _loop;
    min_t=(float)M->FrameStart()/M->FPS(); 
    max_t=(float)M->FrameEnd()/M->FPS();
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObjectAnimator::CObjectAnimator()
{
}

CObjectAnimator::~CObjectAnimator()
{	
	for(MotionPairIt m_it=m_Motions.begin(); m_it!=m_Motions.end(); m_it++){
		free(m_it->first);
		_DELETE(m_it->second);
	}
	m_Motions.clear		();
}

void CObjectAnimator::SetActiveMotion(COMotion* mot, bool bLoop){
	m_ActiveMotion		=mot;
    if (m_ActiveMotion) m_MParam.Set(m_ActiveMotion, bLoop);
	vPosition.set		(0,0,0);
	mRotate.identity	();
}

void CObjectAnimator::LoadMotions(const char* fname){
	FILE_NAME			full_path;
	if (!Engine.FS.Exist( full_path, Path.Current, fname ))
		if (!Engine.FS.Exist( full_path, Path.Meshes, fname ))
			Device.Fatal("Can't find motion file '%s'.",fname);

	CFileStream F		(full_path);
	DWORD dwMCnt		= F.Rdword(); VERIFY(dwMCnt);
	for (DWORD i=0; i<dwMCnt; i++){
		COMotion* M		= new COMotion();
		bool bRes		= M->Load(F);
		if (!bRes){		
			Log("ERROR: Can't load motion. Incorrect file version.");
			THROW;
		}
		m_Motions[strdup(M->Name())]=M;
	}
}

void CObjectAnimator::Load(CInifile* ini, const char * section)
{
	LPCSTR temp			= ini->ReadSTRING(section,"motions");
	Load				(temp);
}

void CObjectAnimator::Load(const char * name)
{
	LoadMotions			(name);
	SetActiveMotion		(0,false);
}

void CObjectAnimator::OnMove()
{
	if (IsMotionActive()){
		Fvector R;
		m_ActiveMotion->Evaluate(m_MParam.Frame(),vPosition,R);
		if (!m_MParam.Update(Device.fTimeDelta)) StopMotion();
		mRotate.setHPB	(R.x,R.y,R.z);
	}
}

COMotion* CObjectAnimator::PlayMotion(LPCSTR name, bool bLoop)
{
	MotionPairIt I = m_Motions.find(LPSTR(name));
	if (I!=m_Motions.end())	SetActiveMotion(I->second, bLoop);
	else {
		Msg("OBJ ANIM::Cycle '%s' not found.",name);
		return NULL;
	}
	return I->second;
}

void CObjectAnimator::StopMotion(){
	SetActiveMotion		(0);
}

void CObjectAnimator::ManualUpdate(COMotion* M, float t){
	Fvector R;
	m_ActiveMotion->Evaluate(t,vPosition,R);
	mRotate.setHPB		(R.x,R.y,R.z);
}
