// pysv.i

%module pysv

%begin %{
    #define SWIG_FILE_WITH_INIT
%}

// %include "std_string.i"

%{
#include "pysv.h"
%}

%include "pysv.h"

