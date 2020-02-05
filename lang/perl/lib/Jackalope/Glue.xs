#define PERL_NO_GET_CONTEXT // we'll define thread context if necessary (faster)
#include "EXTERN.h"         // globals/constant import locations
#include "perl.h"           // Perl symbols, structures and constants definition
#include "XSUB.h"           // xsubpp functions and macros

#include <jackalope/foreign.h>

MODULE = Jackalope::Glue  PACKAGE = Jackalope::Glue
PROTOTYPES: ENABLE

void
jackalope_init()

void
jackalope_object_subscribe(struct jackalope_object_t * object_in, const char * signal_in, struct jackalope_object_t * target_in, const char * slot_in)

void
jackalope_object_start(struct jackalope_object_t * object_in)

void
jackalope_object_stop(struct jackalope_object_t * object_in)

struct jackalope_object_t *
_jackalope_graph_make(char * strings_in)
CODE:
    RETVAL = jackalope_graph_make((const char **) strings_in);
OUTPUT:
    RETVAL

struct jackalope_object_t *
_jackalope_graph_add_node(struct jackalope_object_t * object_in, char * strings_in)
CODE:
    RETVAL = jackalope_graph_add_node(object_in, (const char **) strings_in);
OUTPUT:
    RETVAL

void
jackalope_graph_run(struct jackalope_object_t * graph_in)

unsigned int
jackalope_node_get_num_sources(struct jackalope_object_t * object_in)

unsigned int
jackalope_node_get_num_sinks(struct jackalope_object_t * object_in)

void
jackalope_node_link(struct jackalope_object_t * object_in, const char * source_in, struct jackalope_object_t * target_in, const char * sink_in)
