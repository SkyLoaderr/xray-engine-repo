/************************************************************************

  MxSMF

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxSMF.cxx,v 1.31 2000/12/13 15:52:00 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "mixio.h"
#include "MxSMF.h"
#include "MxVector.h"

#define SMF_MAXLINE 65536


MxSMFReader::cmd_entry MxSMFReader::read_cmds[] = {
    { "v", &MxSMFReader::vertex },
    { "t", &MxSMFReader::face },
    { "f", &MxSMFReader::face },

    // Properties and binding -- this is the new way to handle things
    { "bind", &MxSMFReader::prop_bind },
    { "n", &MxSMFReader::prop_normal },
    { "c", &MxSMFReader::prop_color },
    { "r", &MxSMFReader::prop_texcoord },

    { "begin", &MxSMFReader::begin },
    { "end", &MxSMFReader::end },

    { "trans", &MxSMFReader::trans },
    { "rot", &MxSMFReader::rot },
    { "scale", &MxSMFReader::scale },

    { "tex", &MxSMFReader::read_texture },
    { "t_trans", &MxSMFReader::t_trans },
    { "t_scale", &MxSMFReader::t_scale },

    { NULL, NULL }
};


MxSMFReader::MxSMFReader()
    : vx_stack(Mat4::I(), 2), tx_stack(Mat4::I(), 2),
      vcorrect_stack(0, 2), vfirst_stack(1, 2)
{
    next_vertex=1;
    next_face=1;
    quad_count=0;
    poly_count=0;
    line_buffer = new char[SMF_MAXLINE];
    unparsed_hook = NULL;

    define_avars();
}

MxSMFReader::~MxSMFReader()
{
    delete[] line_buffer;
}

void MxSMFReader::v_xform(Vec3& v)
{
    Vec4 v2 = vx_stack.top() * Vec4(v,1);

    v[X] = v2[X]/v2[W];
    v[Y] = v2[Y]/v2[W];
    v[Z] = v2[Z]/v2[W];
}

void MxSMFReader::t_xform(Vec2& v)
{
    Vec4 v2 = tx_stack.top() * Vec4(v[X],v[Y],0,1);

    v[X] = v2[X]/v2[W];
    v[Y] = v2[Y]/v2[W];
}

unsigned int MxSMFReader::vid_xform(int id)
{
    if( id < 0 )
	id += next_vertex;
    else
	id += vcorrect_stack.top() + (vfirst_stack.top() - 1);

    return id;
}

////////////////////////////////////////////////////////////////////////

void MxSMFReader::define_avars()
{
    asp_store()->defvar("vertex_correction", MXASP_INT, &vcorrect_stack.top());
}

void MxSMFReader::update_avars()
{
    MxAspVar *avar;

    avar = asp_store()->lookup("vertex_correction");
    if( avar )
	avar->bind("vertex_correction", MXASP_INT, &vcorrect_stack.top());
}

void MxSMFReader::begin(int, char *[], MxStdModel&)
{
    vx_stack.push();
    tx_stack.push();
    vfirst_stack.push(next_vertex);
    vcorrect_stack.push();
    update_avars();
}

void MxSMFReader::end(int, char *[], MxStdModel&)
{
    vx_stack.pop();
    tx_stack.pop();
    vfirst_stack.pop();
    vcorrect_stack.pop();
    update_avars();
}

void MxSMFReader::trans(int, char *argv[], MxStdModel&)
{
    Mat4 M = translation_matrix(Vec3(atof(argv[0]), atof(argv[1]), atof(argv[2])));
    vx_stack.top() = vx_stack.top() * M;
}

void MxSMFReader::rot(int, char *argv[], MxStdModel&)
{
    Mat4 M;
    switch( argv[0][0] )
    {
    case 'x':
	M = rotation_matrix_deg(atof(argv[1]), Vec3(1, 0, 0));
	break;
    case 'y':
	M = rotation_matrix_deg(atof(argv[1]), Vec3(0, 1, 0));
	break;
    case 'z':
	M = rotation_matrix_deg(atof(argv[1]), Vec3(0, 0, 1));
	break;
    default:
	mxmsg_signal(MXMSG_WARN, "Malformed SMF rotation command.");
	break;
    }

    vx_stack.top() = vx_stack.top() * M;
}

void MxSMFReader::scale(int, char *argv[], MxStdModel&)
{
    Mat4 M = scaling_matrix(Vec3(atof(argv[0]), atof(argv[1]), atof(argv[2])));
    vx_stack.top() = vx_stack.top() * M;
}

void MxSMFReader::t_trans(int, char *argv[], MxStdModel&)
{
    Mat4 M = translation_matrix(Vec3(atof(argv[0]), atof(argv[1]), 0.0));
    tx_stack.top() = tx_stack.top() * M;
}

void MxSMFReader::t_scale(int, char *argv[], MxStdModel&)
{
    Mat4 M = scaling_matrix(Vec3(atof(argv[0]), atof(argv[1]), 1.0));
    tx_stack.top() = tx_stack.top() * M;
}

void MxSMFReader::vertex(int, char *argv[], MxStdModel& m)
{
    Vec3 v(atof(argv[0]), atof(argv[1]), atof(argv[2]));

    v_xform(v);
    next_vertex++;

    m.add_vertex((float)v[X], (float)v[Y], (float)v[Z]);
}

void MxSMFReader::prop_bind(int argc, char *argv[], MxStdModel& m)
{
    if( argc < 2 )  return;	// Not enough arguments

    switch( argv[0][0] )
    {
    case 'n': m.normal_binding(m.parse_binding(argv[1])); break;
    case 'c': m.color_binding(m.parse_binding(argv[1])); break;
    case 'r': m.texcoord_binding(m.parse_binding(argv[1])); break;
    }
}

void MxSMFReader::prop_normal(int /*argc*/, char *argv[], MxStdModel& m)
{
    Vec3 n(atof(argv[0]), atof(argv[1]), atof(argv[2]));
    //v_xform(n);  //!!BUG: Need to transform normals appropriately
    unitize(n);
    m.add_normal((float)n[X], (float)n[Y], (float)n[Z]);
}

