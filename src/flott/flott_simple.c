#include <stdio.h>
#include <stdlib.h>

#include "flott.h"
#include "flott_term.h"
#include "flott_util.h"


double flott_simple_get_entropy(char *bytes, size_t length) {
    flott_object *op = flott_create_instance(1);
    flott_source *source = &(op->input.source[0]);
    source->storage_type = FLOTT_DEV_MEM;
    source->length = length;
    source->data.bytes = bytes;
    flott_initialize(op);
    flott_t_transform(op);
    double information = op->result.t_information;
    flott_destroy(op);
    return information;
}
