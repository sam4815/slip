typedef char*(*pfunc)(mpc_parser_t*, lenv*, char*);
typedef void(*dfunc)(mpc_parser_t*, lenv*);

struct slip;
typedef struct slip slip;

struct slip {
	lenv* environment;
  mpc_parser_t* parser;
  pfunc evaluate_string;
  dfunc destroy;
};

slip* initialize_slip(void);
char* slip_stringify(lval* result);
