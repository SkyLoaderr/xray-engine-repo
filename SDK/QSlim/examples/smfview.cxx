/************************************************************************

  SMFView

  This is a simple example program which will display an SMF model and
  allow the user to interact with it.  It can also be used as a simple
  prototype for writing more complex interactive programs.

  $Id: smfview.cxx,v 1.7 2000/02/11 19:41:29 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <MxStdGUI.h>

MxStdGUI gui;

main(int argc, char **argv)
{
    gui.initialize(argc, argv);
    return gui.run();
}
