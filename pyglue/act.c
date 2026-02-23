
#define main static __pymain

#include <ae2f/cc.h>
#include <gitnote.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyredir.h>
#include <utils.h>
#include "./GIL.h"
#include <assert.h>
#include <gitnote/snap.h>

#define	assert_unless(A) assert(A); ae2f_expected_but_else(A)
#define	assrtunless	assert_unless
static PyObject *__client = NULL;

ae2f_extern GITNOTE_ABI_IMPL int gitnote_pyglue_init(
		const char* const rd_venv,
		const char* const rd_api
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

	PyImport_ImportModule("utils");
	unless(__pyx_mstate_global) assert(!"global non-initialised");
	/** at least global thing is okay. */
	unless(__pyx_mstate_global->__pyx_n_u_Client) assert(!"global non-initialised: __pyx_n_u_Client");
	unless(__pyx_f_5utils_init_notion_client) {
		PyErr_Print();
		assert(!"Cython function not available");
	}


	__client = __pyx_f_5utils_init_notion_client(PyUnicode_FromString(rd_api), 0);

	return 0;
done:
	assert(!"asdf");
	PyConfig_Clear(&config);
	return 0;
}


PyObject* gitnote_get_notion_client(const char *api_key) {
	return __client;
}

GITNOTE_ABI_IMPL int gitnote_set_snap(const char* const rd_api, const char* const rd_pgid, const char* const rd_hash) 
{
	PyObject* API = PyUnicode_FromString(rd_api);
	PyObject* PGID = PyUnicode_FromString(rd_pgid);
	PyObject* CLIENT = gitnote_get_notion_client(rd_api);
	PyObject* HASH = PyUnicode_FromString(rd_hash);

	const int RET = __pyx_f_5utils_set_snap(
			CLIENT
			, PGID
			, HASH
			, 0
			, 0);

	Py_DECREF(API);
	Py_DECREF(PGID);
	Py_DECREF(CLIENT);
	Py_DECREF(HASH);

	return RET;
}

GITNOTE_ABI_IMPL void gitnote_free_snap(gitnote_snap_t d) {
	Py_DECREF(d.m_handle);
}

GITNOTE_ABI_IMPL gitnote_snap_t gitnote_get_snap(
		const char* const rd_api
		, const char* const rd_pgid
		)
{
	PyObject* API = PyUnicode_FromString(rd_api);
	PyObject* PGID = PyUnicode_FromString(rd_pgid);
	PyObject* CLIENT = gitnote_get_notion_client(rd_api);
	PyObject* HASH = __pyx_f_5utils_get_snap(
			CLIENT
			, PGID
			, 0
			, 0
			);

	gitnote_snap_t	RET;

	RET.m_handle = HASH;
	RET.m_hash = PyUnicode_AsUTF8(HASH);
	return RET;
}

GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_act_creat(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const char* const rd_path
		) {
	GITNOTE_PYGIL_DECLARE;
	PyObject *client;
	PyObject *result;
	PyObject *content;
	PyObject *file_type;
	const char *filename;
	const char *dir_path;
	PyObject *py_filename;
	PyObject *py_dir_path;
	PyObject *py_root_id;
	PyObject *py_parent_id;
	PyObject *py_filepath;

	GITNOTE_PYGIL_ENTER();

	client = gitnote_get_notion_client(rd_notion_api_key);
	py_root_id = PyUnicode_FromString(rd_notion_page_id);
	py_filepath = PyUnicode_FromString(rd_path);
	assert(py_root_id);

	filename = strrchr(rd_path, '/');
	if (filename) {
		size_t dir_len = filename - rd_path;
		char *dir = malloc(dir_len + 1);
		memcpy(dir, rd_path, dir_len);
		dir[dir_len] = '\0';
		py_dir_path = PyUnicode_FromString(dir);
		free(dir);

		result = __pyx_pf_5utils_10create_page_by_path(
				NULL, client, py_root_id, py_dir_path,
				Py_True, Py_False);

		Py_DECREF(py_dir_path);

		py_parent_id = result;
		filename++;
	} else {
		py_parent_id = py_root_id;
		filename = rd_path;
	}

	py_filename = PyUnicode_FromString(filename);
	result =  __pyx_pf_5utils_10create_page_by_path(
			NULL, client, py_parent_id, py_filename,
			Py_False, Py_False);

	if (!result) {
		PyErr_Print();
		fprintf(stderr, "Error: create_page_by_path returned NULL for '%s'\n", filename);
	} else {
		PyObject *page_id = result;
		if (PyTuple_Check(result)) {
			page_id = PyTuple_GetItem(result, 0);
		}

		content = __pyx_pf_5utils_25read_file_content(NULL, py_filepath);
		file_type = __pyx_pf_5utils_12detect_file_type(NULL, py_filepath);

		if (content && file_type) {
			__pyx_pf_5utils_23update_page_content(
					NULL
					, client
					, page_id
					, content
					, file_type
					,Py_False
					, py_filepath
					, py_root_id
					);

			Py_DECREF(content);
			Py_DECREF(file_type);
		}
	}

	Py_DECREF(py_filename);
	Py_DECREF(py_root_id);
	Py_DECREF(py_filepath);

	fputs("A	", stdout);
	puts(rd_path);

	GITNOTE_PYGIL_EXIT();
	return GITNOTE_SUCCESS;
}

GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_rm(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const char* const rd_path
		) {
	GITNOTE_PYGIL_DECLARE;
	PyObject *client;
	PyObject *result;
	PyObject *py_root_id;
	PyObject *py_path;

	GITNOTE_PYGIL_ENTER();

	client = gitnote_get_notion_client(rd_notion_api_key);
	py_root_id = PyUnicode_FromString(rd_notion_page_id);
	py_path = PyUnicode_FromString(rd_path);

	result = __pyx_pf_5utils_6find_page_by_path(
			NULL
			, client
			, py_root_id
			, py_path
			, Py_False
			);

	if (result && result != Py_None) {
		PyObject *page_id = result;
		if (PyTuple_Check(result)) {
			page_id = PyTuple_GetItem(result, 0);
		}
		__pyx_pf_5utils_21archive_page_recursive(
				NULL
				, client
				, page_id
				, Py_False
				);
	}

	Py_DECREF(py_path);
	Py_DECREF(py_root_id);

	fputs("D	", stdout);
	puts(rd_path);

	GITNOTE_PYGIL_EXIT();
	return GITNOTE_SUCCESS;
}

GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_act_mod(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const char* const rd_path
		) {
	GITNOTE_PYGIL_DECLARE;
	PyObject *client;
	PyObject *result;
	PyObject *content;
	PyObject *file_type;
	PyObject *py_root_id;
	PyObject *py_path;
	PyObject *py_filepath;

	GITNOTE_PYGIL_ENTER();

	client = gitnote_get_notion_client(rd_notion_api_key);
	py_root_id = PyUnicode_FromString(rd_notion_page_id);
	py_path = PyUnicode_FromString(rd_path);
	py_filepath = PyUnicode_FromString(rd_path);

	result = __pyx_pf_5utils_6find_page_by_path(
			NULL, client, py_root_id, py_path, Py_False);

	if (result && result != Py_None) {
		PyObject *page_id = result;
		if (PyTuple_Check(result)) {
			page_id = PyTuple_GetItem(result, 0);
		}
		content = __pyx_pf_5utils_25read_file_content(NULL, py_filepath);
		file_type = __pyx_pf_5utils_12detect_file_type(NULL, py_filepath);

		__pyx_pf_5utils_23update_page_content(
				NULL, client, page_id, content, file_type,
				Py_False, py_filepath, py_root_id);

		Py_DECREF(content);
		Py_DECREF(file_type);
	}

	Py_DECREF(py_filepath);
	Py_DECREF(py_path);
	Py_DECREF(py_root_id);

	fputs("M	", stdout);
	puts(rd_path);

	GITNOTE_PYGIL_EXIT();
	return GITNOTE_SUCCESS;
}
