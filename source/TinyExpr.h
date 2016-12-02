// For implementation, see TinyExpr_wrapper.cpp

extern "C" {
	#include "tinyexpr/tinyexpr.h"
}

#include <memory>

class TinyExpr
{
	std::shared_ptr<te_expr> expr;
	int error = -1;
	static double x;
	static const double *abcd[4];

public:
	TinyExpr();
	TinyExpr(const char *expression);
	double Evaluate(double x) const;
	int GetError() const;
	static void SetABCDVars(const double *a, const double *b, const double *c, const double *d);
	static int TestExpr(const char *expression);
};
