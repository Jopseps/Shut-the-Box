#ifndef TOOLS_H
#define TOOLS_H

#include <ctime>
#include <cstdlib>

// seed

int sumSeconds(){
    time_t timestamp;
    time(&timestamp);
    struct tm* t = localtime(&timestamp);

    int mdayInSec = t->tm_mday * 86400;
    int hourInSec = t->tm_hour * 3600;
    int minInSec  = t->tm_min  * 60;
    int secInSec  = t->tm_sec  * 1;

    return mdayInSec + hourInSec + minInSec + secInSec;
}

void randomizeSeed(){
    srand(sumSeconds());
}

#endif
