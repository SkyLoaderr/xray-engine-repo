#ifndef MXSTDGUI_INCLUDED // -*- C++ -*-
#define MXSTDGUI_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  GUI framework for building applications that use MxStdModel surface
  model objects.

  Copyright (C) 1999 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdGUI.h,v 1.10 2000/12/14 17:49:11 garland Exp $

 ************************************************************************/

#include "MxGL.h"
#include "MxGUI.h"
#include "MxAsp.h"
#include "MxStdModel.h"

#include "MxGeom3D.h"
#include <gfx/arcball.h>
#include <gfx/trackball.h>
#include <gfx/planeball.h>
#include "MxCamera.h"

class MxStdGUI : public MxGUI
{
private:
    uint selection_state;

protected:
    MxDynBlock<MxStdModel *> models;
    uint current;

    MxBounds	bounds;
	Baseball*	cam_ball;
    MxCamera	camera;

    uint pick_something(int *where, double radius, uint mode);
    void begin_redraw();
    void end_redraw();
    void default_colors();

public:
    // Inherited application methods overridden here
    virtual void initialize(int argc, char **argv,
			    Fl_Menu_Item *layout=NULL,
			    int xw=640, int yw=480);

    // Inherited GUI methods that are overridden here
    //
    virtual void setup_for_drawing();
    virtual void draw_contents();
    virtual bool mouse_down(int *where, int which);
    virtual bool mouse_up(int *where, int which);
    virtual bool mouse_drag(int *where, int *last, int which);
    virtual void cmdline_file(const char *file);

    // New GUI methods that may be overriden by the application
    //
    virtual void draw_custom_contents(MxStdModel *) { }

public:
    // Public interface introduced in this class
    //
    MxStdGUI();
	~MxStdGUI();

	void set_camera_mode(int cam_mode);
    void asp_setup(MxAspStore *store);

    void attach_model(MxStdModel *m0) { attach_models(&m0); }
    void attach_models(MxStdModel **m0, uint count=1);
    void select_model(uint);
    MxStdModel *model()       { return models(current); }
    MxStdModel *model(uint i) { return models(i); }

    uint pick_face(int *where);
    uint pick_vertex(int *where);

    uint selected_vertex, selected_face;

    bool will_draw_surface;
    bool will_draw_mesh;
    bool will_draw_points;
    bool will_draw_texture;
    bool will_draw_color;
    bool will_light_scene;
    bool will_smooth_lines;

    double surface_color[3];
    double background_color[3];
    double mesh_color[3];
    double line_width;
};

// MXSTDGUI_INCLUDED
#endif
