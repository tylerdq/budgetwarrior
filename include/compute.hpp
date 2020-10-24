//=======================================================================
// Copyright (c) 2013-2020 Baptiste Wicht.
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#pragma once

#include "money.hpp"
#include "date.hpp"

namespace budget {

struct status {
    budget::money expenses;
    budget::money taxes;
    budget::money earnings;
    budget::money budget;
    budget::money balance;
    budget::money base_income;
    budget::money income;
    budget::money savings;

    status add_expense(budget::money expense){
        auto new_status = *this;

        new_status.expenses += expense;
        new_status.balance -= expense;

        return new_status;
    }

    status add_earning(budget::money earning){
        auto new_status = *this;

        new_status.earnings += earning;
        new_status.balance += earning;

        return new_status;
    }
};

struct data_cache;

status compute_year_status(data_cache & cache);
status compute_year_status(data_cache & cache, budget::year year);
status compute_year_status(data_cache & cache, budget::year year, budget::month last);

status compute_month_status(data_cache & cache);
status compute_month_status(data_cache & cache, budget::month year);
status compute_month_status(data_cache & cache, budget::year year, budget::month month);

status compute_avg_month_status(data_cache & cache);
status compute_avg_month_status(data_cache & cache, budget::month year);
status compute_avg_month_status(data_cache & cache, budget::year year, budget::month month);

} //end of namespace budget
