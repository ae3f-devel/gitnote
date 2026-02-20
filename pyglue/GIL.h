#ifndef PYGLUE_GIL_H
#define PYGLUE_GIL_H
#include <Python.h>

#define GITNOTE_PYGIL_DECLARE PyGILState_STATE _gitnote_gilstate
#define GITNOTE_PYGIL_ENTER() _gitnote_gilstate = PyGILState_Ensure()
#define GITNOTE_PYGIL_EXIT() PyGILState_Release(_gitnote_gilstate)

#endif
