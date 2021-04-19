#include "../include/MathModule.h"
#include "../src/VectorImpl.cpp"
#include "../include/ValidChecker.h"
#include "../include/IVector.h"


#include <new>

RC IVector::setLogger(ILogger *const logger)
{
    return VectorImpl::setLogger(logger);
}


IVector *IVector::add(IVector const* const& op1, IVector const* const& op2)
{
    return AddAndSub(op1, op2, [] (double a, double b) { return a + b;});
}

IVector *IVector::sub(IVector const* const& op1, IVector const* const& op2)
{
    return AddAndSub(op1, op2, [] (double a, double b) { return a - b;});
}

IVector* IVector::createVector(size_t dim, const double* const& ptr_data)
{
    if ((int)dim <= 0)
    {
        VectorImpl::log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }


    if (!ptr_data)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }


    for (size_t i = 0; i < dim; i++)
    {
        if (!ValidChecker::isValidNumber(ptr_data[i]))
        {
            VectorImpl::log(RC::NOT_NUMBER, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return nullptr;
        }
    }

    const size_t _size = sizeof(VectorImpl) + dim * sizeof(double);
    uint8_t* pInstance = new (std::nothrow)uint8_t[_size];

    if (!pInstance)
    {
        VectorImpl::log(RC::ALLOCATION_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return nullptr;
    }

    uint8_t* pData = pInstance + sizeof(VectorImpl);


    memcpy(pData, (uint8_t*)ptr_data, dim * sizeof(double));

    return new(pInstance)VectorImpl(dim);
}

double IVector::dot(IVector const* const& op1, IVector const* const& op2) {

    if (!op1 || !op2 || op1->getDim() != op2->getDim())
        return std::numeric_limits<double>::quiet_NaN();


    double result = 0, left, right, result_tmp;
    const double dim = op1->getDim();

    double const * data1 = op1->getData();
    double const * data2 = op2->getData();

    for (size_t i = 0; i < dim; i++)
    {
        result += data1[i] * data2[i];

        if (!ValidChecker::isValidNumber(result))
        {
            VectorImpl::log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    return result;
}

bool IVector::equals(IVector const* const& op1, IVector const* const& op2, NORM n, double tol)
{
    if (!op1 || !op2 || tol < 0 || !ValidChecker::isValidNumber(tol))
    {
        VectorImpl::log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    if (op1->getDim() != op2->getDim())
    {
        VectorImpl::log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    IVector* deltaVector = IVector::sub(op1, op2);

    if (!deltaVector)
    {
        VectorImpl::log(RC::ALLOCATION_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    const double deltaNorm = fabs(deltaVector->norm(n));

    if (std::isnan(deltaNorm))
    {
        delete deltaVector;
        VectorImpl::log(RC::NOT_NUMBER, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return false;
    }

    delete deltaVector;

    return deltaNorm < tol;
}

RC IVector::copyInstance(IVector* const dest, IVector const* const& src)
{
    if (src == nullptr || dest == nullptr)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    if (abs(reinterpret_cast<uint8_t*>(dest) - reinterpret_cast<uint8_t const*>(src)) < src->sizeAllocated())
    {
        VectorImpl::log(RC::UNKNOWN, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::UNKNOWN;
    }

    memcpy((void*)dest, (void*)src, src->sizeAllocated());

    return RC::SUCCESS;
}

RC IVector::moveInstance(IVector* const dest, IVector*& src)
{
    if (src == nullptr || dest == nullptr)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    memmove((void*)dest, (void*)src, src->sizeAllocated());

    delete src;
    src = nullptr;
    return RC::SUCCESS;
}

RC IVector::moveInstance(IVector *const dest, const IVector *&src) {
    if (src == nullptr || dest == nullptr)
    {
        VectorImpl::log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    memmove((void*)dest, (void*)src, src->sizeAllocated());

    delete src;
    src = nullptr;
    return RC::SUCCESS;
}


IVector::~IVector()= default;

