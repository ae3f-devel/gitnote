#include <gitnote/enum.h>
#include <gitnote/tdpool.h>
#include <stdlib.h>
#include <c89atomic.h>
#include <stdio.h>


GITNOTE_ABI_IMPL gitnote_tdpool_t* g_gitnote_tdpool;

size_t	__count_tdpool;


 GITNOTE_ABI_IMPL int gitnote_alloc_tdpool(size_t c_count)
{
	unless(c_count)	return 0;
	return !((c_count)
			&& ae2f_expected(g_gitnote_tdpool = malloc(sizeof(gitnote_tdpool_t) * c_count))
			&& (__count_tdpool = c_count));
}

 GITNOTE_ABI_IMPL void gitnote_free_tdpool(void) {
	free(g_gitnote_tdpool);
	g_gitnote_tdpool = 0;
	__count_tdpool = 0;
}

ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_	gitnote_tdpool_join(void)
{
	enum AE2FSYS_THRD_	RET;
	int			STAT = 0;
	int			STAT_RET = 0;

	size_t IDX = __count_tdpool;

	while(IDX-- && (2 & c89atomic_load_64(&g_gitnote_tdpool[IDX].m_info.m_atom))) {
		_ae2fsys_join_thrd_imp(L, RET, STAT, g_gitnote_tdpool[IDX].m_info.m_thrd);
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
	size_t IDX = __count_tdpool;

	ae2f_expected_but_else(g_gitnote_tdpool || __count_tdpool)
		return	GITNOTE_NULL_ARG;

	while(IDX-- && !(1 & c89atomic_load_64(&g_gitnote_tdpool[IDX].m_info.m_atom))) {}

	if(IDX + 1) {
		ae2fsys_thrdres_t	STAT = 0;
		enum AE2FSYS_THRD_	RET = 0;

		if(c89atomic_load_64(&g_gitnote_tdpool[IDX].m_info.m_atom) & 2) {
			_ae2fsys_join_thrd_imp(L, RET, STAT, g_gitnote_tdpool[IDX].m_info.m_thrd);
		}

		if(RET) {
			return STAT ? (enum GITNOTE_)STAT : GITNOTE_THREAD_FAILED;
		}

		g_gitnote_tdpool[IDX].m_info.m_act	= call_act;
		g_gitnote_tdpool[IDX].m_info.m_api_key	= rd_api;
		g_gitnote_tdpool[IDX].m_info.m_page_id	= rd_pgid;
		g_gitnote_tdpool[IDX].m_info.m_path	= rd_path;
		g_gitnote_tdpool[IDX].m_info.m_atom	= 3;

		_ae2fsys_mk_thrd_imp(RET
				, g_gitnote_tdpool[IDX].m_info.m_thrd
				, gitnote_tdpool_worker, (ae2fsys_thrdprm_t)(uintptr_t)IDX
				, 1 << 10
				);

		return RET ? GITNOTE_THREAD_FAILED : GITNOTE_SUCCESS;
	}

	return call_act(rd_api, rd_pgid, rd_path);

}