void MxSMFReader::prop_color(int /*argc*/, char *argv[], MxStdModel& m)
{
    float r = (float)atof(argv[0]);
    float g = (float)atof(argv[1]);
    float b = (float)atof(argv[2]);
    m.add_color(r, g, b);
}

void MxSMFReader::prop_texcoord(int /*argc*/, char *argv[], MxStdModel& m)
{
    Vec2 v(atof(argv[0]), atof(argv[1]));
    t_xform(v);
    m.add_texcoord((float)v[X], (float)v[Y]);
}

void MxSMFReader::read_texture(int /*argc*/, char *argv[], MxStdModel& m)
{
    MxRaster *tex = read_pnm_image(argv[0]);
    tex->vflip();
    m.add_texmap(tex, argv[0]);
}

void MxSMFReader::face(int argc, char *argv[], MxStdModel& m)
{
    if( argc == 3 )
    {
	unsigned int v0 = vid_xform(atoi(argv[0]));
	unsigned int v1 = vid_xform(atoi(argv[1]));
	unsigned int v2 = vid_xform(atoi(argv[2]));

	next_face++;
	m.add_face(v0 - 1, v1 - 1, v2 - 1);
    }
    else if( argc == 4 )
    {
	if( !quad_count )
	    mxmsg_signalf(MXMSG_NOTE,
	    "Input polygon #%d is a quadrilateral.  Splitting it.", next_face);

	quad_count++;

	unsigned int v[4];
	v[0] = vid_xform(atoi(argv[0])) - 1;
	v[1] = vid_xform(atoi(argv[1])) - 1;
	v[2] = vid_xform(atoi(argv[2])) - 1;
	v[3] = vid_xform(atoi(argv[3])) - 1;

	float e[4][3];
	mxv_unitize(mxv_sub(e[0], m.vertex(v[1]), m.vertex(v[0]), 3), 3);
	mxv_unitize(mxv_sub(e[1], m.vertex(v[2]), m.vertex(v[1]), 3), 3);
	mxv_unitize(mxv_sub(e[2], m.vertex(v[3]), m.vertex(v[2]), 3), 3);
	mxv_unitize(mxv_sub(e[3], m.vertex(v[0]), m.vertex(v[3]), 3), 3);

	float a_02 = (1-mxv_dot(e[0], e[3], 3)) + (1-mxv_dot(e[1], e[2], 3));
	float a_13 = (1-mxv_dot(e[1], e[0], 3)) + (1-mxv_dot(e[3], e[2], 3));

	if( a_02 <= a_13 )
	{
	    next_face++;  m.add_face(v[0], v[1], v[2]);
	    next_face++;  m.add_face(v[0], v[2], v[3]);
	}
	else
	{
	    next_face++;  m.add_face(v[0], v[1], v[3]);
	    next_face++;  m.add_face(v[1], v[2], v[3]);
	}
    }
    else
    {
	mxmsg_signalf(MXMSG_WARN,
		      "Input polygon #%d has more than 4 sides.  Ignoring it.",
		      next_face);
	
	poly_count++;
    }
}

