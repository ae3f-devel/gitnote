#include <gitnote.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <easyredir.h>
#include <utils.h>
#include "./GIL.h"

static PyObject* _get_notion_client(const char *api_key) {
    static PyObject *client = NULL;
    assert(Py_IsInitialized());
    if (!client) {
        client = __pyx_pf_5utils_init_notion_client(NULL, PyUnicode_FromString(api_key));
    }
    return client;
}

ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_act_creat(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const char* const rd_path
		) {
	GITNOTE_PYGIL_DECLARE;
	PyObject *client;
	PyObject *result;
	const char *filename;
	const char *dir_path;
	PyObject *py_filename;
	PyObject *py_dir_path;
	PyObject *py_root_id;
	PyObject *py_parent_id;

	GITNOTE_PYGIL_ENTER();

	client = _get_notion_client(rd_notion_api_key);
	py_root_id = PyUnicode_FromString(rd_notion_page_id);

	filename = strrchr(rd_path, '/');
	if (filename) {
		size_t dir_len = filename - rd_path;
		char *dir = malloc(dir_len + 1);
		memcpy(dir, rd_path, dir_len);
		dir[dir_len] = '\0';
		py_dir_path = PyUnicode_FromString(dir);
		free(dir);

		result = __pyx_pf_5utils_6create_page_by_path(
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
	result = __pyx_pf_5utils_6create_page_by_path(
		NULL, client, py_parent_id, py_filename,
		Py_False, Py_False);

	Py_DECREF(py_filename);
	Py_DECREF(py_root_id);

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

	client = _get_notion_client(rd_notion_api_key);
	py_root_id = PyUnicode_FromString(rd_notion_page_id);
	py_path = PyUnicode_FromString(rd_path);

	result = __pyx_pf_5utils_2find_page_by_path(
		NULL, client, py_root_id, py_path, Py_False);

	if (result && result != Py_None) {
		__pyx_pf_5utils_14archive_page_recursive(NULL, client, result, Py_False);
	}

	Py_DECREF(py_path);
	Py_DECREF(py_root_id);

	fputs("D	", stdout);
	puts(rd_path);

	GITNOTE_PYGIL_EXIT();
	return GITNOTE_SUCCESS;
}

ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_act_mod(
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

	client = _get_notion_client(rd_notion_api_key);
	py_root_id = PyUnicode_FromString(rd_notion_page_id);
	py_path = PyUnicode_FromString(rd_path);
	py_filepath = PyUnicode_FromString(rd_path);

	result = __pyx_pf_5utils_2find_page_by_path(
		NULL, client, py_root_id, py_path, Py_False);

	if (result && result != Py_None) {
		content = __pyx_pf_5utils_18read_file_content(NULL, py_filepath);
		file_type = __pyx_pf_5utils_8detect_file_type(NULL, py_filepath);

		__pyx_pf_5utils_16update_page_content(
			NULL, client, result, content, file_type,
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
