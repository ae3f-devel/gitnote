#ifndef gitnote_link_h
#define gitnote_link_h

#include "./.link.h"

#if GITNOTE_SHARED
#define GITNOTE_ABI_DECL	ae2f_decl
#define GITNOTE_ABI_IMPL	ae2f_impl
#else
#define GITNOTE_ABI_DECL
#define GITNOTE_ABI_IMPL
#endif

#endif
