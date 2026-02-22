#include <ae2f/Keys.h>
#include <assert.h>
#include <gitnote/pyglue.h>
#include <Python.h>


#define	assert_unless(A) assert(A); ae2f_expected_but_else(A)
#define	assrtunless	assert_unless


ae2f_extern GITNOTE_ABI_IMPL int gitnote_pyglue_init(
		const char* const rd_venv
		) {
	PyStatus status;

	PyConfig config;
	PyConfig_InitPythonConfig(&config);

#if 1
	/* Set the program name before reading the configuration
	   (decode byte string from the locale encoding).

	   Implicitly preinitialize Python. */

	{
		const size_t VENV_LEN	= strlen(rd_venv);
		char* VENV_PY		= malloc(VENV_LEN + sizeof("/bin/python"));
		ae2f_expected_but_else(VENV_PY) goto done;
		strcpy(VENV_PY, rd_venv);
		strcat(VENV_PY, "/bin/python");

		status = PyConfig_SetBytesString(&config, &config.executable,
				VENV_PY);
		if (PyStatus_Exception(status)) {
			free(VENV_PY);
			goto done;
		}

		free(VENV_PY);
	}


#if 0

	/* Set the program name before reading the configuration
	   (decode byte string from the locale encoding).

	   Implicitly preinitialize Python. */
	status = PyConfig_SetBytesString(&config, &config.prefix,
			rd_venv);
	if (PyStatus_Exception(status)) {
		goto done;
	}

#endif

#else
	size_t	VENV_LEN = strlen(rd_venv);
	wchar_t*	VENV_WCS = malloc((VENV_LEN << 1) * sizeof(wchar_t));
	assert_unless(VENV_WCS) return 1;
	mbstowcs(VENV_WCS, rd_venv, (VENV_LEN << 1) * sizeof(wchar_t));

	status = PyWideStringList_Append(&config.module_search_paths, VENV_WCS);
	if (PyStatus_Exception(status)) {
		goto done;
	}
#endif
	config.use_environment = 1;
	config.isolated = 0;
	config.dev_mode = 1;
	config.verbose = 1;

	status = Py_InitializeFromConfig(&config);

	assert(Py_IsInitialized());
	puts("GOOD");
	PyConfig_Clear(&config);
	return 0;
done:
	assert(!"asdf");
	PyConfig_Clear(&config);
	return 0;
}
