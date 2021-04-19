#ifndef IVECTOR_MATHMODULE_H
#define IVECTOR_MATHMODULE_H

#include "../include/IVector.h"
#include "../include/ISet.h"
#include "../src/VectorImpl.cpp"
#include <functional>
#include <limits>
#include <math.h>


IVector* AddAndSub(const IVector *op1, const IVector *op2, const std::function<double(double, double)>& func);

RC forEachInSet(const ISet* const& set, std::function<RC(IVector*)>& func);

#endif //IVECTOR_MATHMODULE_H
