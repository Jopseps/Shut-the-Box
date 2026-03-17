#ifndef DATETIME_H
#define DATETIME_H
#pragma once

struct time {
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

#endif // DATETIME_H
