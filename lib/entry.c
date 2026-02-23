#include <string.h>

#include <gitnote.h>
#include <gitnote/enum.h>
#include <gitnote/pyglue.h>
#include <gitnote/snap.h>
#include <gitnote/tdpool.h>

#include "./util/assert_unless.h"
#include "./util/rdallfp.h"

#include "./cfg/.TMPFILE_NAME.h"
#include "./cfg/.HASH_LENGTH.h"


GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_entry(
		const char* const		rd_branch_name,
		const gitnote_commit_hash_str_t rd_begin,
		const gitnote_commit_hash_str_t rd_breakpoint,
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const B_gitnote_option_t	c_flags,
		const char* const		rd_venv,
		unsigned			c_count_threads
		)
{
	struct {
		unsigned m_is_alloc_branch	: 1;
		unsigned m_is_alloc_lsfiles	: 1;
		unsigned m_is_naked		: 1;
	} CONTEXT = {0, };

	union {
		char* ae2f_restrict m_buf;
		const char* ae2f_restrict m_str;
	} BRANCH;

	typedef char hash_strbuf_t[CFG_HASH_LENGTH + 1];
	hash_strbuf_t		BEG, BREAKPOINT;
	char*			BUF_LSFILES = 0;

	enum GITNOTE_			STATE_ON_ERR = GITNOTE_SUCCESS;

	/****************************************************************************/

	assert_unless(rd_branch_name)		return GITNOTE_NULL_ARG;
	assert_unless(rd_begin)			return GITNOTE_NULL_ARG;
	assert_unless(rd_breakpoint)		return GITNOTE_NULL_ARG;
	assert_unless(rd_venv)			return GITNOTE_NULL_ARG;

	if(gitnote_pyglue_init(rd_venv, rd_notion_api_key))
		return GITNOTE_PYINIT_FAILED;

	if(gitnote_alloc_tdpool(c_count_threads))
		return GITNOTE_THREAD_FAILED;

	unless(*(BRANCH.m_str = rd_branch_name))	{
		const char* ARGS[] = { "rev-parse", "--abbrev-ref", "HEAD" };

		ae2f_expected_but_else(BRANCH.m_buf = gitnote_get_default(
				CFG_TMPFILE_NAME
				, "git"
				, sizeof(ARGS) / sizeof(ARGS[0])
				, ARGS
				)) return GITNOTE_ALLOC_FAILED;

		CONTEXT.m_is_alloc_branch = 1;
		strtok(BRANCH.m_buf, "\n");
	}

#define	jmpret(a)	{ STATE_ON_ERR = (a); goto LBL_NONGOOD; }

	if(*rd_begin) {
		strncpy(BEG, rd_begin, CFG_HASH_LENGTH);
	}
	else {
		const char* ARGS[] = { "rev-list", "--max-parents=0", 0 };
		char*	BUF;

		gitnote_snap_t	SNAP = gitnote_get_snap(rd_notion_api_key, rd_notion_page_id);

		unless(SNAP.m_handle && SNAP.m_hash && SNAP.m_hash[0]) {
			ARGS[2] = BRANCH.m_str;
			ae2f_expected_but_else(BUF = gitnote_get_default(
						CFG_TMPFILE_NAME
						, "git"
						, sizeof(ARGS) / sizeof(ARGS[0])
						, ARGS
						)) jmpret(GITNOTE_ALLOC_FAILED);

			strncpy(BEG, BUF, CFG_HASH_LENGTH);
			strtok(BEG, "\n");
			free(BUF);
			c_flags & GITNOTE_OPT_INIT && (CONTEXT.m_is_naked = 1);
		} else {
			ARGS[2] = BRANCH.m_str;
			strncpy(BEG, SNAP.m_hash, CFG_HASH_LENGTH);
			gitnote_free_snap(SNAP);
		}
	}



	if(*rd_breakpoint) {
		strncpy(BREAKPOINT, rd_breakpoint, CFG_HASH_LENGTH);
	} else {
		const char* ARGS[] = { "rev-parse", 0 };

		char*	BUF;

		ARGS[1] = BRANCH.m_str;

		ae2f_expected_but_else(BUF = gitnote_get_default(
					CFG_TMPFILE_NAME
					, "git"
					, sizeof(ARGS) / sizeof(ARGS[0])
					, ARGS
					)) jmpret(GITNOTE_ALLOC_FAILED);

		strncpy(BREAKPOINT, BUF, CFG_HASH_LENGTH);
		free(BUF);
		strtok(BREAKPOINT, "\n");
	}

	/************************************************************************/

	if(CONTEXT.m_is_naked) {
		const char* ARGS[] = { "ls-files" };

		ae2f_expected_but_else(BUF_LSFILES = gitnote_get_default(
					CFG_TMPFILE_NAME
					, "git"
					, sizeof(ARGS) / sizeof(ARGS[0])
					, ARGS
					)) jmpret(GITNOTE_ALLOC_FAILED);

		CONTEXT.m_is_alloc_lsfiles = 1;

	} else {
		const char* ARGS[] = { "--no-pager", "diff", "--name-status", 0 };
		char BUF_TWOCOMMITS[(CFG_HASH_LENGTH << 1) + 3];
		strcpy(BUF_TWOCOMMITS, BEG);
		strcat(BUF_TWOCOMMITS, "..");
		strcat(BUF_TWOCOMMITS, BREAKPOINT);
		ARGS[3] = BUF_TWOCOMMITS;
		ae2f_expected_but_else(BUF_LSFILES = gitnote_get_default(
					CFG_TMPFILE_NAME
					, "git"
					, sizeof(ARGS) / sizeof(ARGS[0])
					, ARGS
					)) jmpret(GITNOTE_ALLOC_FAILED);

		CONTEXT.m_is_alloc_lsfiles = 1;
	}

	puts("********************************");
	fputs(BEG, stdout);
	fputs("..", stdout);
	puts(BREAKPOINT);
	puts(BUF_LSFILES);
	puts("********************************");

	/****************************************************************************/
	{
		const size_t	LEN_LSFILES = strlen(BUF_LSFILES);
		char* ae2f_restrict TOKEN = strtok(BUF_LSFILES, "\n");
		while (TOKEN && TOKEN < LEN_LSFILES + BUF_LSFILES)
		{
			ae2f_unexpected_but_if((STATE_ON_ERR = gitnote_invoke_act(
							rd_notion_api_key
							, rd_notion_page_id
							, TOKEN
							, CONTEXT.m_is_naked
							)))
				goto LBL_NONGOOD;

			TOKEN = strtok(ae2f_NIL, "\n");
		}
	}

	gitnote_set_snap(rd_notion_api_key, rd_notion_page_id, BREAKPOINT);
	gitnote_tdpool_join();

LBL_NONGOOD:
	if(CONTEXT.m_is_alloc_branch)	free(BRANCH.m_buf);
	if(CONTEXT.m_is_alloc_lsfiles)	free(BUF_LSFILES);
	return	STATE_ON_ERR;
}
