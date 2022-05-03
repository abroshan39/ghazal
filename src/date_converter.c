/*
       JavaScript functions for the Fourmilab Calendar Converter:
                  by John Walker  --  September, MMIX
              http://www.fourmilab.ch/documents/calendar/

                            Converted to C:
                 by Aboutaleb Roshan  --  October, MMXI
        27 Mehr, 1390 (21 Dhu al-Qidah, 1432) (19 October, 2011)
                         ab.roshan39@gmail.com
*/

#include <math.h>

float persian_to_jd(int, int, int);
void jd_to_persian(float, int *, int *, int *);
int leap_gregorian(int);
float gregorian_to_jd(int, int, int);
float modFJS(float, float);
//********************************************************************************************//
////////////////////////////////////    PERSIAN CALENDAR    ////////////////////////////////////
const float PERSIAN_EPOCH = 1948320.5;

float persian_to_jd(int day, int month, int year)
{
    float epbase, epyear;

    epbase = year - ((year >= 0) ? 474 : 473);
    epyear = 474 + modFJS(epbase, 2820);

    return day + ((month <= 7) ? ((month - 1) * 31) : (((month - 1) * 30) + 6)) +
            floor(((epyear * 682) - 110) / 2816) + (epyear - 1) * 365 + floor(epbase / 2820) * 1029983 +
            (PERSIAN_EPOCH - 1);
}

void jd_to_persian(float jd, int *day, int *month, int *year)
{
    float wjd, depoch, cycle, cyear, ycycle, aux1, aux2, yday;

    wjd = floor(jd) + 0.5;
    depoch = wjd - persian_to_jd(1, 1, 475);
    cycle = floor(depoch / 1029983);
    cyear = modFJS(depoch, 1029983);
    if(cyear == 1029982)
    {
        ycycle = 2820;
    }
    else
    {
        aux1 = floor(cyear / 366);
        aux2 = modFJS(cyear, 366);
        ycycle = floor(((2134 * aux1) + (2816 * aux2) + 2815) / 1028522) + aux1 + 1;
    }

    *year = ycycle + (2820 * cycle) + 474;
    if((*year) <= 0) (*year)--;

    yday = (wjd - persian_to_jd(1, 1, *year)) + 1;
    *month = (yday <= 186) ? ceil(yday / 31) : ceil((yday - 6) / 30);
    *day = (wjd - persian_to_jd(1, *month, *year)) + 1;
}
////////////////////////////////////    PERSIAN CALENDAR    ////////////////////////////////////
//********************************************************************************************//
////////////////////////////////////   GREGORIAN CALENDAR   ////////////////////////////////////
const float GREGORIAN_EPOCH = 1721425.5;

int leap_gregorian(int year)
{
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

float gregorian_to_jd(int day, int month, int year)
{
    return (GREGORIAN_EPOCH - 1) + (365 * (year - 1)) + floor((year - 1) / (float)4) +
            (-floor((year - 1) / (float)100)) + floor((year - 1) / (float)400) +
            floor((((367 * month) - 362) / (float)12) +
            ((month <= 2) ? 0 : (leap_gregorian(year) ? -1 : -2)) + day);
}
////////////////////////////////////   GREGORIAN CALENDAR   ////////////////////////////////////
//********************************************************************************************//
float modFJS(float n1, float n2)
{
    float a = fabs(fmod(n1, n2));
    if((n1 < 0) && (n2 > 0) && (a != 0)) return n2 - a;
    if((n1 > 0) && (n2 < 0) && (a != 0)) return -(fabs(n2) - a);
    if((n1 < 0) && (n2 < 0) && (a != 0)) return -a;
    return a;
}

void gregorian_persian(int *day, int *month, int *year)
{
    jd_to_persian(gregorian_to_jd(*day, *month, *year), day, month, year);
}
