//
// Created by Danil on 26.03.2021.
//
#include "../include/ISet.h"
#include "../include/ValidChecker.h"
#include "../include/ILogger.h"
#include "../include/IControlBlock.h"
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace {
    class ISetImpl : public ISet {
    public:
        static void log(RC code, ILogger::Level level, const char* const& srcfile, const char* const& function, int line);

        ISetImpl();

        static RC setLogger(ILogger *const logger);

        size_t getDim() const override;
        size_t getSize() const override;

        ISet* clone() const override;

        RC getCopy(size_t index, IVector*& val) const override;
        RC findFirstAndCopy(IVector const * const& pat, IVector::NORM n, double tol, IVector *& val) const override;

        RC getCoords(size_t index, IVector * const& val) const override;
        RC findFirstAndCopyCoords(IVector const * const& pat, IVector::NORM n, double tol, IVector * const& val) const override;

        RC insert(IVector const * const& val, IVector::NORM n, double tol) override;

        RC remove(size_t index) override;
        RC remove(IVector const * const& pat, IVector::NORM n, double tol) override;

        IIterator *getIterator(size_t index) const override;
        IIterator *getBegin() const override;
        IIterator *getEnd() const override;

        ~ISetImpl() override;

        class IControlBlockImpl : public ISetControlBlock
        {
        private:
            ISetImpl* _set;
        public:
            IControlBlockImpl(ISetImpl* set) : _set(set) {};
            RC getNext(IVector *const &vec, size_t &index, size_t indexInc = 1) const override;
            RC getPrevious(IVector *const &vec, size_t &index, size_t indexInc = 1) const override;

            bool isBegin(size_t index) override;
            bool isEnd(size_t index) override;

            ~IControlBlockImpl() override = default ;
        };

        class IIteratorImpl : public IIterator
        {
        private:
            size_t _index;
            IVector* _vec;
            std::shared_ptr<ISetControlBlock> _cntrl_block;
        public:
            IIteratorImpl(std::shared_ptr<ISetControlBlock>& cntrl_block, IVector* vec, size_t index);
            IIterator * getNext(size_t indexInc = 1) const override;
            IIterator * getPrevious(size_t indexInc = 1) const override;
            IIterator * clone() const override;

            static RC setLogger(ILogger * const pLogger);

            RC next(size_t indexInc = 1) override;
            RC previous(size_t indexInc = 1) override;

            static bool equal(const IIterator *op1, const IIterator *op2);

            RC getVectorCopy(IVector *& val) const override;

            RC getVectorCoords(IVector * const& val) const override;

            bool isBegin() const override;
            bool isEnd() const override;

            ~IIteratorImpl() override;

        protected:
            virtual size_t getIndex() const override;
        };

    private:
        size_t _dim = 0;
        size_t _size = 0;
        size_t _capacity = 100;
        static ILogger *pLogger;
        double* _data;
        std::vector<size_t> availableIndexes;
        size_t maxHash = 0;
        std::shared_ptr<IControlBlockImpl> controlBlock = std::make_shared<IControlBlockImpl>(this);

        RC FindEqualData(const IVector *const &pat, IVector::NORM n, double tol, int& index, int startIndex = 0) const;
    };


    ILogger* ISetImpl::pLogger = nullptr;
}

    ISetImpl::ISetImpl() {
        _data = new double[_capacity];
    }

size_t ISetImpl::getDim() const {
    return _dim;
}

size_t ISetImpl::getSize() const {
    return _size;
}

