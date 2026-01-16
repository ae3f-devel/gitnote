#ifndef gitnote_h
#define gitnote_h

#include <ae2f/Keys.h>
#include <ae2f/Guide.h>

#include "./gitnote/.link.h"
#include "./gitnote/enum.h"

#if GITNOTE_SHARED
#define GITNOTE_ABI_DECL	ae2f_decl
#define GITNOTE_ABI_IMPL	ae2f_impl
#else
#define GITNOTE_ABI_DECL
#define GITNOTE_ABI_IMPL
#endif

ae2f_extern GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_creat(const char* const rd_path);
ae2f_extern GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_rm(const char* const rd_path);
ae2f_extern GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_mod(const char* const rd_path);

ae2f_extern GITNOTE_ABI_DECL enum GITNOTE_ gitnote_invoke_act(
		char* ae2f_restrict const rdwr_tok, 
		const int c_is_naked
		);

/** 
 * @brief commit hash string
 * */
typedef const char* ae2f_restrict gitnote_commit_hash_str_t;

typedef ae2f_TYPEDEF(enum GITNOTE_OPT_) unsigned B_gitnote_option_t;

ae2f_extern GITNOTE_ABI_DECL enum GITNOTE_ gitnote_entry(
		const char* const rd_branch_name,
		const gitnote_commit_hash_str_t rd_begin,
		const gitnote_commit_hash_str_t rd_breakpoint,
		const B_gitnote_option_t	c_flags
		);

/** get default branch */
ae2f_extern ae2f_FREE(free) ae2f_noexcept  GITNOTE_ABI_DECL char* ae2f_retnew gitnote_get_default(
		const char* const ae2f_restrict rd_filename, 
		const char* const ae2f_restrict rd_process,
		const int c_argc,
		const char** const ae2f_restrict rd_argv
		);

#endif
