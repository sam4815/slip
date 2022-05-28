typedef struct {
	int type;
	long num;
	int err;
} lval;

enum { LVAL_NUM, LVAL_ERR };

enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval lval_num(long x);
lval lval_err(int x);
void print_lval(lval v);

