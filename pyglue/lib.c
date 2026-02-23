#include <ae2f/Keys.h>
#include <assert.h>
#include <gitnote/pyglue.h>
#include <Python.h>


#define	assert_unless(A) assert(A); ae2f_expected_but_else(A)
#define	assrtunless	assert_unless

/** extern */
PyObject* PyInit_utils(void);

ae2f_extern GITNOTE_ABI_IMPL int gitnote_pyglue_init(
		const char* const rd_venv
		) {
	PyStatus status;

	PyConfig config;
	PyConfig_InitPythonConfig(&config);

	{
		const size_t VENV_LEN	= strlen(rd_venv);
		char* VENV_PY		= malloc(VENV_LEN + sizeof("/bin/python"));
		ae2f_expected_but_else(VENV_PY) goto done;
		strcpy(VENV_PY, rd_venv);
		strcat(VENV_PY, "/bin/python");

		status = PyConfig_SetBytesString(
				&config
				, &config.executable
				, VENV_PY);
		if (PyStatus_Exception(status)) {
			free(VENV_PY);
			goto done;
		}

		free(VENV_PY);
	}

	config.use_environment = 1;
	config.dev_mode = 1;
	config.verbose = 1;

	PyImport_AppendInittab("utils", PyInit_utils);

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
