%module omron
%{
#include "libomron/omron.h"
#include "Omron.h"
%}

%include "std_string.i"
%include "carrays.i"

 // to access unknown_*[]
%array_functions(unsigned char, CharArray);

%ignore omron_get_daily_bp_data_count;
%ignore OMRON_VID;		/* access as just VID */
%ignore OMRON_PID;		/* access as just PID */

%rename(get_count) omron_get_count;
%rename(create_device) omron_create;
%rename(delete_device) omron_delete;

%typemap(in) uint32_t = int;
%typemap(out) uint32_t = int;

%include "libomron/omron.h"
%include "Omron.h"
