# C++ Jewish date header only library

A C++20 jewish date libray in the same style of std::chrono date. As the compilers i tried dont have std::chrono date classes the library currently uses [Howard Hinnant's date library](https://github.com/HowardHinnant/date). 

### Example usage:

    #include <jewish/date.h>
    using namespace jewish::literals;
    static_assert(date::year_month_day(5781_y / Tishrei / 1) == date::year(2020) / 9 / 19);
    static_assert(5781_y / Tishrei / 1 == jewish::year_month_day(date::year(2020) / 9 / 19));

### Calendar example output:

    $ ./examples/jcal 5781
    +-------------------------------------------------------------------------------------------+
    |                                                                                      5781 | 
    +----------------------+----------------------+----------------------+----------------------+
    |              Tishrei |             Cheshvan |               Kislev |                Tevet | 
    +----------------------+----------------------+----------------------+----------------------+
    |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S | 
    +----------------------+----------------------+----------------------+----------------------+
    |                   01 |    01 02 03 04 05 06 |       01 02 03 04 05 |          01 02 03 04 | 
    | 02 03 04 05 06 07 08 | 07 08 09 10 11 12 13 | 06 07 08 09 10 11 12 | 05 06 07 08 09 10 11 | 
    | 09 10 11 12 13 14 15 | 14 15 16 17 18 19 20 | 13 14 15 16 17 18 19 | 12 13 14 15 16 17 18 | 
    | 16 17 18 19 20 21 22 | 21 22 23 24 25 26 27 | 20 21 22 23 24 25 26 | 19 20 21 22 23 24 25 | 
    | 23 24 25 26 27 28 29 | 28 29                | 27 28 29             | 26 27 28 29          | 
    | 30                   |                      |                      |                      | 
    +----------------------+----------------------+----------------------+----------------------+
    |                Shvat |                 Adar |                Nisan |                 Iyar | 
    +----------------------+----------------------+----------------------+----------------------+
    |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S | 
    +----------------------+----------------------+----------------------+----------------------+
    |             01 02 03 |                   01 | 01 02 03 04 05 06 07 |       01 02 03 04 05 | 
    | 04 05 06 07 08 09 10 | 02 03 04 05 06 07 08 | 08 09 10 11 12 13 14 | 06 07 08 09 10 11 12 | 
    | 11 12 13 14 15 16 17 | 09 10 11 12 13 14 15 | 15 16 17 18 19 20 21 | 13 14 15 16 17 18 19 | 
    | 18 19 20 21 22 23 24 | 16 17 18 19 20 21 22 | 22 23 24 25 26 27 28 | 20 21 22 23 24 25 26 | 
    | 25 26 27 28 29 30    | 23 24 25 26 27 28 29 | 29 30                | 27 28 29             | 
    |                      |                      |                      |                      | 
    +----------------------+----------------------+----------------------+----------------------+
    |                Sivan |               Tammuz |                   Av |                 Elul | 
    +----------------------+----------------------+----------------------+----------------------+
    |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S |  S  M  T  W  T  F  S | 
    +----------------------+----------------------+----------------------+----------------------+
    |          01 02 03 04 |                01 02 |                   01 |    01 02 03 04 05 06 | 
    | 05 06 07 08 09 10 11 | 03 04 05 06 07 08 09 | 02 03 04 05 06 07 08 | 07 08 09 10 11 12 13 | 
    | 12 13 14 15 16 17 18 | 10 11 12 13 14 15 16 | 09 10 11 12 13 14 15 | 14 15 16 17 18 19 20 | 
    | 19 20 21 22 23 24 25 | 17 18 19 20 21 22 23 | 16 17 18 19 20 21 22 | 21 22 23 24 25 26 27 | 
    | 26 27 28 29 30       | 24 25 26 27 28 29    | 23 24 25 26 27 28 29 | 28 29                | 
    |                      |                      | 30                   |                      | 
    +----------------------+----------------------+----------------------+----------------------+


## Notable differences

A jewish year leap year has 13 months. To handle this a few more classes were added. 

### Dependencies

[Howard Hinnant's date library](https://github.com/HowardHinnant/date). 