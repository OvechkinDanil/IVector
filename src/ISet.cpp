#include "../include/ISet.h"
#include "../src/ISetImpl.cpp"

RC ISet::setLogger(ILogger* const logger)
{
    return ISetImpl::setLogger(logger);
}

ISet *ISet::createSet() {
    return new(std::nothrow)ISetImpl();
}

ISet *ISet::makeIntersection(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) {
    if (!op1 || !op2 || n == IVector::NORM::AMOUNT || tol < 0 || op1->getSize() == 0 || op2->getSize() == 0 ||
        op1->getDim() != op2->getDim())
        return nullptr;

    IVector* vectorForInsert = IVector::createVector(op1->getDim(), new double[op1->getDim()]);
    ISet* intersectionSet = ISet::createSet();
    if (!intersectionSet)
        return nullptr;

    ISet::IIterator* itOp1 = op1->getBegin();
    bool isEnd = false;

    if (itOp1 != nullptr) {
        do {
            isEnd = itOp1->isEnd();

            itOp1->getVectorCoords(vectorForInsert);

            IVector *vectorForCheck;
            switch (op2->findFirstAndCopy(vectorForInsert, n, tol, vectorForCheck)) {
                case RC::SUCCESS: {
                    delete vectorForCheck;
                    if (intersectionSet->insert(vectorForInsert, n, tol) != RC::SUCCESS) {
                        return nullptr;
                    }
                }
                    break;
                case RC::VECTOR_NOT_FOUND:
                    continue;
                default:
                    return nullptr;
            }
        } while (!isEnd);
        delete itOp1;
        itOp1 = nullptr;
        return intersectionSet;
    }

    return nullptr;
}

ISet *ISet::makeUnion(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) {
    if (!op1 || !op2 || n == IVector::NORM::AMOUNT || tol < 0 || op1->getSize() == 0 || op2->getSize() == 0 ||
        op1->getDim() != op2->getDim())
        return nullptr;

    ILogger* pLogger = ILogger::createLogger();
    IVector* vectorForInsert = IVector::createVector(op1->getDim(), new double[op1->getDim()]);
    ISet* unionSet = ISet::createSet();
    if (!unionSet)
        return nullptr;

    ISet::IIterator* itOp1 = op1->getBegin();
    bool isEnd = false;

    do {
        isEnd = itOp1->isEnd();

        if (itOp1->getVectorCoords(vectorForInsert) != RC::SUCCESS)
            return nullptr;
        if (unionSet->insert(vectorForInsert, n, tol) != RC::SUCCESS)
            return nullptr;
    }while (!isEnd);

    delete itOp1;
    itOp1 = nullptr;

    isEnd = false;
    ISet::IIterator* itOp2 = op2->getBegin();

    do {
        isEnd = itOp2->isEnd();

        if (itOp2->getVectorCoords(vectorForInsert) != RC::SUCCESS)
            return nullptr;
        if (unionSet->insert(vectorForInsert, n, tol) != RC::SUCCESS)
            return nullptr;

    }while (!isEnd);

    delete itOp2;
    itOp2 = nullptr;

    return unionSet;
}

ISet *ISet::sub(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) {
    if (!op1 || !op2 || n == IVector::NORM::AMOUNT || tol < 0 || op1->getSize() == 0 || op2->getSize() == 0 ||
        op1->getDim() != op2->getDim())
        return nullptr;

    IVector* vectorForInsert = IVector::createVector(op1->getDim(), new double [op1->getDim()]);
    ISet* newSet = ISet::createSet();
    if (!newSet)
        return nullptr;

    ISet::IIterator* itOp1 = op1->getBegin();
    bool isEnd = false;

    do {
        isEnd = itOp1->isEnd();
        if (itOp1->getVectorCoords(vectorForInsert) != RC::SUCCESS)
            return nullptr;

        IVector* vectorForCheck;
        switch (op2->findFirstAndCopy(vectorForInsert, n, tol, vectorForCheck))
        {
            case RC::VECTOR_NOT_FOUND:
            {
                delete vectorForCheck;
                if (newSet->insert(vectorForInsert, n, tol) != RC::SUCCESS)
                {
                    return nullptr;
                }
                break;
            }
            case RC::SUCCESS:
                continue;
            default:
                return nullptr;

        }

    } while(!isEnd);

    return newSet;
}

