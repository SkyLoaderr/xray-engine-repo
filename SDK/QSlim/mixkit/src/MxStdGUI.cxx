/************************************************************************

MxStdGUI

Copyright (C) 1999 Michael Garland.  See "COPYING.txt" for details.

$Id: MxStdGUI.cxx,v 1.9.2.1 2004/07/01 18:46:58 garland Exp $

************************************************************************/

#include "stdmix.h"
#include "mixio.h"
#include "stdio.h"
#include "MxStdGUI.h"
#include "MxGLUtils.h"
#include "MxSMF.h"
#include <FL/Fl_Color_Chooser.H>
#include <FL/fl_file_chooser.H>
#include <FL/filename.H>


const int MX_SELECT_NONE=0;
const int MX_SELECT_FACE=1;
const int MX_SELECT_VERT=2;

MxStdGUI::MxStdGUI()
{
	will_draw_surface = true;
	will_draw_mesh = false;
	will_draw_points = false;
	will_light_scene = true;
	will_smooth_lines = false;
	line_width = 1.0;

	selection_state = MX_SELECT_NONE;
	selected_vertex = selected_face = MXID_NIL;

	default_colors();

	cam_ball = new Arcball;
}

MxStdGUI::~MxStdGUI()
{
	delete cam_ball;
}

void MxStdGUI::set_camera_mode(int cam_mode)
{
	delete cam_ball;
	switch (cam_mode){
	case 0: cam_ball = new Planeball; camera.arc_ball = false; break;
	case 1: 
	default:
		cam_ball = new Arcball; break;
	}
}

static void cb_color(Fl_Menu_ *, double *c)
{
	if( fl_color_chooser("Select new color", c[0], c[1], c[2]) )
		MxGUI::current->canvas->redraw();
}

void MxStdGUI::initialize(int argc, char **argv, Fl_Menu_Item *layout,
						  int xw, int yw)
{
	MxGUI::initialize(argc, argv, layout, xw, yw);

	add_toggle_menu("&View/Light scene", FL_CTRL+'l', will_light_scene);

	menu_bar->add("&View/Set color/for Surface ...", 0,
		(Fl_Callback*)cb_color,surface_color);
	menu_bar->add("&View/Set color/for Mesh ...", 0,
		(Fl_Callback*)cb_color,mesh_color);
	menu_bar->add("&View/Set color/for Background ...", 0,
		(Fl_Callback*)cb_color,background_color);

	add_toggle_menu("&View/Will draw/Surface", FL_CTRL+'s', will_draw_surface);
	add_toggle_menu("&View/Will draw/Mesh", FL_CTRL+'m', will_draw_mesh);
	add_toggle_menu("&View/Will draw/Points", FL_CTRL+'o', will_draw_points);
}

void MxStdGUI::default_colors()
{
	surface_color[0] = 0.569;
	surface_color[1] = 0.447;
	surface_color[2] = 0.315;

	background_color[0] = 0.3;//0.3;
	background_color[1] = 0.3;//0.3;
	background_color[2] = 0.3;//1.0;

	mesh_color[0] = mesh_color[1] = mesh_color[2] = 0.0f;//5f;
}

static void cb_model(Fl_Menu_ *, int i)
{ ((MxStdGUI*)MxGUI::current)->select_model(i); }

void MxStdGUI::cmdline_file(const char *filename)
{
	MxSMFReader smf;
	MxStdModel *m = NULL;
	static int model_count = 0;

	if( !filename )
	{
		filename = fl_file_chooser("Select file to view:", "*.smf", "");
		if( !filename ) return;
	}

	char buf[128];
	//.    sprintf(buf, "&Models/%s", fl_filename_name(filename));
	menu_bar->add(buf, FL_CTRL+'1'+model_count,
		(Fl_Callback *)cb_model, (void *)model_count);
	model_count++;

	if( !strcmp(filename, "-") )
		m = smf.read(cin);
	else
	{
		ifstream in(filename);
		if(in.good())  m = smf.read(in);
		in.close();
	}

	if( !m ) return;

	if( m->normal_binding() == MX_UNBOUND )
	{
		m->normal_binding(MX_PERFACE);
		m->synthesize_normals();
	}

	attach_model(m);
}

void MxStdGUI::attach_models(MxStdModel **m0, uint count)
{
	will_draw_texture = true;
	will_draw_color = true;
	current = 0;

	bounds.reset();
	for(uint i=0; i<count; i++)
	{
		MxStdModel *m = m0[i];
		models.add(m);

		will_draw_texture = will_draw_texture && m->texmap();
		will_draw_color = will_draw_color && (m->color_binding()!=MX_UNBOUND);

		for(MxVertexID v=0; v<m->vert_count(); v++)
			bounds.add_point(m->vertex(v));

	}
	bounds.complete();
}

void MxStdGUI::select_model(uint i)
{
	if( i<models.length() )
	{
		current = i;
		canvas->redraw();
	}
}

void MxStdGUI::setup_for_drawing()
{
	//
	// Initialize the general GL setup
	//

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);

	GLfloat mat_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat mat_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);

	GLfloat light_pos[] = {0.0f, 0.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glEnable(GL_LIGHT0);

	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(0.5f, 0.5f, 0.5f);
	glClearColor(0.3f, 0.3f, 1.0f, 0.0f);

	//
	// Initialize texturing
	//
	MxRaster *tex = models[current]->texmap();
	if( tex )
	{
		if( will_draw_texture ) glEnable(GL_TEXTURE_2D);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		glTexImage(tex);
	}


	//
	// Set up the camera and arcball controller
	//

	float aspect = (float)canvas->w() / (float)canvas->h();
	camera.look_at(bounds, aspect);
	cam_ball->bounding_sphere(bounds.center, bounds.radius);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	camera.apply();
}