////////////////////////////////////////////////////////////////////////

bool MxSMFReader::execute_command(const MxCmd& cmd, void *closure)
{
    MxStdModel& m = *(MxStdModel *)closure;
    int argc = cmd.phrases[0].length();
    char **argv = (char **)&cmd.phrases[0][0];

    cmd_entry *entry = &read_cmds[0];
    bool handled = false;

    while( entry->name && !handled )
        if( streq(entry->name, cmd.op) )
        {
            (this->*(entry->cmd))(argc, argv, m);
            handled = true;
        }
        else
            entry++;

    if( !handled )
        if( !unparsed_hook || 
            unparsed_hook && !(*unparsed_hook)(cmd.op, argc, argv, m) )
        {
            return false;
        }

    return true;
}

void MxSMFReader::parse_line(char *line, MxStdModel& m)
{
    MxCmdParser::parse_line(line, &m);
}

MxStdModel *MxSMFReader::read(istream& in, MxStdModel *m)
{
    if( !m )
	m = new MxStdModel(8, 16);

    begin(0, NULL, *m);
    while( !in.eof() )
    {
	in >> ws;
	if( in.peek() == '#' )
	    in.ignore(SMF_MAXLINE, '\n');
	else if( in.getline(line_buffer, SMF_MAXLINE, '\n').good() )
	    parse_line(line_buffer, *m);
    }
    end(0, NULL, *m);

    if( quad_count )
	mxmsg_signalf(MXMSG_WARN,
		      "Split %d input quadrilaterals.  "
		      "Auto-splitting does not preserve properties!",
		      quad_count);
    if( poly_count )
	mxmsg_signalf(MXMSG_WARN,
		      "Ignored %d input polygons of more than 4 sides.",
		      poly_count);


    return m;
}



MxSMFWriter::MxSMFWriter()
{
    vertex_annotate = NULL;
    face_annotate = NULL;
}

void MxSMFWriter::write(ostream& out, MxStdModel& m)
{
    unsigned int i;

    out << "begin" << endl;

    for(unsigned int v=0; v<m.vert_count(); v++)
    {
	// We must output all vertices since numbering is implicit
	if( vertex_annotate )  (*vertex_annotate)(out, m, v);
	out << m.vertex(v) << endl;
    }

    for(unsigned int f=0; f<m.face_count(); f++)
	if( m.face_is_valid(f) )
	{
	    if( face_annotate ) (*face_annotate)(out, m, f);
	    out << m.face(f) << endl;
	}

    if( m.normal_binding() != MX_UNBOUND )
    {
	out << "bind n " << m.binding_name(m.normal_binding()) << endl;
	for(i=0; i<m.normal_count(); i++)
	    out << m.normal(i) << endl;
    }

    if( m.color_binding() != MX_UNBOUND )
    {
	out << "bind c " << m.binding_name(m.color_binding()) << endl;
	for(i=0; i<m.color_count(); i++)
	    out << m.color(i) << endl;
    }

    if( m.texcoord_binding() != MX_UNBOUND )
    {
	out << "tex " << m.texmap_name() << endl;
	out << "bind r " << m.binding_name(m.texcoord_binding()) << endl;
	for(i=0; i<m.texcoord_count(); i++)
	    out << m.texcoord(i) << endl;
    }

    out << "end" << endl;
}
