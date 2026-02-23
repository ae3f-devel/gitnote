#include "./tdpool.h"
#include "gitnote/enum.h"
#include <stdlib.h>
#include <c89atomic.h>

static union __tdpool {
	struct {
		ae2fsys_thrd	m_thrd;
		const char* ae2f_restrict m_api_key;
		const char* ae2f_restrict m_page_id;
		const char* ae2f_restrict m_path;
		fp_gitnote_act_t	m_act;
		uint_least64_t		m_atom;
	}	m_info;
	int	m_some[1 << 10];
}* __tdpool;

size_t	__count_tdpool;

static ae2fsys_thrdres_t	__worker(ae2fsys_thrdprm_t h) {
#define	C	__tdpool[((uintptr_t)(h))]
	c89atomic_fetch_xor_64(&C.m_info.m_atom, 1);

	C.m_info.m_act(
			C.m_info.m_api_key
			, C.m_info.m_page_id
			, C.m_info.m_path);

	c89atomic_fetch_xor_64(&C.m_info.m_atom, 1);
#undef	C
	return 0;
}

ae2f_extern GITNOTE_ABI_IMPL int gitnote_alloc_tdpool(size_t c_count)
{
	return !((c_count) 
			&& ae2f_expected(__tdpool = malloc(sizeof(union __tdpool) * c_count))
			&& (__count_tdpool = c_count));
}

ae2f_extern GITNOTE_ABI_IMPL void gitnote_free_tdpool(void) {
	free(__tdpool);
	__tdpool = 0;
	__count_tdpool = 0;
}

ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_	gitnote_tdpool_join(void)
{
	enum AE2FSYS_THRD_	RET;
	int			STAT = 0;
	int			STAT_RET = 0;

	size_t IDX = __count_tdpool;

	while(IDX--) {
		_ae2fsys_join_thrd_imp(L, RET, STAT, __tdpool[IDX].m_info.m_thrd);
		if(!STAT_RET && !STAT && RET) {
			STAT_RET = GITNOTE_THREAD_FAILED;
		}

		if(!STAT_RET && STAT) {
			STAT_RET = STAT;
		}
	}

	return (enum GITNOTE_)STAT_RET;
}

ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_tdpool_push(
		fp_gitnote_act_t call_act, 
		const char* ae2f_restrict rd_api, 
		const char* ae2f_restrict rd_pgid,
		const char* ae2f_restrict rd_path
		)
{
	enum AE2FSYS_THRD_	RET;

	size_t IDX = __count_tdpool;

	ae2f_expected_but_else(__tdpool)
		return	GITNOTE_NULL_ARG;

	while(IDX-- && !c89atomic_load_64(&__tdpool[IDX].m_info.m_atom)) {
		ae2fsys_thrdres_t	STAT = 0;
		_ae2fsys_join_thrd_imp(L, RET, STAT, __tdpool[IDX].m_info.m_thrd);
		if(RET) {
			return STAT ? (enum GITNOTE_)STAT : GITNOTE_THREAD_FAILED;
		}

		__tdpool[IDX].m_info.m_act	= call_act;
		__tdpool[IDX].m_info.m_api_key	= rd_api;
		__tdpool[IDX].m_info.m_page_id	= rd_pgid;
		__tdpool[IDX].m_info.m_path	= rd_path;
		_ae2fsys_mk_thrd_imp(RET
				, __tdpool[IDX].m_info.m_thrd
				, __worker, (ae2fsys_thrdprm_t)(uintptr_t)IDX
				, 1 << 10
				);

		return RET ? GITNOTE_THREAD_FAILED : GITNOTE_SUCCESS;
	}

	return call_act(rd_api, rd_pgid, rd_path);
}
