#include <gitnote.h>
#include <stdio.h>

enum ARG_ {
	ARG_PROGRAMME,
	ARG_BRANCH,
	ARG_BEGIN_COMMIT,
	ARG_BREAKPOINT_COMMIT
};

enum ARGC_ {
	ARGC_ZERO,
	ARGC_PROGRAMME,
	ARGC_BRANCH,
	ARGC_BEGIN_COMMIT,
	ARGC_BREAKPOINT_COMMIT
};

int main(int argc, const char** argv);

int main(int argc, const char** argv) {
	typedef const char* const_str_t;

	const_str_t BRANCH = "", BEGIN_COMMIT = "", BREAKPOINT_COMMIT = "";

	switch((enum ARGC_)argc) {
		char CH;
		case ARGC_ZERO:
		return -1;

		case ARGC_BREAKPOINT_COMMIT:
		BREAKPOINT_COMMIT = argv[ARG_BREAKPOINT_COMMIT];
		ae2f_fallthrough;

		case ARGC_BEGIN_COMMIT:
		BEGIN_COMMIT = argv[ARG_BEGIN_COMMIT];
		ae2f_fallthrough;

		case ARGC_BRANCH:
		if(argv[ARG_BRANCH][0] == '-') {
			/** heuristic --help */
			goto LBL_DEFAULT;
		} else BRANCH = argv[ARG_BRANCH];
		break;

		case ARGC_PROGRAMME:
		puts("Want to default as your current branch?");
		scanf("%c", &CH);
		fgetc(stdin);

		/** query for something */
		if(CH == 'y' || CH == 'Y') {
		} else {
			ae2f_fallthrough;
			default:
LBL_DEFAULT:
			puts("./gitnote "
					"<--help|{branch}|{null}> "
					"<{begin_commit}|{null}> "
					"<{breakpoint}|{null}>"
					);
			return 0;
		}
	}


	printf("%s %s %s\n", BRANCH, BEGIN_COMMIT, BREAKPOINT_COMMIT);
	gitnote_entry(
			BRANCH
			, BEGIN_COMMIT
			, BREAKPOINT_COMMIT
			, GITNOTE_OPT_INIT & 0
			);

	return 0;
}
