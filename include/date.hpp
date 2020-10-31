//=======================================================================
// Copyright (c) 2013-2020 Baptiste Wicht.
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include <ctime>

#include "utils.hpp"

namespace budget {

using date_type = unsigned short;

class date_exception: public std::exception {
    protected:
        std::string _message;

    public:
        date_exception(const std::string& message) : _message(message){}

        /*!
         * Return the error message.
         * \return The error message.
         */
        const std::string& message() const {
            return _message;
        }

        virtual const char* what() const throw() {
            return _message.c_str();
        }
};

struct day {
    date_type value;
    day(date_type value) : value(value) {}
    operator date_type() const { return value; }

    bool is_default() const {
        return value == 0;
    }
};

struct month {
    date_type value;
    month(date_type value) : value(value) {}
    operator date_type() const { return value; }

    month& operator=(date_type value){
        this->value = value;

        return *this;
    }

    std::string as_short_string() const {
        static constexpr const char* months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

        return months[value-1];
    }

    std::string as_long_string() const {
        static constexpr const char* months[12] = {"January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"};

        return months[value-1];
    }

    bool is_default() const {
        return value == 0;
    }
};

struct year {
    date_type value;
    year(date_type value) : value(value) {}
    operator date_type() const { return value; }

    year& operator=(date_type value){
        this->value = value;

        return *this;
    }

    bool is_default() const {
        return value == 0;
    }
};

struct days {
    date_type value;
    explicit days(date_type value) : value(value) {}
    operator date_type() const { return value; }
};

struct months {
    date_type value;
    explicit months(date_type value) : value(value) {}
    operator date_type() const { return value; }

    months& operator=(date_type value){
        this->value = value;

        return *this;
    }
};

struct years {
    date_type value;
    explicit years(date_type value) : value(value) {}
    operator date_type() const { return value; }
};

struct date;

std::ostream& operator<<(std::ostream& stream, const month& month);
std::ostream& operator<<(std::ostream& stream, const date& date);

struct date {
    date_type _year;
    date_type _month;
    date_type _day;

    explicit date(){}

    date(date_type year, date_type month, date_type day) : _year(year), _month(month), _day(day){
        if(year < 1400){
            throw date_exception("Year not in the valid range: " + std::to_string(year));
        }

        if(month == 0 || month > 12){
            throw date_exception("Invalid month: " + std::to_string(month));
        }

        if(day == 0 || day > days_month(year, month)){
            throw date_exception("Invalid day: " + std::to_string(month));
        }
    }

    date(const date& d) : _year(d._year), _month(d._month), _day(d._day) {
        //Nothing else
    }

    date& operator=(const date& rhs) {
        if (this != &rhs) {
            _year  = rhs._year;
            _month = rhs._month;
            _day   = rhs._day;
        }

        return *this;
    }

    budget::year year() const {
        return _year;
    }

    budget::month month() const {
        return _month;
    }

    budget::day day() const {
        return _day;
    }

    // The number of days of this year until today
    // January 1 is 1
    size_t year_days() const {
        size_t result = 0;

        for (size_t m = 1; m < _month; ++m) {
            result += days_month(_year, m);
        }

        result += _day;

        return result;
    }

    // The current week number
    size_t week() const {
        return 1 + year_days() / 7;
    }

    date start_of_week() {
        date r = *this;

        auto w = r.week();
        auto d = r.year_days();

        r -= days(d - (w - 1) * 7);

        return r;
    }

    date_type day_of_the_week() const {
        static constexpr const date_type t[12] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};

        // Thanks to leap years, this has to be complicated!

        date_type y = _year - (_month < 3);
        date_type dow = (y + y / 4 - y / 100 + y / 400 + t[_month - 1] + _day) % 7;
        return dow ? dow : 7;
    }

    static date_type days_month(date_type year, date_type month){
        static constexpr const date_type month_days[12] = {31,0,31,30,31,30,31,31,30,31,30,31};

        if(month == 2){
            return is_leap(year) ? 29 : 28;
        } else {
            return month_days[month - 1];
        }
    }

    static bool is_leap(date_type year){
        return
                ((year % 4 == 0) && year % 100 != 0)
            ||  year % 400 == 0;
    }

