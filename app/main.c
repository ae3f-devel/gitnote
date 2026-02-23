#include <gitnote.h>
#include <stdio.h>
#include <stdlib.h>

enum ARG_ {
	ARG_PROGRAMME,
	ARG_NOTION_KEY,
	ARG_NOTION_ROOT_ID,

	/*** optional */

	ARG_BRANCH,
	ARG_VENV,
	ARG_BEGIN_COMMIT,
	ARG_BREAKPOINT_COMMIT,
	ARG_TDCOUNT
};

enum ARGC_ {
	ARGC_ZERO,
	ARGC_PROGRAMME,
	ARGC_NOTION_KEY,
	ARGC_NOTION_ROOT_ID,

	/*** optional **/

	ARGC_BRANCH,
	ARGC_VENV,
	ARGC_BEGIN_COMMIT,
	ARGC_BREAKPOINT_COMMIT,
	ARGC_TDCOUNT
};

int main(int argc, const char** argv);

int main(int argc, const char** argv) {
	typedef const char* const_str_t;
	enum GITNOTE_	STAT;

	int		THREAD_COUNT = 10;

	const_str_t 
		BRANCH = ""
		, BEGIN_COMMIT = ""
		, BREAKPOINT_COMMIT = ""
		, NOTION_TOKEN = ""
		, NOTION_ROOT_PAGE_ID = ""
		, VENV = "./.venv";

	B_gitnote_option_t	GITNOTE_OPT = 0;

	switch((enum ARGC_)argc) {
		char CH;
		case ARGC_ZERO:
		return -1;

		default:
		case ARGC_NOTION_KEY:
		case ARGC_PROGRAMME:
LBL_DEFAULT:
		puts("./gitnote <notion-api-key> <notion-page-id-root> "
				"<--help|--init|{branch}|{null}> "
				"<{venv}|{null}> "
				"<{begin_commit}|{null}> "
				"<{breakpoint}|{null}> "
				"<{thread_count}|{null}>"
		    );
		return 0;

		case ARGC_TDCOUNT:
		THREAD_COUNT = atoi(argv[ARG_TDCOUNT]);
		if(THREAD_COUNT < 0) THREAD_COUNT = 1;
		ae2f_fallthrough;

		case ARGC_BREAKPOINT_COMMIT:
		BREAKPOINT_COMMIT = argv[ARG_BREAKPOINT_COMMIT];
		ae2f_fallthrough;

		case ARGC_BEGIN_COMMIT:
		BEGIN_COMMIT = argv[ARG_BEGIN_COMMIT];
		ae2f_fallthrough;

		case ARGC_VENV:
		VENV = argv[ARG_VENV];
		ae2f_fallthrough;

		case ARGC_BRANCH:
		if(argv[ARG_BRANCH][0] == '-' && argv[ARG_BRANCH][1] == '-') {
			if(argv[ARG_BRANCH][2] == 'h') {
				/** heuristic --help */
				goto LBL_DEFAULT;
			} else if(argv[ARG_BRANCH][2] == 'i') {
				/** heuristic --init */
				GITNOTE_OPT |= GITNOTE_OPT_INIT;
			}
		} else BRANCH = argv[ARG_BRANCH];
		break;

		case ARGC_NOTION_ROOT_ID:

		puts("Want to default as your current branch?");
		scanf("%c", &CH);
		fgetc(stdin);

		/** query for something */
		if(CH == 'y' || CH == 'Y') {
		} else { goto LBL_DEFAULT; }
	}

	NOTION_TOKEN = argv[ARG_NOTION_KEY];
	NOTION_ROOT_PAGE_ID = argv[ARG_NOTION_ROOT_ID];

	STAT = gitnote_entry(
			BRANCH
			, BEGIN_COMMIT
			, BREAKPOINT_COMMIT
			, NOTION_TOKEN
			, NOTION_ROOT_PAGE_ID
			, GITNOTE_OPT
			, VENV
			, (unsigned)THREAD_COUNT
			);

	printf("STAT: %d\n", (int)STAT);
	return (int)STAT;
}
