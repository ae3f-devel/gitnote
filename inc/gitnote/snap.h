#ifndef gitnote_snap_h
#define gitnote_snap_h

#include "./link.h"
#include <ae2f/Keys.h>

typedef struct {
	const char*	m_hash;
	void*		m_handle;
} gitnote_snap_t;

ae2f_extern GITNOTE_ABI_DECL gitnote_snap_t 
gitnote_get_snap(
		const char* const rd_api
		, const char* const rd_pgid
		);


ae2f_extern GITNOTE_ABI_DECL 
int gitnote_set_snap(
		const char* const rd_api, 
		const char* const rd_pgid, 
		const char* const rd_hash
		);

ae2f_extern GITNOTE_ABI_DECL void gitnote_free_snap(
		gitnote_snap_t d);

#endif
