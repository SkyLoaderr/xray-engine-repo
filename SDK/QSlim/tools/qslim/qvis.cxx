/************************************************************************

  Interactive QSlim program.  This version of QSlim actually opens up
  a simple window pane and let's you interact with the model.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: qvis.cxx,v 1.26.2.1 2002/01/31 18:37:35 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>
#include <MxStdGUI.h>
#include <stdio.h>

#include <FL/fl_file_chooser.H>
#include <FL/filename.H>
#include <FL/Fl_Slider.H>
#include <FL/fl_draw.H>
#include <FL/fl_ask.H>

#include "qslim.h"

static unsigned int orig_vert_count, orig_face_count;

class GUI : public MxStdGUI
{
public:
    bool will_draw_quadrics;
    bool will_draw_edges;
    bool will_colorize_regions;
    double quadric_radius;

    Fl_Slider *target_slider;

    virtual void draw_custom_contents	(MxStdModel *m);
    virtual void add_lower_controls		(int& yfill, const int pad);
    virtual void cleanup_for_exit		();
};

GUI gui;


////////////////////////////////////////////////////////////////////////
//
// Internal functionality
//

void decimate_callback(const MxPairContraction& conx, float cost)
{
    slim_history_callback(conx, cost);
}


void slim_undo()
{
    MxPairContraction& conx = history->drop();

    eslim->apply_expansion(conx);
}

void slim_to_target(unsigned int target, bool recalculate)
{
	if (recalculate){
		unsigned int tgt_rc = m->face_count();
		if( tgt_rc > slim->valid_faces && history ){
			while( slim->valid_faces < tgt_rc && history->length() > 0 )
				slim_undo();
		}
	}

	if (target > m->face_count())  target = m->face_count();
	if (target < slim->valid_faces)
		slim->decimate(target,slim_max_error);
	else if( target > slim->valid_faces && history ){
		while( slim->valid_faces < target && history->length() > 0 )
			slim_undo();
	}
}



////////////////////////////////////////////////////////////////////////
//
// GUI methods
//


void GUI::cleanup_for_exit()
{
    slim_cleanup();
}

static
void update_status()
{
    gui.status("Original: %uv/%uf   Current: %uv/%uf",
	       orig_vert_count, orig_face_count,
	       slim?slim->valid_verts:m->vert_count(),
	       slim?slim->valid_faces:m->face_count());
}

static void cb_model(Fl_Menu_ *, int i) { gui.select_model(i); }

static void cb_pp(void *, int will_ask)
{
	if( will_ask ){
		char buf[64];
		sprintf(buf, "%d", slim->placement_policy);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->placement_policy = atoi(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}
static void cb_bw(void *, int will_ask)
{
	if( will_ask ){
		char buf[64];
		sprintf(buf, "%f", slim->boundary_weight);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->boundary_weight = atof(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}
static void cb_qwp(void *, int will_ask)
{
	if( will_ask ){
		char buf[64];
		sprintf(buf, "%d", slim->weighting_policy);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->weighting_policy = atoi(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}
static void cb_pbm(void *, int will_ask)
{
	if( will_ask ){
		char buf[64];
		sprintf(buf, "%f", slim->meshing_penalty);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->meshing_penalty = atof(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}
static void cb_dcr(void *, int will_ask)
{
	if( will_ask ){
		char buf[64];
		sprintf(buf, "%f", slim->compactness_ratio);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->compactness_ratio = atof(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}
static void cb_jo(void *, int will_ask)
{
	if( will_ask ){
//		fl_bool();
		char buf[64];
		sprintf(buf, "%d", slim->will_join_only);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->will_join_only = atoi(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}
static void cb_lvt(void *, int will_ask)
{
	if( will_ask ){
		//		fl_bool();
		char buf[64];
		sprintf(buf, "%f", slim->local_validity_threshold);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->local_validity_threshold = atof(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}
static void cb_vdl(void *, int will_ask)
{
	if( will_ask ){
		//		fl_bool();
		char buf[64];
		sprintf(buf, "%d", slim->vertex_degree_limit);
		const char *t_str = fl_input("Value:\n" , buf);

		if( t_str&&t_str[0]){
			slim->vertex_degree_limit = atof(t_str);
		}
	}

	slim_to_target(slim->valid_faces,true);
	gui.canvas->redraw();
	update_status();
}

static void cb_colorize()
{
    unsigned int i;

    if( !gui.will_colorize_regions )
    {
	m_orig->color_binding(MX_PERVERTEX);

	// Randomly color the vertices
	for(i=0; i<m_orig->vert_count(); i++)
	{
	    m_orig->add_color(random1(), random1(), random1());
	}

	// Propagate colors as per current model state
	for(i=history->length(); i>0; i--)
	{
	    MxPairContraction& conx = (*history)[i-1];
	    m_orig->color(conx.v2) = m_orig->color(conx.v1);
	}

	gui.will_draw_color = true;
    }
    else
    {
	m_orig->color_binding(MX_UNBOUND);
	gui.will_draw_color = false;
    }

    gui.will_colorize_regions = !gui.will_colorize_regions;
    gui.canvas->redraw();
}

static void cb_target(void *, int will_ask)
{
    unsigned int t;

	if( will_ask ){
		char buf[64];
		sprintf(buf, "%d", slim?slim->valid_faces:m->face_count());
		const char *t_str = fl_input("Enter desired number of faces:\n" , buf);

		if( !t_str ) return;
		else if( *t_str == '\0' ) t = m->face_count();
		else  t = atoi(t_str);

		gui.target_slider->value(t);
	}else{
		t = (unsigned int)gui.target_slider->value();
	}

    slim_to_target(t,false);
    gui.canvas->redraw();
    update_status();
}

void GUI::add_lower_controls(int& yfill, const int pad)
{
    int xw = toplevel->w() - 2*pad;

    yfill += pad;
    target_slider = new Fl_Slider(pad, yfill, xw, 25);
    target_slider->type(3);
    target_slider->callback((Fl_Callback*)cb_target, NULL);
    yfill += target_slider->h();
}

static
void draw_quadric(unsigned int v)
{
    mx_draw_quadric(((MxQSlim*)slim)->vertex_quadric(v),
		    gui.quadric_radius, m->vertex(v));
}

void GUI::draw_custom_contents(MxStdModel *m)
{
    if(will_draw_edges && eslim)  glOffsetForMesh();

    if( will_draw_edges && eslim )
    {
        glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
        glC(0.0, 1.0, 0.0);

        glBegin(GL_LINES);
        for(unsigned int i=0; i<eslim->edge_count(); i++)
        {
	    const MxQSlimEdge *e = eslim->edge(i);
	    glV(m->vertex(e->v1));
	    glV(m->vertex(e->v2));
        }
        glEnd();

        glPopAttrib();
    }

    if( will_draw_quadrics )
    {
        glPushAttrib(GL_LIGHTING_BIT|GL_TRANSFORM_BIT);
        glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
        mx_quadric_shading(MX_GREEN_ELLIPSOIDS);
        for(VID v=0; v<m->vert_count(); v++)
            if( m->vertex_is_valid(v) )
		draw_quadric(v);

        glPopAttrib();
    }
    else if( selected_vertex != MXID_NIL &&
	     m->vertex_is_valid(selected_vertex) )
    {
        glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_COLOR_MATERIAL);
        mx_quadric_shading(MX_GREEN_ELLIPSOIDS);
	draw_quadric(selected_vertex);
        glPopAttrib();
    }
}


////////////////////////////////////////////////////////////////////////
//
// Main application routine
//

main(int argc, char **argv)
{
    // Change default values for application variables in GUI version
    will_record_history = true;
    
    gui.will_draw_quadrics = false;
    gui.will_draw_edges = false;
    gui.will_colorize_regions = false;
	gui.will_draw_mesh = true;
    gui.quadric_radius = 0.75;

    // NOTE: Pass NULL instead of argv because we use the standard
    //       QSlim command line parsing routine below.
    gui.initialize(0, NULL);

    smf = new MxSMFReader;
    gui.asp_setup(smf->asp_store());

    process_cmdline(argc, argv);

    // No files were specified on the command line,
    // present the user with a file selector.
    //
    if( m->face_count() == 0 )
    {
	const char *filename =
	    fl_file_chooser("Select file to view:", "*.smf", "");

	if( !filename ) exit(1);

	smf->unparsed_hook = unparsed_hook;
	input_file(filename);
    }

    if( m->normal_binding() == MX_UNBOUND )
    {
        m->normal_binding(MX_PERFACE);
        m->synthesize_normals();
    }

    slim_init();
    if (eslim) eslim->contraction_callback = decimate_callback;

    include_deferred_files();


    gui.toplevel->label("QSlim Surface Simplification");
    gui.target_slider->value(m->face_count());
    gui.target_slider->range(0, m->face_count());
    gui.target_slider->step(1);


    gui.add_toggle_menu("&View/Will draw/Ellipsoids", FL_CTRL+'e',
			gui.will_draw_quadrics);
    gui.add_toggle_menu("&View/Will draw/Edges", 0,
			gui.will_draw_edges);

    gui.menu_bar->add("&Simplify/Face Tar&get ...", FL_CTRL+'g',
		      (Fl_Callback *)cb_target, (void *)1);
    gui.menu_bar->add("&Simplify/Draw &vertex regions", FL_CTRL+'v',
		      (Fl_Callback *)cb_colorize, 0,
		      FL_MENU_TOGGLE|
		      (gui.will_colorize_regions?FL_MENU_VALUE:0));

	gui.menu_bar->add("&Simplify/Optimal Placement Policy [0-3] ...", 0,
		(Fl_Callback *)cb_pp, (void *)1);
	gui.menu_bar->add("&Simplify/Boundary Weight ...", 0,
		(Fl_Callback *)cb_bw, (void *)1);
	gui.menu_bar->add("&Simplify/Quadric Weighting Policy [0-2] ...", 0,
		(Fl_Callback *)cb_qwp, (void *)1);
	gui.menu_bar->add("&Simplify/Penalty For Bad Mesh ...", 0,
		(Fl_Callback *)cb_pbm, (void *)1);
	gui.menu_bar->add("&Simplify/Desired Compactness Ratio ...", 0,
		(Fl_Callback *)cb_dcr, (void *)1);
	gui.menu_bar->add("&Simplify/Local Validity Threshhold...", 0,
		(Fl_Callback *)cb_lvt, (void *)1);
	gui.menu_bar->add("&Simplify/Vertex Degree Limit ...", 0,
		(Fl_Callback *)cb_vdl, (void *)1);
	gui.menu_bar->add("&Simplify/Join Only ...", 0,
		(Fl_Callback *)cb_jo, (void *)1);	

    // For the GUI interface, the user can examine both the
    // approximation and the original.  So, we have to make a copy of
    // the model and set up some menu items to switch views back and
    // forth.
    //
	if( !m_orig )
		m_orig = m->clone();
	gui.attach_model(m);
	gui.attach_model(m_orig);
	gui.menu_bar->add("&Models/Approximation", FL_CTRL+'1',
		(Fl_Callback *)cb_model, (void *)0);
	gui.menu_bar->add("&Models/Original", FL_CTRL+'2',
		(Fl_Callback *)cb_model, (void *)1);

	orig_vert_count = m->vert_count();
	orig_face_count = m->face_count();
	update_status();

	return gui.run();
}
