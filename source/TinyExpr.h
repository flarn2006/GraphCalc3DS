// For implementation, see TinyExpr_wrapper.cpp

extern "C" {
	#include "tinyexpr/tinyexpr.h"
}

#include <memory>

class TinyExpr
{
	std::shared_ptr<te_expr> expr;
	int error = -1;
	double x;

public:
	TinyExpr();
	TinyExpr(const char *expression);
	double Evaluate(double x);
	int GetError() const;
	static int TestExpr(const char *expression);
};
