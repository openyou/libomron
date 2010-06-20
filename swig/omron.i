%module omron
%{
#include "omron.h"
#include "Omron.h"
%}

%include "std_string.i"
%include "carrays.i"

 // to access unknown_*[]
%array_functions(unsigned char, CharArray);

%ignore omron_get_daily_bp_data_count;
%rename(get_count) omron_get_count;

%include "omron.h"
%include "Omron.h"
