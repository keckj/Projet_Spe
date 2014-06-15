#ifndef INITIALCONDFACTORY_H
#define INITIALCONDFACTORY_H

#include "headers.hpp"
#include "initialCond.hpp"


class InitialCondFactory {

    public:
        static const InitialCond<float> *getInitialCond(int i);
        static const QString getInitialCondName(int i);
};

#endif

