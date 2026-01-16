#include <gitnote.h>
#include <stdio.h>

GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_creat(const char* const rd_path) {
	printf("GITNOTE-A\t%s\n", rd_path);
	return GITNOTE_SUCCESS;
}
GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_rm(const char* const rd_path) {
	printf("GITNOTE-D\t%s\n", rd_path);
	return GITNOTE_SUCCESS;
}
GITNOTE_ABI_DECL enum GITNOTE_ gitnote_act_mod(const char* const rd_path) {
	printf("GITNOETE-M\t%s\n", rd_path);
	return GITNOTE_SUCCESS;
}
