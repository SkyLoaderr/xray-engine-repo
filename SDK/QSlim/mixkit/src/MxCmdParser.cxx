/************************************************************************

  MxCmdParser

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxCmdParser.cxx,v 1.5 1998/10/26 21:08:44 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "mixio.h"
#include "MxCmdParser.h"

#include "MxDynBlock.h"
#include <ctype.h>

typedef MxDynBlock<char *> string_list;

#define MXCMD_PHRASE_SEPARATOR ';'

static
char *trimleft(char *line)
{
    if( !line ) return NULL;
    while(isspace(*line)) line++;
    return line;
}

static
char *shift_token(char *line)
{
    if( !line )  return NULL;

    char *rest = NULL;

    while( *line && !isspace(*line) )  line++;

    if( *line )  rest = line+1;

    *line = '\0';

    return trimleft(rest);
}

static
char *shift_phrase(char *line)
{
    if( !line )  return NULL;

    char *rest = NULL;

    while( *line && *line != MXCMD_PHRASE_SEPARATOR ) line++;

    if( *line )  rest = line+1;

    *line = '\0';

    return trimleft(rest);
}

MxCmdParser::MxCmdParser()
    : cmd(16)
{
    will_ignore_unknown = true;
}

void MxCmdParser::parse_line(char *line, void *closure)
{
    uint i,j;

    line = trimleft(line);

    if(!line[0] || line[0] == '#')  return; // Ignore comments and empty lines

    // Extract operator
    //
    char *op = line;   line = shift_token(line);

    // Extract phrases
    //
    string_list phrases;

    while( line )
    {
        phrases.add(line);
        line = shift_phrase(line);
    }

    cmd.phrases.reset();
    cmd.op = op;

    // Extract args in phrases
    //
    string_list args;

    for(i=0; i<phrases.length(); i++)
    {
        char *phrase = phrases[i];
        args.reset();

        while( phrase )
        {
            char *arg = phrase;
            phrase = shift_token(phrase);
            if( *arg )  args.add(arg); // trailing space doesn't count
        }

	cmd.phrases.add();
	cmd.phrases[i].reset();
        if( args.length() )
        {
            for(j=0; j<args.length(); j++)
                cmd.phrases[i].add(args[j]);
        }
    }

    bool result = execute_command(cmd, closure);

    if( !result && phrases.length()==1 )
	result = store.execute_command(cmd.phrases[0].length(), cmd.op,
				       (const char **)&cmd.phrases[0][0]);

    if( !result && !will_ignore_unknown )
	mxmsg_signal(MXMSG_WARN, "Unknown operator on input", line);
}

bool MxCmdParser::execute_command(const MxCmd& cmd, void *)
{
    return false;
}
