#include "xrCore.h"

#include "../../QSCore/src/MxStdModel.h"
#include "../../QSCore/src/MxQSlim.h"

#pragma comment(lib,"x:/xrCore.lib")

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
					 char *    lpCmdLine,
					 int       nCmdShow)
{
	Core._initialize("QSTest",0,FALSE);
	FS._initialize	(CLocatorAPI::flTargetFolderOnly,"x:/import");

	MxStdModel* m	= xr_new<MxStdModel>(100,100);

	m->add_vertex	(0,0,0);
	m->add_vertex	(1,0,0);
	m->add_vertex	(0,1,0);
	m->add_vertex	(1,1,0);
	m->add_vertex	(0,0,1);
	m->add_vertex	(1,0,1);
	m->add_vertex	(0,1,1);
	m->add_vertex	(1,1,1);
	m->add_face		(0,3,1);
	m->add_face		(0,2,3);
	m->add_face		(4,5,7);
	m->add_face		(4,7,6);
	m->add_face		(0,1,5);
	m->add_face		(0,5,4);
	m->add_face		(1,3,7);
	m->add_face		(1,7,5);
	m->add_face		(3,2,6);
	m->add_face		(3,6,7);
	m->add_face		(2,0,4);
	m->add_face		(2,4,6);

	MxQSlim* slim	= xr_new<MxEdgeQSlim>(m);
	slim->meshing_penalty = 10000.f;
	slim->initialize();

	slim->decimate			(8);
	m->compact_vertices		();

	IWriter* W		= FS.w_open("x:/import/cube4.smf");

	for (u32 v=0; v<m->vert_count(); v++){
		string256	tmp;
		sprintf		(tmp,"v %f %f %f",m->vertex(v)[0],m->vertex(v)[1],m->vertex(v)[2]);
		W->w_string	(tmp);
	}
	for (u32 f=0; f<m->face_count(); f++){
		if (m->face_is_valid(f)){
			string256	tmp;
			sprintf		(tmp,"f %d %d %d",m->face(f)[0]+1,m->face(f)[1]+1,m->face(f)[2]+1);
			W->w_string	(tmp);
		}
	}
	FS.w_close		(W);

	xr_delete		(slim);
	xr_delete		(m);

	Core._destroy();

	return 0;
}
