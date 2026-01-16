
#include <gitnote.h>
#include <assert.h>
#include <string.h>

#include "util/assert_unless.h"

GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_invoke_act(
		char* ae2f_restrict const rd_tok, 
		const int c_is_naked
		) 
{
	char* ae2f_restrict	TOKEN;

	assert_unless(rd_tok)	return GITNOTE_NULL_ARG;

	if(c_is_naked)
		return gitnote_act_creat(rd_tok);

	switch(*rd_tok) {
		enum GITNOTE_ ERR;
		case 'R': 
		{
			char* TOKEN2;
			ae2f_expected_but_else(TOKEN = strstr(rd_tok, "\t"))
				return GITNOTE_STRSTR_FAILED;

			ae2f_expected_but_else(TOKEN2 = strstr(TOKEN + 1, "\t"))
				return GITNOTE_STRSTR_FAILED;

			*TOKEN = 0;
			*TOKEN2 = 0;
			ae2f_unexpected_but_if(ERR = gitnote_act_rm(TOKEN + 1))
				return ERR;


			return gitnote_act_creat(TOKEN2 + 1);
		}

		case 'D':
		{
			ae2f_expected_but_else(TOKEN = strstr(rd_tok, "\t"))
				return GITNOTE_STRSTR_FAILED;

			return gitnote_act_rm(TOKEN + 1);
		}
		case 'A':
		{
			ae2f_expected_but_else(TOKEN = strstr(rd_tok, "\t"))
				return GITNOTE_STRSTR_FAILED;

			return gitnote_act_creat(TOKEN + 1);
		}
		case 'M':
		{
			ae2f_expected_but_else(TOKEN = strstr(rd_tok, "\t"))
				return GITNOTE_STRTOK_FAILED;

			return gitnote_act_mod(TOKEN + 1);
		}
		default:
		assert(0);
	}

	return GITNOTE_SUCCESS;
}
