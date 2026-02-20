#include <ae2f/Keys.h>
#include <assert.h>
#include <gitnote/pyglue.h>
#include <Python.h>

#define	assert_unless(A) assert(A); ae2f_expected_but_else(A)
#define	assrtunless	assert_unless

GITNOTE_ABI_IMPL int gitnote_pyglue_init(
		const char* const rd_venv
		) {
	assert_unless(rd_venv) return 5;

	{
		PyInitConfig*	INITCFG = PyInitConfig_Create();

		assrtunless(INITCFG) {
			PyInitConfig_Free(INITCFG);
			return 1;
		}

		ae2f_unexpected_but_if((PyInitConfig_SetInt(INITCFG, "isolated", 1)))
		{
			PyInitConfig_Free(INITCFG);
			return 2;
		}

		ae2f_unexpected_but_if(PyInitConfig_SetStr(INITCFG, "home", rd_venv))
			PyInitConfig_Free(INITCFG);
		return 3;

		ae2f_unexpected_but_if(Py_InitializeFromInitConfig(INITCFG)) {
			PyInitConfig_Free(INITCFG);
			return 4;
		}

		PyInitConfig_Free(INITCFG);

		return 0;
	}
}
