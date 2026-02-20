#include <gitnote.h>
#include <stdio.h>
#include <easyredir.h>
#include <dist/utils.c>

ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_act_creat(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const char* const rd_path
		) {
	const char* ARGV[3];

	ARGV[0] = rd_notion_api_key;
	ARGV[1] = rd_notion_page_id;
	ARGV[2] = rd_path;

	fputs("A	", stdout);
	puts(rd_path);

	easyredir_entry(
			"", "", ""
			, "./dist/touch", 0
			, 3, ARGV
			);

	easyredir_entry(
			"", "", ""
			, "./dist/modify_file", 0
			, 3, ARGV
			);

	return GITNOTE_SUCCESS;
}
GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_rm(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const char* const rd_path
		) {
	const char* ARGV[3];

	ARGV[0] = rd_notion_api_key;
	ARGV[1] = rd_notion_page_id;
	ARGV[2] = rd_path;

	fputs("D	", stdout);
	puts(rd_path);

	easyredir_entry(
			"", "", ""
			, "./dist/rm", 0
			, 3, ARGV
			);

	return GITNOTE_SUCCESS;
}
ae2f_extern GITNOTE_ABI_IMPL enum GITNOTE_ gitnote_act_mod(
		const char* ae2f_restrict const rd_notion_api_key,
		const char* ae2f_restrict const rd_notion_page_id,
		const char* const rd_path
		) {
	const char* ARGV[3];

	ARGV[0] = rd_notion_api_key;
	ARGV[1] = rd_notion_page_id;
	ARGV[2] = rd_path;

	fputs("M	", stdout);
	puts(rd_path);

	easyredir_entry(
			"", "", ""
			, "./dist/modify_file", 0
			, 3, ARGV
			);
	return GITNOTE_SUCCESS;
}
