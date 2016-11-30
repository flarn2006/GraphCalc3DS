#include "TinyExpr.h"

TinyExpr::TinyExpr(const char *expression)
{
	te_variable vars[] = {{"x", &x}};
	te_expr *ptr = te_compile(expression, vars, 1, &error);
	if (!error) {
		expr = std::shared_ptr<te_expr>(ptr, te_free);
	} else {
		expr.reset();
	}
}

double TinyExpr::Evaluate(double x)
{
	this->x = x;
	return te_eval(expr.get());
}

int TinyExpr::GetError() const
{
	return error;
}

int TinyExpr::TestExpr(const char *expression)
{
	int error;
	te_variable vars[] = {{"x", NULL}};
	te_expr *expr = te_compile(expression, vars, 1, &error);
	if (!error)
		te_free(expr);
	return error;
}
