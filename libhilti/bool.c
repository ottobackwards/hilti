//
// Support functions HILTI's bool data type.
//

#include <assert.h>

#include "context.h"
#include "rtti.h"
#include "string_.h"

hlt_string hlt_bool_to_string(const hlt_type_info* type, const void* obj, int32_t options,
                              __hlt_pointer_stack* seen, hlt_exception** excpt,
                              hlt_execution_context* ctx)
{
    assert(type->type == HLT_TYPE_BOOL);
    int8_t b = *((int8_t*)obj);

    return hlt_string_from_asciiz((b ? "True" : "False"), excpt, ctx);
}

int64_t hlt_bool_to_int64(const hlt_type_info* type, const void* obj, int32_t options,
                          hlt_exception** expt, hlt_execution_context* ctx)
{
    assert(type->type == HLT_TYPE_BOOL);
    int8_t b = *((int8_t*)obj);

    return b ? 1 : 0;
}
