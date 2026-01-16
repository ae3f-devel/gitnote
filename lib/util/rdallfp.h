#ifndef util_rdallfp_h
#define util_rdallfp_h

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <easyredir.h>

#include <ae2f/c90/Limits.h>

#include "./assert_unless.h"

ae2f_inline static char* ae2f_retnew util_rdallfp(const char* rd_path) {
	char* RET = 0;
	FILE* F = 0;
	union { long m_ftell; size_t m_read; } LEN;

	assert_unless(rd_path) return 0;

	ae2f_expected_but_else(F = fopen(rd_path, "r"))
		return 0;

	fseek(F, 0, SEEK_END);
	ae2f_expected_but_else(RET = (LEN.m_ftell = ftell(F)) < 0 ? 
			ae2f_NIL : malloc((size_t)LEN.m_ftell + 3))
		goto LBL_DOOMALL;

	fseek(F, 0, SEEK_SET);
	LEN.m_read = fread(RET, 1, (size_t)LEN.m_ftell + 3, F);

	fclose(F);
	return	RET;

LBL_DOOMALL:
	if(F)	fclose(F);
	if(RET)	free(RET);
	return 0;
}

#endif
