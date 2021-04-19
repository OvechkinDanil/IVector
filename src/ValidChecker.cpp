#include "../include/ValidChecker.h"

bool ValidChecker::isValidNumber(double val)
{
    return std::isfinite(fabs(val));
}