void MxStdGUI::begin_redraw()
{
	MxStdModel *m = models[current];

	// Clear the framebuffer
	glClearColor(background_color[0], background_color[1],
		background_color[2], 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Apply the transformation from the arcball controller
	glMatrixMode(GL_MODELVIEW);
	cam_ball->apply_transform();

	if (!camera.arc_ball){
		GLfloat light_pos[] = {0.0f, 1.0f, 1.0f, 0.0f};
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	}

	if(will_light_scene) glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING);

	// We'll change this to pure white if texturing/coloring
	glColor3dv(surface_color);

	if( will_smooth_lines ) glEnable(GL_LINE_SMOOTH);
	else glDisable(GL_LINE_SMOOTH);

	glLineWidth(line_width);

	if( will_draw_mesh || will_smooth_lines )  glOffsetForMesh();
	else                                       glNoOffset();

	if( will_draw_texture )
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		m->binding_mask |= MX_TEXTURE_MASK;
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
		m->binding_mask &= (~MX_TEXTURE_MASK);
	}

	if( will_draw_color && m->color_binding()!=MX_UNBOUND )
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		m->binding_mask |= MX_COLOR_MASK;
	}
	else
		m->binding_mask &= (~MX_COLOR_MASK);

	CAREFUL(mxgl_check_errors("at end of MxGUI::begin_redraw"));
}

void MxStdGUI::end_redraw()
{
	cam_ball->unapply_transform();
	CAREFUL(mxgl_check_errors("at end of MxGUI::end_redraw"));
}

void MxStdGUI::draw_contents()
{
	begin_redraw();

	MxStdModel *m = models[current];

	switch( selection_state )
	{
	case MX_SELECT_NONE:
		if( will_draw_surface )
		{
			mx_render_model(*m);
			if( will_draw_mesh )
				mx_draw_mesh(*m, mesh_color);
		}
		else if( will_draw_mesh )
			mx_draw_wireframe(*m, mesh_color);

		if( will_draw_points )
			mx_draw_pointcloud(*m);

		draw_custom_contents(m);
		break;

	case MX_SELECT_FACE:  mx_render_model(*m); break;
	case MX_SELECT_VERT:  mx_draw_pointcloud(*m); break;
	}


	end_redraw();
}


bool MxStdGUI::mouse_down(int *where, int which)
{
	if( which==1 && Fl::event_state(FL_CTRL) )
	{
		uint old = selected_vertex;
		selected_vertex = pick_vertex(where);
		return old!=selected_vertex;
	}
	else if( which==1 && Fl::event_state(FL_SHIFT) )
	{
		uint old = selected_face;
		selected_face = pick_face(where);
		return old!=selected_face;
	}
	else
		return cam_ball->mouse_down(where, which);
}

bool MxStdGUI::mouse_up(int *where, int which)
{
	return cam_ball->mouse_up(where, which);
}

bool MxStdGUI::mouse_drag(int *where, int *last, int which)
{
	return cam_ball->mouse_drag(where, last, which);
}

void MxStdGUI::asp_setup(MxAspStore *store)
{
	store->defvar("will_draw_surface", MXASP_BOOL, &will_draw_surface);
	store->defvar("will_draw_mesh", MXASP_BOOL, &will_draw_mesh);
	store->defvar("will_draw_points", MXASP_BOOL, &will_draw_points);
	store->defvar("will_draw_texture", MXASP_BOOL, &will_draw_texture);
	store->defvar("will_draw_color", MXASP_BOOL, &will_draw_color);
	store->defvar("will_light_scene", MXASP_BOOL, &will_light_scene);
	store->defvar("will_smooth_lines", MXASP_BOOL, &will_smooth_lines);

	store->defvar("surface_color", MXASP_DOUBLE, surface_color, 3);
	store->defvar("background_color", MXASP_DOUBLE, background_color, 3);
	store->defvar("mesh_color", MXASP_DOUBLE, mesh_color, 3);

	store->defvar("line_width", MXASP_DOUBLE, &line_width);

	store->defvar("cam.fovy", MXASP_DOUBLE, &camera.fovy);
	store->defvar("cam.aspect", MXASP_DOUBLE, &camera.aspect);
	store->defvar("cam.znear", MXASP_DOUBLE, &camera.znear);
	store->defvar("cam.zfar", MXASP_DOUBLE, &camera.zfar);
	store->defvar("cam.from", MXASP_DOUBLE, camera.from, 3);
	store->defvar("cam.at", MXASP_DOUBLE, camera.at, 3);
	store->defvar("cam.up", MXASP_DOUBLE, camera.up, 3);

	store->defvar("bounds.min", MXASP_DOUBLE, bounds.min, 3);
	store->defvar("bounds.max", MXASP_DOUBLE, bounds.max, 3);
	store->defvar("bounds.ctr", MXASP_DOUBLE, bounds.center, 3);
	store->defvar("bounds.radius", MXASP_DOUBLE, &bounds.radius, 1);

	/* BUG: libgfx arcball does not support this
	store->defvar("arcball.cam", MXASP_FLOAT, arcball.cam, 3);
	*/
}

uint MxStdGUI::pick_something(int *where, double radius, uint mode)
{
	canvas->make_current();

	selection_state = mode;
	GLuint buffer[128];

	mxgl_begin_pick(where, radius, buffer, 128);
	camera.apply();
	canvas->draw();
	selection_state = MX_SELECT_NONE;

	return mxgl_complete_pick(buffer);
}

uint MxStdGUI::pick_face(int *where)
{ return pick_something(where, 1.0, MX_SELECT_FACE); }

uint MxStdGUI::pick_vertex(int *where)
{ return pick_something(where, 16.0, MX_SELECT_VERT); }

