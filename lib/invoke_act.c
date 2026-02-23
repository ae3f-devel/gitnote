#include <gitnote.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "util/tdpool.h"
#include "util/assert_unless.h"

#include <ae2f/Sys/Thrd.h>


GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_invoke_act(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		char* ae2f_restrict const rdwr_tok,
		const int c_is_naked
		) 
{
	char* ae2f_restrict	TOKEN;

	assert_unless(rdwr_tok)	return GITNOTE_NULL_ARG;

	if(c_is_naked) {
		return gitnote_act_creat(rd_notion_api_key, rd_notion_page_id, rdwr_tok);
	}

	switch(*rdwr_tok) {
		enum GITNOTE_ ERR;
		case 'R': 
		{
			char* TOKEN2;
			ae2f_expected_but_else(TOKEN = strstr(rdwr_tok, "\t"))
				return GITNOTE_STRSTR_FAILED;

			ae2f_expected_but_else(TOKEN2 = strstr(TOKEN + 1, "\t"))
				return GITNOTE_STRSTR_FAILED;

			*TOKEN = 0;
			*TOKEN2 = 0;

			ae2f_unexpected_but_if(
					ERR = gitnote_tdpool_push(gitnote_act_rm
						, rd_notion_api_key
						, rd_notion_page_id
						, TOKEN + 1))
				return ERR;


			return 	gitnote_tdpool_push(
					gitnote_act_creat
					, rd_notion_api_key
					, rd_notion_page_id
					, TOKEN2 + 1);
		}

		case 'D':
		{
			ae2f_expected_but_else(TOKEN = strstr(rdwr_tok, "\t"))
				return GITNOTE_STRSTR_FAILED;

			return 	gitnote_tdpool_push(
					gitnote_act_rm
					, rd_notion_api_key
					, rd_notion_page_id
					, TOKEN + 1);
		}
		case 'A':
		{
			ae2f_expected_but_else(TOKEN = strstr(rdwr_tok, "\t"))
				return GITNOTE_STRSTR_FAILED;

			return 	gitnote_tdpool_push(
					gitnote_act_creat
					, rd_notion_api_key
					, rd_notion_page_id
					, TOKEN + 1);

		}
		case 'M':
		{
			ae2f_expected_but_else(TOKEN = strstr(rdwr_tok, "\t"))
				return GITNOTE_STRTOK_FAILED;

			return 	gitnote_tdpool_push(
					gitnote_act_mod
					, rd_notion_api_key
					, rd_notion_page_id
					, TOKEN + 1);
		}
		default:
		printf("[gitnote]: Met unexpected: %c[%d]\n", *rdwr_tok, (int)*rdwr_tok);
		puts("=============");
		puts("");
		puts(rdwr_tok);
		puts("");
		puts("=============");
		return GITNOTE_MET_UNEXPECTED;
	}

	return GITNOTE_SUCCESS;
}
