#include <gitnote.h>

#include "./util/rdallfp.h"

char* ae2f_retnew GITNOTE_ABI_IMPL ae2f_noexcept gitnote_get_default(
		const char* const ae2f_restrict rd_filename,
		const char* const ae2f_restrict rd_process,
		const int c_argc, 
		const char** const ae2f_restrict rd_argv) {
	assert_unless(rd_filename) return 0;

	remove(rd_filename);

	ae2f_unexpected_but_if(easyredir_entry(
				""
				, rd_filename
				, ""
				, rd_process
				, 1, c_argc, rd_argv)) {
		assert(0 && "easyredir");
		return 0;
	}


	return util_rdallfp(rd_filename);
}
