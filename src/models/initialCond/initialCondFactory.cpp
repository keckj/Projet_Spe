#include "headers.hpp"
#include "initialCondFactory.hpp"
#include "initialCond.hpp"
#include "circleInitialCond.hpp"

using namespace log4cpp;

const InitialCond<float> &InitialCondFactory::getInitialCond(int i) {
    switch (i) {
        case 0:
            return CircleInitialCond<float>();
        default:
            log_console->errorStream() << "[InitialCondFactory] Unknown initial condition : " << i;
            exit(1338);
    }
}

const QString InitialCondFactory::getInitialCondName(int i) {
    switch (i) {
        case 0:
            return "Circle";
        default:
            return "";
    }
}

