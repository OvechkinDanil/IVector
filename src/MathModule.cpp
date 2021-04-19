#include "../include/MathModule.h"


IVector* AddAndSub(const IVector *op1, const IVector *op2, const std::function<double(double, double)>& func)
{
    if (!op1 || !op2 || op1->getDim() != op2->getDim())
        return nullptr;

    const double dim = op1->getDim();
    double result;
    IVector* resultInstance = op1->clone();

    if (!resultInstance)
        return nullptr;

    const double* data1 = op1->getData();
    const double* data2 = op2->getData();
    auto* ptr = (double*)((uint8_t*)resultInstance + sizeof(VectorImpl));

    for (size_t i = 0; i < dim; i++)
    {
        result = func(data1[i], data2[i]);

        if (fabs(result) == std::numeric_limits<double>::infinity())
        {
            VectorImpl::log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            delete resultInstance;
            resultInstance = nullptr;
            return nullptr;
        }

        ptr[i] = result;
    }

    return resultInstance;
}

//RC forEachInSet(const ISet* const& set, std::function<RC(IVector*)>& func)
//{
//    IVector* vectorForInsert = IVector::createVector(set->getDim(), new double[set->getDim()]);
//    ISet::IIterator* it = set->getBegin();
//    if (it == nullptr)
//        return RC::NULLPTR_ERROR;
//
//    bool isAtEnd = false;
//
//    while (!it->isEnd())
//    {
//
//    }
//
//
//}






