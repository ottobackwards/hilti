// $Id$
//
// Internal BinPAC++ API funnctionaly supporting the generated parsers.

#include <stdio.h>
#include <stdlib.h>

#include "binpac.h"

static hlt_list* _parsers = 0;
static int _initalized = 0;

extern hlt_type_info hlt_type_info_binpac_parser;

static _ensure_parsers(hlt_exception** excpt)
{
    if ( ! _parsers )
        _parsers = hlt_list_new(&hlt_type_info_binpac_parser, excpt);
}

// Public API functions.

void binpac_init()
{
    _initalized = 1;
}

const hlt_list* binpac_parsers(hlt_exception** excpt)
{
    _ensure_parsers(excpt);
    return _parsers;
}

// Internal functions. 
 
void binpac_fatal_error(const char* msg)
{
    fprintf(stderr, "fatal binpac error: %s", msg);
}

// Note that this function can be called before binpac_init(). 
void register_parser(binpac_parser* parser, hlt_exception** excpt)
{
    _ensure_parsers(excpt);
    hlt_list_push_back(_parsers, &hlt_type_info_binpac_parser, parser, excpt)
}

        