ISet *ISet::symSub(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) {
    if (!op1 || !op2 || n == IVector::NORM::AMOUNT || tol < 0 || op1->getSize() == 0 || op2->getSize() == 0 ||
        op1->getDim() != op2->getDim())
        return nullptr;

    ISet* symSubSet = ISet::createSet();
    IVector* vectorForInsert = IVector::createVector(op1->getDim(), new double [op1->getDim()]);
    IVector* vectorForCheck = IVector::createVector(op1->getDim(), new double [op1->getDim()]);

    ISet::IIterator* itOp1 = op1->getBegin();
    bool isEnd = false;

    do {
        isEnd = itOp1->isEnd();
        if (itOp1->getVectorCoords(vectorForInsert) != RC::SUCCESS)
            return nullptr;

        switch (op2->findFirstAndCopyCoords(vectorForInsert, n, tol, vectorForCheck))
        {
            case RC::VECTOR_NOT_FOUND:
            {
                if (symSubSet->insert(vectorForInsert, n, tol) != RC::SUCCESS)
                {
                    return nullptr;
                }
                break;
            }
            case RC::SUCCESS:
                continue;
            default:
                return nullptr;

        }

    } while (!isEnd);

    isEnd = false;
    ISet::IIterator* itOp2 = op2->getBegin();

    do {
        isEnd = itOp2->isEnd();
        if (itOp2->getVectorCoords(vectorForInsert) != RC::SUCCESS)
            return nullptr;

        switch (op1->findFirstAndCopyCoords(vectorForInsert, n, tol, vectorForCheck))
        {
            case RC::VECTOR_NOT_FOUND:
            {
                if (symSubSet->insert(vectorForInsert, n, tol) != RC::SUCCESS)
                {
                    return nullptr;
                }
                break;
            }
            case RC::SUCCESS:
                continue;
            default:
                return nullptr;

        }

    } while (!isEnd);

    delete vectorForInsert;
    delete vectorForCheck;

    return symSubSet;
}

bool ISet::equals(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) {
    if (!op1 || !op2 || n == IVector::NORM::AMOUNT || tol < 0 || op1->getSize() == 0 || op2->getSize() == 0 ||
        op1->getDim() != op2->getDim())
        return false;

    return subSet(op1, op2, n, tol) == subSet(op2, op1, n, tol);
}

bool ISet::subSet(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) {
    if (!op1 || !op2 || n == IVector::NORM::AMOUNT || tol < 0 || op1->getSize() == 0 || op2->getSize() == 0 ||
        op1->getDim() != op2->getDim())
        return false;

    IVector* vectorFromSet = IVector::createVector(op1->getDim(), new double [op1->getDim()]);
    IVector* vectorForCheck = IVector::createVector(op1->getDim(), new double [op1->getDim()]);

    ISet::IIterator* itOp1 = op1->getBegin();
    bool isEnd = false;

    do {
        isEnd = itOp1->isEnd();
        if (itOp1->getVectorCoords(vectorFromSet) != RC::SUCCESS)
            return false;

        switch (op2->findFirstAndCopyCoords(vectorFromSet, n, tol, vectorForCheck))
        {
            case RC::VECTOR_NOT_FOUND:
            {
                return false;
            }
            case RC::SUCCESS:
                continue;
            default:
                return false;
        }

    } while (!isEnd);

    delete vectorFromSet;
    delete vectorForCheck;

    return true;
}

bool ISet::IIterator::equal(const ISet::IIterator *op1, const ISet::IIterator *op2) {
    if (!op1 || !op2)
        return false;

    return op1->getIndex() == op2->getIndex();
}

ISet::~ISet() = default;

ISet::IIterator::~IIterator() = default;

