#include <gitnote/tdpool.h>
#include <ae2f/Sys/Thrd.h>
#include <c89atomic.h>
#include <python3.14/Python.h>
#include <ae2f/Sys/Ftx.h>


GITNOTE_ABI_IMPL ae2fsys_thrdres_t	gitnote_tdpool_worker(ae2fsys_thrdprm_t h) {
#define	C	g_gitnote_tdpool[((uintptr_t)(h))]
	PyGILState_STATE GIL;
	static ae2fsys_ftxel_t	FTX = 0;

	GIL = PyGILState_Ensure();
	c89atomic_fetch_xor_64(&C.m_info.m_atom, 1);

	C.m_info.m_act(
			C.m_info.m_api_key
			, C.m_info.m_page_id
			, C.m_info.m_path);

	c89atomic_fetch_xor_64(&C.m_info.m_atom, 1);
	PyGILState_Release(GIL);
#undef	C

	return 0;
}