    bool is_leap() const {
        return is_leap(_year);
    }

    date end_of_month(){
        return {_year, _month, days_month(_year, _month)};
    }

    date& operator+=(years years){
        if(years >= std::numeric_limits<date_type>::max() - _year){
            throw date_exception("Year too high (will overflow)");
        }

        _year += years;

        return *this;
    }

    date& operator+=(months months){
        // Handle the NOP addition
        if(months == 0){
            return *this;
        }

        // First add several years if necessary
        if (months >= 12) {
            *this += years(months.value / 12);
            months = months.value % 12;
        }

        // Add the remaining months
        _month += months;

        // Update the year if necessary
        if(_month > 12){
            *this += years((_month - 1) / 12);
            _month = (_month) % 12;
        }

        // Update the day of month, if necessary
        _day = std::min(_day, days_month(_year, _month));

        return *this;
    }

    date& operator+=(days d){
        while(d > 0){
            ++_day;

            if(_day > days_month(_year, _month)){
                _day = 1;
                ++_month;

                if(_month > 12){
                    _month = 1;
                    ++_year;
                }
            }

            d = days(d - 1);
        }

        return *this;
    }

    date& operator-=(years years){
        if(_year < years){
            throw date_exception("Year too low");
        }

        _year -= years;

        return *this;
    }

    date& operator-=(months months){
        // Handle the NOP subtraction
        if(months == 0){
            return *this;
        }

        // First remove several years if necessary
        if (months >= 12) {
            *this -= years(months.value / 12);
            months = months.value % 12;
        }

        if(_month == months){
            *this -= years(1);
            _month = 12;
        } else if(_month < months){
            *this -= years(1);
            _month = 12 - (int(months) - _month);
        } else {
            _month -= months;
        }

        _day = std::min(_day, days_month(_year, _month));

        return *this;
    }

    date& operator-=(days d){
        while(d > 0){
            --_day;
            if(_day == 0){
                --_month;

                if(_month == 0){
                    --_year;
                    _month = 12;
                }

                _day = days_month(_year, _month);
            }

            d = days(d - 1);
        }

        return *this;
    }

    date operator+(years years) const {
        date d(*this);
        d += years;
        return d;
    }

    date operator+(months months) const {
        date d(*this);
        d += months;
        return d;
    }

    date operator+(days days) const {
        date d(*this);
        d += days;
        return d;
    }

    date operator-(years years) const {
        date d(*this);
        d -= years;
        return d;
    }

    date operator-(months months) const {
        date d(*this);
        d -= months;
        return d;
    }

    date operator-(days days) const {
        date d(*this);
        d -= days;
        return d;
    }

    bool operator==(const date& rhs) const {
        return _year == rhs._year && _month == rhs._month && _day == rhs._day;
    }

    bool operator!=(const date& rhs) const {
        return !(*this == rhs);
    }

    bool operator<(const date& rhs) const {
        if(_year < rhs._year){
            return true;
        } else if(_year == rhs._year){
            if(_month < rhs._month){
                return true;
            } else if(_month == rhs._month){
                return _day < rhs._day;
            }
        }

        return false;
    }

    bool operator<=(const date& rhs) const {
        return (*this == rhs) || (*this < rhs);
    }

    bool operator>(const date& rhs) const {
        if(_year > rhs._year){
            return true;
        } else if(_year == rhs._year){
            if(_month > rhs._month){
                return true;
            } else if(_month == rhs._month){
                return _day > rhs._day;
            }
        }

        return false;
    }

    bool operator>=(const date& rhs) const {
        return (*this == rhs) || (*this > rhs);
    }

    date_type operator-(const date& rhs) const {
        if(*this == rhs){
            return 0;
        }

        if(rhs > *this){
            return -(rhs - *this);
        }

        auto x = *this;
        size_t d = 0;

        while(x != rhs){
            x -= days(1);
            ++d;
        }

        return d;
    }
};

date local_day();

date from_string(const std::string& str);

std::string date_to_string(date date);

template<>
inline std::string to_string(budget::date date){
    return date_to_string(date);
}

unsigned short start_year();
unsigned short start_month(budget::year year);

} //end of namespace budget
