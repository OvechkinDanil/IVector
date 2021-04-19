#ifndef IVECTOR_VECTORIML_H
#define IVECTOR_VECTORIML_H

#include "../include/IVector.h"
#include "../include/MathModule.h"
#include "../include/ValidChecker.h"
#include <limits>
#include <cmath>
#include <cstring>
#include <cstdint>

namespace {
    class VectorImpl : public IVector{
    private:
        size_t _dim = 0;
        static ILogger* pLogger;

    public:

        static void log(RC code, ILogger::Level level, const char* const& srcfile, const char* const& function, int line);

        VectorImpl(size_t dim):_dim(dim){}

        static RC setLogger(ILogger *const logger);

        RC getCord(size_t index, double& val) const override;

        RC setCord(size_t index, double val) override;

        RC scale(double multiplier) override;

        RC inc(IVector const* const& op) override;

        RC dec(IVector const* const& op) override;

        size_t getDim() const override;

        double norm(NORM n) const override;

        IVector* clone() const override;

        RC applyFunction(const std::function<double(double)>& fun) override;

        RC foreach(const std::function<void(double)>& fun) const override;

        double const* getData() const override;

        size_t sizeAllocated() const override;

        RC setData(size_t dim, double const* const& ptr_data) override;

        ~VectorImpl() override;
    };
    ILogger* VectorImpl::pLogger = nullptr;

}; //end name   space anonymous

VectorImpl::~VectorImpl() = default;

RC VectorImpl::setLogger(ILogger *const logger) {
    if (!logger)
    {
        log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    pLogger = logger;
    return RC::SUCCESS;
}

RC VectorImpl::getCord(size_t index, double &val) const
{
    if (index > this->getDim() - 1)
    {
        log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    if (!ValidChecker::isValidNumber(val))
    {
        log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    auto* ptr = (double*)((uint8_t*)this + sizeof(VectorImpl) + index * sizeof(double));
    val = *ptr;
    return RC::SUCCESS;
}

RC VectorImpl::setCord(size_t index, double val)
{
    if (index > this->getDim() - 1)
    {
        log(RC::INDEX_OUT_OF_BOUND, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    if (!ValidChecker::isValidNumber(val))
    {
        log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    auto* ptr = (double*)((uint8_t*)this + sizeof(VectorImpl) + index * sizeof(double));
    *ptr = val;
    return RC::SUCCESS;
}

RC VectorImpl::scale(double multiplier)
{
    if (multiplier == 1)
        return RC::SUCCESS;

    if (!ValidChecker::isValidNumber(multiplier))
    {
        log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    double* ptr = (double*)((uint8_t*)this + sizeof(VectorImpl));
    for (size_t i = 0; i < this->getDim(); i++)
    {
        ptr[i] *= multiplier;
    }
    return RC::SUCCESS;
}

size_t VectorImpl::getDim() const
{
    return _dim;
}

double VectorImpl::norm(IVector::NORM n) const
{
    double result = 0;
    const double* data = this->getData();

    switch (n)
    {
        case NORM::FIRST:
            for (size_t i = 0; i < this->getDim(); i++)
            {
                result += fabs(data[i]);

                if (!ValidChecker::isValidNumber(result))
                {
                    log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                    return std::numeric_limits<double>::quiet_NaN();;
                }
            }

            break;

        case NORM::SECOND:
            for (size_t i = 0; i < this->getDim(); i++)
            {
                result += data[i] * data[i];

                if (!ValidChecker::isValidNumber(result))
                {
                    log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                    return std::numeric_limits<double>::quiet_NaN();;
                }
            }
            result = sqrt(result);
            break;

        case NORM::CHEBYSHEV:
            result = 0;
            for (size_t i = 0; i < this->getDim(); i++)
            {
                if (fabs(data[i]) > result)
                    result = fabs(data[i]);

                if (!ValidChecker::isValidNumber(result))
                {
                    log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
                    return std::numeric_limits<double>::quiet_NaN();
                }
            }
            break;
        default:
        {
            log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    return result;
}

IVector *VectorImpl::clone() const
{
    int size = sizeof(VectorImpl) + this->getDim() * sizeof(double);

    auto* ptr_block = new uint8_t[size];
    auto* currentPtr = (uint8_t*)this;

    memcpy(ptr_block, currentPtr, size);

    return (IVector*) ptr_block;
}

RC VectorImpl::applyFunction(const std::function<double(double)> &fun) {
    double* data = (double*)((uint8_t*)this + sizeof(VectorImpl));

    for (size_t i = 0; i < _dim; i++)
    {
        if (!ValidChecker::isValidNumber(fun(data[i])))
        {
            log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::INFINITY_OVERFLOW;
        }
    }

    for (size_t i = 0; i < _dim; i++)
    {
        data[i] = fun(data[i]);
    }

    return RC::SUCCESS;
}

RC VectorImpl::foreach(const std::function<void(double)> &fun) const
{
    double* data = (double*)((uint8_t*)this + sizeof(VectorImpl));

    for (size_t i = 0; i < _dim; i++)
    {
        fun(data[i]);
    }

    return RC::SUCCESS;
}

double const *VectorImpl::getData() const
{
    return (double*)((uint8_t*)this + sizeof(VectorImpl));
}

size_t VectorImpl::sizeAllocated() const
{
    return sizeof(*this) + _dim * sizeof(double);
}

RC VectorImpl::inc(const IVector *const &op)
{
    if (!op)
        return RC::INVALID_ARGUMENT;

    if (_dim != op->getDim())
    {
        log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    const double* ptr2 = op->getData();
    double* ptr1 = (double*)((uint8_t*)this + sizeof(VectorImpl));

    for (size_t i = 0; i < _dim; i++)
    {
        if (!ValidChecker::isValidNumber(ptr1[i] + ptr2[i]))
        {
            log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::INFINITY_OVERFLOW;
        }
    }

    for (size_t i = 0; i < _dim; i++)
    {
        ptr1[i] += ptr2[i];
    }

    return RC::SUCCESS;
}


RC VectorImpl::dec(const IVector *const &op)
{
    double result;

    if (!op)
        return RC::INVALID_ARGUMENT;

    if (_dim != op->getDim())
    {
        log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    const double* ptr2 = op->getData();
    double* ptr1 = (double*)((uint8_t*)this + sizeof(VectorImpl));

    for (size_t i = 0; i < _dim; i++)
    {
        result = ptr1[i] - ptr2[i];
        if (!ValidChecker::isValidNumber(result))
        {
            log(RC::INFINITY_OVERFLOW, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::INFINITY_OVERFLOW;
        }
    }

    for (size_t i = 0; i < _dim; i++)
    {
        ptr1[i] -= ptr2[i];
    }

    return RC::SUCCESS;
}

void VectorImpl::log(RC code, ILogger::Level level, const char* const& srcfile, const char* const& function, int line)
{
    if (pLogger != nullptr)
        pLogger->log(code, level, srcfile, function, line);
}

RC VectorImpl::setData(size_t dim, const double *const &ptr_data) {
    if (!ptr_data || dim != this->_dim)
    {
        pLogger->log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    for (size_t i = 0; i < dim; i++)
    {
        if (!ValidChecker::isValidNumber(ptr_data[i]))
        {
            VectorImpl::log(RC::NOT_NUMBER, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::NOT_NUMBER;
        }
    }
    auto* curPtrData = (double*)((uint8_t*)this + sizeof(VectorImpl));
    memcpy(curPtrData, ptr_data, _dim * sizeof(double));

    return RC::SUCCESS;
}


#endif // IVECTOR_VECTORIML_H