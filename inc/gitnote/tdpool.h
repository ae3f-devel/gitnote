#ifndef gitnote_tdpool_h
#define gitnote_tdpool_h

#include <gitnote/enum.h>
#include <gitnote/link.h>

#include <ae2f/Sys/Thrd.h>
#include <ae2f/Keys.h>
#include <ae2f/c90/StdInt.h>

typedef enum GITNOTE_ (*fp_gitnote_act_t)(
		const char *const ae2f_restrict rd_notion_api_key, 
		const char *const ae2f_restrict rd_notion_page_id, 
		const char *const ae2f_restrict rd_path
		);

typedef union {
	struct {
		ae2fsys_thrd	m_thrd;
		const char* ae2f_restrict m_api_key;
		const char* ae2f_restrict m_page_id;
		const char* ae2f_restrict m_path;
		fp_gitnote_act_t	m_act;
		uint_least64_t		m_atom;
	}	m_info;
	int	m_some[1 << 10];
} gitnote_tdpool_t;

ae2f_extern GITNOTE_ABI_DECL gitnote_tdpool_t* g_gitnote_tdpool;

ae2f_extern GITNOTE_ABI_DECL int gitnote_alloc_tdpool(size_t c_count);
ae2f_extern GITNOTE_ABI_DECL void gitnote_free_tdpool(void);
ae2f_extern GITNOTE_ABI_DECL enum GITNOTE_	gitnote_tdpool_join(void);
ae2f_extern GITNOTE_ABI_DECL enum GITNOTE_	gitnote_tdpool_push(
		fp_gitnote_act_t call_act,
		const char* ae2f_restrict rd_api,
		const char* ae2f_restrict rd_pgid,
		const char* ae2f_restrict rd_path
		);


ae2f_extern GITNOTE_ABI_DECL ae2fsys_thrdres_t	gitnote_tdpool_worker(ae2fsys_thrdprm_t h);

#endif
