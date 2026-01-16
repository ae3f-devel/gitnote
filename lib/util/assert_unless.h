/** NOTE: `a` will be duplicately in `assert` and `if` */
#define assert_unless(a)	assert(a); ae2f_expected_but_else(a)
