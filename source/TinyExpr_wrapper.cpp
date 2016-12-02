#include "TinyExpr.h"
#include <limits>

double TinyExpr::x = 1.23456;
const double *TinyExpr::abcd[4];

TinyExpr::TinyExpr()
{
}

TinyExpr::TinyExpr(const char *expression)
{
	te_variable vars[] = {{"x", &x}, {"a", abcd[0]}, {"b", abcd[1]}, {"c", abcd[2]}, {"d", abcd[3]}};
	te_expr *ptr = te_compile(expression, vars, 5, &error);
	if (!error) {
		expr = std::shared_ptr<te_expr>(ptr, te_free);
	} else {
		expr.reset();
	}
}

double TinyExpr::Evaluate(double x) const
{
	if (!error) {
		this->x = x;
		return te_eval(expr.get());
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

int TinyExpr::GetError() const
{
	return error;
}

void TinyExpr::SetABCDVars(const double *a, const double *b, const double *c, const double *d)
{
	abcd[0] = a;
	abcd[1] = b;
	abcd[2] = c;
	abcd[3] = d;
}

int TinyExpr::TestExpr(const char *expression)
{
	int error;
	te_variable vars[] = {{"x", NULL}, {"a", NULL}, {"b", NULL}, {"c", NULL}, {"d", NULL}};
	te_expr *expr = te_compile(expression, vars, 5, &error);
	if (!error)
		te_free(expr);
	return error;
}
