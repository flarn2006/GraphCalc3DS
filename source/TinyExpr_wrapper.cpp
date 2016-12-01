#include "TinyExpr.h"
#include <limits>

TinyExpr::TinyExpr()
{
}

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
	if (!error) {
		this->x = x;
		return te_eval(expr.get());
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
}

double TinyExpr::Evaluate(double x) const
{
	if (!error) {
		TinyExpr expr2 = *this;
		return expr2.Evaluate(x);
	} else {
		return std::numeric_limits<double>::quiet_NaN();
	}
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
