#include "stdafx.h"
#include "xr_effsun.h"
#include "xr_creator.h"
#include "environment.h"
#include "xr_ini.h"

CSun::CSun( CInifile* pIni, LPSTR section ) {
// общие установки
	sun_color.set				(255, 255, 255, 255);
	sun_dir.set					(-0.5774f,-0.5774f,-0.5774f);

	LoadSunData					( pIni, section );
	LensFlare.Load				( pIni, section );
}

CSun::~CSun	( ) {
}

void CSun::LoadSunData	( CInifile* pIni, LPSTR section ) {
	VERIFY( section );
	sun_color.set		( pIni->ReadCOLOR( section, "sun_color" ));
	sun_dir				= pIni->ReadVECTOR( section, "sun_dir"  );
	sun_dir.normalize	();
}

void CSun::OnMove( ){
	LensFlare.OnMove();
}

void CSun::RenderSource( ) {
	LensFlare.Render	(true,false);
}

void CSun::RenderFlares( ) {
	LensFlare.Render	(false,true);
}

void CSun::Update() 
{
	LensFlare.Update(sun_dir,pCreator->Environment.Current.Far,sun_color);
}