RC ISetImpl::getCoords(size_t index, IVector * const& val) const {
    if (index < 0 || index > _size - 1)
    {
        pLogger->log(RC::INDEX_OUT_OF_BOUND, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    auto *dataForOutput = new double[_dim];
    memcpy((void *) dataForOutput, _data + index * _dim, _dim * sizeof(double));

    val->setData(_dim, dataForOutput);

    return RC::SUCCESS;
}

RC ISetImpl::findFirstAndCopyCoords(IVector const * const& pat, IVector::NORM n, double tol, IVector * const& val) const {
    if (_size == 0) {
        pLogger->log(RC::INDEX_OUT_OF_BOUND, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    if (pat == nullptr || n == IVector::NORM::AMOUNT) {
        pLogger->log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    int indexOfEqualData;
    RC code;
    if ((code = FindEqualData(pat, n, tol, indexOfEqualData)) != RC::SUCCESS)
        return code;

    if (indexOfEqualData != -1)
    {
        auto* dataForOutput = new double[_dim];
        memcpy(dataForOutput, _data + indexOfEqualData * _dim, _dim * sizeof(double));
        val->setData(_dim, dataForOutput);
        return RC::SUCCESS;
    }

    return RC::VECTOR_NOT_FOUND;
}

RC ISetImpl::findFirstAndCopy(const IVector *const &pat, IVector::NORM n, double tol, IVector *&val) const {
    RC rc;
    IVector* newVector = IVector::createVector(_dim, new double[_dim]);
    if ((rc = findFirstAndCopyCoords(pat, n, tol, newVector)) != RC::SUCCESS)
        return rc;

    val = newVector;

    return RC::SUCCESS;
}

RC ISetImpl::insert(IVector const * const& val, IVector::NORM n, double tol) {
    if (val == nullptr || n == IVector::NORM::AMOUNT || !ValidChecker::isValidNumber(tol) || tol < 0) {
        pLogger->log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    if (_dim != 0 && val->getDim() != _dim) {
        pLogger->log(RC::MISMATCHING_DIMENSIONS, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    if (_dim == 0 && _size == 0) {
        _dim = val->getDim();
        _size = 1;

        const double *ValData = val->getData();
        memcpy(_data, ValData, _dim * sizeof(double));
        availableIndexes.push_back(0);

        return RC::SUCCESS;
    }

    int indexOfEqualData;
    RC code;
    if ((code = FindEqualData(val, n, tol, indexOfEqualData)) != RC::SUCCESS)
        return code;

    if (indexOfEqualData != -1)
        return RC::SUCCESS;


    if (_capacity < (_size + 1) * _dim * sizeof(double)) {
        _capacity *= 2;
        auto *tmp = new double[_capacity];
        memmove(tmp, _data, _capacity / 2);
        delete _data;
        _data = tmp;
    }

    const double *vecData = val->getData();

    memcpy(_data + _size * _dim, vecData, _dim * sizeof(double));
    maxHash++;
    availableIndexes.push_back(maxHash);
    _size += 1;

    return RC::SUCCESS;
}

RC ISetImpl::remove(size_t index) {

    if (index < 0 || index > _size - 1) {
        pLogger->log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    availableIndexes.erase(availableIndexes.begin() + index);

    if (index == _size - 1) {
        _size--;
        return RC::SUCCESS;
    }

    memmove(_data + index * _dim, _data + (index + 1) * _dim,
            (_size - index - 1) * _dim * sizeof(double));
    _size--;
    return RC::SUCCESS;
}

RC ISetImpl::remove(const IVector *const &pat, IVector::NORM n, double tol) {
    if (pat == nullptr || n == IVector::NORM::AMOUNT || tol < 0) {
        pLogger->log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    int indexOfEqualData = 0, startIndex = 0, numOfDataRemoved = 0;
    RC code;

    do {
        if ((code = FindEqualData(pat, n, tol, indexOfEqualData, startIndex)) != RC::SUCCESS)
            return code;

        if (indexOfEqualData != -1)
        {
            remove(indexOfEqualData);
            availableIndexes.erase(availableIndexes.begin() + indexOfEqualData);
            startIndex = indexOfEqualData;
            numOfDataRemoved++;
        }
    }while (indexOfEqualData != -1);

    if (numOfDataRemoved == 0)
        return RC::VECTOR_NOT_FOUND;

    return RC::SUCCESS;
}


RC ISetImpl::getCopy(size_t index, IVector *&val) const {
    if (index < 0 || index > _dim - 1)
    {
        pLogger->log(RC::INVALID_ARGUMENT, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }

    IVector* newVector = IVector::createVector(_dim, new double[_dim]);
    RC rc = getCoords(index, newVector);
    if (rc != RC::SUCCESS)
    {
        val = nullptr;
        return rc;
    }

    val = newVector;

    return RC::SUCCESS;
}

RC ISetImpl::FindEqualData(const IVector *const &pat, IVector::NORM n, double tol, int& index, int startIndex) const {
    if (startIndex > _size - 1)
    {
        index = std::numeric_limits<int>::quiet_NaN();
        return RC::SUCCESS;
    }

    const double *currentData = new double[_dim];
    if (!currentData)
    {
        pLogger->log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    IVector *currentVector = IVector::createVector(_dim, currentData);

    if (!currentVector)
    {
        pLogger->log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    for (size_t i = startIndex; i < _size; i++) {
        memcpy((void *) currentData, _data + i * _dim, _dim * sizeof(double));
        currentVector->setData(_dim, currentData);

        IVector *diff = IVector::sub(currentVector, pat);
        if (!diff)
        {
            delete[] currentData;
            delete currentVector;
            pLogger->log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
            return RC::NULLPTR_ERROR;
        }

        if (diff->norm(n) < tol)
        {
            index = i;
            delete diff;
            delete[] currentData;
            delete currentVector;
            return RC::SUCCESS;
        }

        delete diff;
    }

    index = -1;
    delete[] currentData;
    delete currentVector;
    return RC::SUCCESS;
}

RC ISetImpl::setLogger(ILogger *const logger) {
    if (!logger)
    {
        log(RC::NULLPTR_ERROR, ILogger::Level::SEVERE, __FILE__, __FUNCTION__, __LINE__);
        return RC::NULLPTR_ERROR;
    }

    pLogger = logger;
    return RC::SUCCESS;
}

void ISetImpl::log(RC code, ILogger::Level level, const char *const &srcfile, const char *const &function, int line) {
    if (pLogger != nullptr)
        pLogger->log(code, level, srcfile, function, line);
}

bool ISetImpl::IIteratorImpl::isBegin() const {
    return _cntrl_block->isBegin(_index);
}

bool ISetImpl::IControlBlockImpl::isBegin(size_t index) {
    return index <= _set->availableIndexes[0];
}

bool ISetImpl::IIteratorImpl::isEnd() const {
    return _cntrl_block->isEnd(_index);
}

bool ISetImpl::IControlBlockImpl::isEnd(size_t index) {
    return index >= _set->availableIndexes[_set->availableIndexes.size() - 1];
}

RC ISetImpl::IControlBlockImpl::getNext(IVector *const &vec, size_t &index, size_t indexInc) const {
    size_t sizeAvailableIndexes = _set->availableIndexes.size();
    int nIndex = -1;

    for (size_t i = 0; i < sizeAvailableIndexes; i++)
    {
        if (_set->availableIndexes[i] > index)
        {
            nIndex = i;
            break;
        }
    }

    if (nIndex < 0)
        return RC::INVALID_ARGUMENT;

    index =_set->availableIndexes[nIndex];
    RC rc = _set->getCoords(nIndex, vec);
    if (rc != RC::SUCCESS)
    {
        ISetImpl::log(rc, ILogger::Level::SEVERE, __FILE__, __func__, __LINE__);
        return rc;
    }

    return RC::SUCCESS;
}

RC ISetImpl::IControlBlockImpl::getPrevious(IVector *const &vec, size_t &index, size_t indexInc) const {
    size_t sizeAvailableIndexes = _set->availableIndexes.size();
    int nIndex = -1;

    for (size_t i = 0; i < sizeAvailableIndexes; i++)
    {
        if (_set->availableIndexes[i] >= index)
        {
            nIndex = i - 1;
            break;
        }
    }

    if (nIndex < 0)
        return RC::INVALID_ARGUMENT;

    index = _set->availableIndexes[nIndex];
    RC rc = _set->getCoords(nIndex, vec);
    if (rc != RC::SUCCESS)
    {
        ISetImpl::log(rc, ILogger::Level::SEVERE, __FILE__, __func__, __LINE__);
        return rc;
    }

    return RC::SUCCESS;
}

ISet::IIterator *ISetImpl::getIterator(size_t index) const {
    if (_size == 0)
        return nullptr;


    IVector* vec;
    RC rc = getCopy(index, vec);

    if (rc != RC::SUCCESS)
        return nullptr;

    size_t currentIndex = availableIndexes[index];
    std::shared_ptr<ISetControlBlock> tmpCtrlBlock = std::dynamic_pointer_cast<ISetControlBlock>(controlBlock);

    IIterator* it = new (std::nothrow)IIteratorImpl(tmpCtrlBlock, vec, currentIndex);
    delete vec;

    return it;
}

ISet::IIterator *ISetImpl::getBegin() const {
    if (_size == 0)
        return nullptr;

    IVector* vec;
    RC rc = getCopy(0, vec);
    if (rc != RC::SUCCESS)
        return nullptr;

    size_t index = availableIndexes[0];
    std::shared_ptr<ISetControlBlock> tmpCtrlBlock = std::dynamic_pointer_cast<ISetControlBlock>(controlBlock);
    IIterator* it = new (std::nothrow)IIteratorImpl(tmpCtrlBlock, vec, index);
    delete vec;

    return it;
}

ISet::IIterator *ISetImpl::getEnd() const {
    if (_size == 0)
        return nullptr;

    IVector* vec;
    RC rc = getCopy(_size - 1, vec);
    if (rc != RC::SUCCESS)
        return nullptr;

    size_t index = availableIndexes[_size - 1];
    std::shared_ptr<ISetControlBlock> tmpCtrlBlock = std::dynamic_pointer_cast<ISetControlBlock>(controlBlock);
    IIterator* it = new (std::nothrow)IIteratorImpl(tmpCtrlBlock, vec, index);
    delete vec;

    return it;
}

ISet::IIterator* ISetImpl::IIteratorImpl::getNext(size_t indexInc) const {
    IIterator* cloneIterator = this->clone();

    cloneIterator->next(indexInc);

    return cloneIterator;
}

ISet::IIterator *ISetImpl::IIteratorImpl::getPrevious(size_t indexInc) const {
    IIterator* cloneIterator = this->clone();

    cloneIterator->previous(indexInc);

    return cloneIterator;
}

ISet::IIterator *ISetImpl::IIteratorImpl::clone() const {
    IVector* vec = _vec->clone();
    size_t index = _index;

    std::shared_ptr<ISetControlBlock> tmpCtrlBlock = std::static_pointer_cast<ISetControlBlock>(_cntrl_block);
    IIterator* it = new (std::nothrow)IIteratorImpl(tmpCtrlBlock, vec, index);
    delete vec;

    return it;
}

RC ISetImpl::IIteratorImpl::setLogger(ILogger *const pLogger) {
    return RC::INDEX_OUT_OF_BOUND;
}

RC ISetImpl::IIteratorImpl::next(size_t indexInc) {
    size_t i = indexInc;

    while (i != 0)
    {
        RC rc = _cntrl_block->getNext(_vec, _index, indexInc);
        if (rc == RC::INVALID_ARGUMENT)
            return rc;

        i-= 1;
    }
    return RC::SUCCESS;
}

RC ISetImpl::IIteratorImpl::previous(size_t indexInc) {
    size_t i = indexInc;

    while (i != 0)
    {
        RC rc = _cntrl_block->getPrevious(_vec, _index, indexInc);
        if (rc == RC::INVALID_ARGUMENT)
            return rc;

        i -= 1;
    }
    return RC::SUCCESS;
}

RC ISetImpl::IIteratorImpl::getVectorCopy(IVector *&val) const {
    IVector* vec = this->_vec->clone();
    if (vec == nullptr)
        return RC::NULLPTR_ERROR;

    val = vec;
    return RC::SUCCESS;
}

RC ISetImpl::IIteratorImpl::getVectorCoords(IVector *const &val) const {
    const double* currentData = _vec->getData();
    RC rc = val->setData(_vec->getDim(), currentData);
    if (rc != RC::SUCCESS)
        return rc;

    return RC::SUCCESS;
}

size_t ISetImpl::IIteratorImpl::getIndex() const {
    return _index;
}

ISetImpl::IIteratorImpl::IIteratorImpl(std::shared_ptr<ISetControlBlock>& cntrl_block, IVector* vec, size_t index) {
    _cntrl_block = cntrl_block;
    _index = index;
    _vec = vec->clone();
}

ISetImpl::IIteratorImpl::~IIteratorImpl(){
    delete _vec;
}


ISet *ISetImpl::clone() const {
//    TODO: доделать
    ISetImpl* newSet = new ISetImpl;

    newSet->_dim = this->_dim;

}

ISetImpl::~ISetImpl() {
    delete _data;
}




