#include "VectorImpl.cpp"
#include "../include/IVector.h"
#include "../include/ILogger.h"
#include "../include/ISet.h"
#include "../src/ISetImpl.cpp"
#include <iostream>
#include <cmath>
#include <limits>

#include <queue>


int main() {



    double data1[] {1, 2, 3, -3, 6};
    double data2[] {2, 2, 2, 2, 2};
    double data3[] {6, 3, 9, 1, 8};
    double data4[] {1, 6, 3, 9, 8};
//
//    std::cout << std::numeric_limits<double>::infinity() - 5000 << std::endl;
    ILogger* log = ILogger::createLogger("log1.txt");
    IVector::setLogger(log);
//
    const IVector* vec1 = IVector::createVector(5, data1);
    const IVector* vec2 = IVector::createVector(5, data2);
    const IVector* vec3 = IVector::createVector(5, data3);
    const IVector* vec4 = IVector::createVector(5, data4);
//

    ISet* set = ISet::createSet();
    set->setLogger(log);

    set->insert(vec1, IVector::NORM::SECOND, 0.1);
    set->insert(vec2, IVector::NORM::SECOND, 0.1);
    set->insert(vec3, IVector::NORM::SECOND, 0.01);
    set->insert(vec4, IVector::NORM::SECOND, 0.1);




//    set->remove(vec3, IVector::NORM::FIRST, 0.1);

    IVector* vecGet;

    ISet::IIterator* it1 = set->getIterator(0);
    it1->getVectorCopy(vecGet);
    vecGet->foreach([](double x) {std::cout << x << std::endl;});
    ISet::IIterator* it2 = it1->getNext(3);
    std::cout << "Third" << std::endl;
    it2->getVectorCopy(vecGet);
    vecGet->foreach([](double x) {std::cout << x << std::endl;});

    ISet::IIterator* it3 = it2->getNext(1);
    std::cout << "Third" << std::endl;
    it3->getVectorCopy(vecGet);
    vecGet->foreach([](double x) {std::cout << x << std::endl;});

}