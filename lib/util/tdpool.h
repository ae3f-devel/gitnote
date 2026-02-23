#ifndef lib_tdpool_h
#define lib_tdpool_h

#include <gitnote/enum.h>
#include <gitnote/link.h>

#include <ae2f/Sys/Thrd.h>
#include <ae2f/Keys.h>

typedef enum GITNOTE_ (*fp_gitnote_act_t)(
		const char *const ae2f_restrict rd_notion_api_key, 
		const char *const ae2f_restrict rd_notion_page_id, 
		const char *const ae2f_restrict rd_path
		);

ae2f_extern GITNOTE_ABI_DECL int gitnote_alloc_tdpool(size_t c_count);
ae2f_extern GITNOTE_ABI_DECL void gitnote_free_tdpool(void);
ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_	gitnote_tdpool_join(void);
ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_	gitnote_tdpool_push(
		fp_gitnote_act_t call_act,
		const char* ae2f_restrict rd_api,
		const char* ae2f_restrict rd_pgid,
		const char* ae2f_restrict rd_path
		);

#endif
