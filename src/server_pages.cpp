//=======================================================================
// Copyright (c) 2013-2018 Baptiste Wicht.
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include <set>

#include "cpp_utils/assert.hpp"

#include "accounts.hpp"
#include "assets.hpp"
#include "config.hpp"
#include "debts.hpp"
#include "expenses.hpp"
#include "fortune.hpp"
#include "objectives.hpp"
#include "overview.hpp"
#include "recurring.hpp"
#include "report.hpp"
#include "summary.hpp"
#include "version.hpp"
#include "wishes.hpp"
#include "writer.hpp"

#include "server_pages.hpp"

#include "httplib.h"

using namespace budget;

namespace {

static constexpr const char new_line = '\n';

std::string header(const std::string& title, bool menu = true) {
    std::stringstream stream;

    // The header

    stream << R"=====(
        <!doctype html>
        <html lang="en">
          <head>
            <meta charset="utf-8">
            <meta name="viewport" content="width=device-width, initial-scale=1, shrink-to-fit=no">

            <meta name="description" content="budgetwarrior">
            <meta name="author" content="Baptiste Wicht">

            <!-- The CSS -->

            <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-beta.3/css/bootstrap.min.css" integrity="sha384-Zug+QiDoJOrZ5t4lssLdxGhVrurbmBWopoEl+M6BdEfwnCJZtKxi1KgxUyJq13dy" crossorigin="anonymous">

            <style>
                body {
                  padding-top: 5rem;
                }

                p {
                    margin-bottom: 8px;
                }

                .small-form-inline {
                    float: left;
                    padding-right: 10px;
                }

                .small-text {
                    font-size: 10pt;
                }

                .extend-only {
                    width: 75%;
                }

                .selector {
                    float: right;
                    font-size: xx-large;
                    pading-top: 5px;
                }

                .card {
                    margin-bottom: 10px !important;
                }

                .card-header-primary {
                    color:white !important;
                    background-color: #007bff !important;
                    padding: 0.5rem 0.75rem !important;
                }

                .gauge-cash-flow-title {
                    margin-top: -15px;
                }

                .gauge-objective-title {
                    color: rgb(124, 181, 236);
                    margin-top: -15px;
                    text-align: center;
                }

                .default-graph-style {
                    min-width: 300px;
                    height: 400px;
                    margin: 0 auto;
                }

                .dataTables_wrapper {
                    padding-left: 0px !important;
                    padding-right: 0px !important;
                }

                .flat-hr {
                    margin:0px;
                }
            </style>
    )=====";

    if (title.empty()) {
        stream << "<title>budgetwarrior</title>";
    } else {
        stream << "<title>budgetwarrior - " << title << "</title>";
    }

    stream << new_line;

    stream << "</head>" << new_line;
    stream << "<body>" << new_line;

    // The navigation

    stream << R"=====(<nav class="navbar navbar-expand-md navbar-dark bg-dark fixed-top">)=====";

    stream << "<a class=\"navbar-brand\" href=\"#\">" << budget::get_version() << "</a>";

    if (menu) {
        stream << R"=====(
          <button class="navbar-toggler" type="button" data-toggle="collapse" data-target="#navbarsExampleDefault" aria-controls="navbarsExampleDefault" aria-expanded="false" aria-label="Toggle navigation">
            <span class="navbar-toggler-icon"></span>
          </button>
          <div class="collapse navbar-collapse" id="navbarsExampleDefault">
            <ul class="navbar-nav mr-auto">
              <li class="nav-item">
                <a class="nav-link" href="/">Index <span class="sr-only">(current)</span></a>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown01" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Overview</a>
                <div class="dropdown-menu" aria-labelledby="dropdown01">
                  <a class="dropdown-item" href="/overview/">Overview Month</a>
                  <a class="dropdown-item" href="/overview/year/">Overview Year</a>
                  <a class="dropdown-item" href="/overview/aggregate/year/">Aggregate Year</a>
                  <a class="dropdown-item" href="/overview/aggregate/month/">Aggregate Month</a>
                  <a class="dropdown-item" href="/report/">Report</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown05" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Assets</a>
                <div class="dropdown-menu" aria-labelledby="dropdown05">
                  <a class="dropdown-item" href="/assets/">Assets</a>
                  <a class="dropdown-item" href="/net_worth/status/">Net worth Status</a>
                  <a class="dropdown-item" href="/net_worth/graph/">Net worth Graph</a>
                  <a class="dropdown-item" href="/net_worth/currency/">Net worth Currency</a>
                  <a class="dropdown-item" href="/portfolio/status/">Portfolio Status</a>
                  <a class="dropdown-item" href="/portfolio/graph/">Portfolio Graph</a>
                  <a class="dropdown-item" href="/portfolio/currency/">Portfolio Currency</a>
                  <a class="dropdown-item" href="/rebalance/">Rebalance</a>
                  <a class="dropdown-item" href="/assets/add/">Add Asset</a>
                  <a class="dropdown-item" href="/asset_values/list/">Asset Values</a>
                  <a class="dropdown-item" href="/asset_values/batch/">Batch Update</a>
                  <a class="dropdown-item" href="/asset_values/add/">Set One Asset Value</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown03" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Expenses</a>
                <div class="dropdown-menu" aria-labelledby="dropdown03">
                  <a class="dropdown-item" href="/expenses/add/">Add Expense</a>
                  <a class="dropdown-item" href="/expenses/">Expenses</a>
                  <a class="dropdown-item" href="/expenses/all/">All Expenses</a>
                  <a class="dropdown-item" href="/expenses/breakdown/month/">Expenses Breakdown Month</a>
                  <a class="dropdown-item" href="/expenses/breakdown/year/">Expenses Breakdown Year</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown04" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Earnings</a>
                <div class="dropdown-menu" aria-labelledby="dropdown04">
                  <a class="dropdown-item" href="/earnings/add/">Add Earning</a>
                  <a class="dropdown-item" href="/earnings/">Earnings</a>
                  <a class="dropdown-item" href="/earnings/all/">All Earnings</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown02" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Accounts</a>
                <div class="dropdown-menu" aria-labelledby="dropdown02">
                  <a class="dropdown-item" href="/accounts/">Accounts</a>
                  <a class="dropdown-item" href="/accounts/all/">All Accounts</a>
                  <a class="dropdown-item" href="/accounts/add/">Add Account</a>
                  <a class="dropdown-item" href="/accounts/archive/month/">Archive Account (month)</a>
                  <a class="dropdown-item" href="/accounts/archive/year/">Archive Account (year)</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown06" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Fortune</a>
                <div class="dropdown-menu" aria-labelledby="dropdown06">
                  <a class="dropdown-item" href="/fortunes/graph/">Fortune</a>
                  <a class="dropdown-item" href="/fortunes/status/">Status</a>
                  <a class="dropdown-item" href="/fortunes/list/">List</a>
                  <a class="dropdown-item" href="/fortunes/add/">Set fortune</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown06" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Objectives</a>
                <div class="dropdown-menu" aria-labelledby="dropdown06">
                  <a class="dropdown-item" href="/objectives/status/">Status</a>
                  <a class="dropdown-item" href="/objectives/list/">List</a>
                  <a class="dropdown-item" href="/objectives/add/">Add Objective</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown06" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Wishes</a>
                <div class="dropdown-menu" aria-labelledby="dropdown06">
                  <a class="dropdown-item" href="/wishes/status/">Status</a>
                  <a class="dropdown-item" href="/wishes/list/">List</a>
                  <a class="dropdown-item" href="/wishes/estimate/">Estimate</a>
                  <a class="dropdown-item" href="/wishes/add/">Add Wish</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown06" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Recurrings</a>
                <div class="dropdown-menu" aria-labelledby="dropdown06">
                  <a class="dropdown-item" href="/recurrings/list/">List</a>
                  <a class="dropdown-item" href="/recurrings/add/">Add Recurring Expense</a>
                </div>
              </li>
              <li class="nav-item dropdown">
                <a class="nav-link dropdown-toggle" href="#" id="dropdown06" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">Debts</a>
                <div class="dropdown-menu" aria-labelledby="dropdown06">
                  <a class="dropdown-item" href="/debts/add/">Add Debt</a>
                  <a class="dropdown-item" href="/debts/list/">Debts</a>
                  <a class="dropdown-item" href="/debts/all/">All Debts</a>
                </div>
              </li>
            </ul>
          </div>
    )=====";
    }

    stream << "</nav>" << new_line;

    // The main component

    stream << R"=====(<main role="main" class="container-fluid">)=====" << new_line;
    stream << "<div>" << new_line;

    return stream.str();
}

void display_error_message(budget::writer& w, const std::string& message) {
    w << R"=====(<div class="alert alert-danger" role="alert">)=====";
    w << message;
    w << R"=====(</div>)=====";
}

void display_message(budget::writer& w, const httplib::Request& req) {
    if (req.has_param("message")) {
        if (req.has_param("error")) {
            w << R"=====(<div class="alert alert-danger" role="alert">)=====";
        } else if (req.has_param("success")) {
            w << R"=====(<div class="alert alert-success" role="alert">)=====";
        } else {
            w << R"=====(<div class="alert alert-primary" role="alert">)=====";
        }

        w << req.params.at("message");
        w << R"=====(</div>)=====";
    }
}

bool page_start(const httplib::Request& req, httplib::Response& res, std::stringstream& content_stream, const std::string& title) {
    content_stream.imbue(std::locale("C"));

    if (req.has_header("Authorization")) {
        auto authorization = req.get_header_value("Authorization");

        if (authorization.substr(0, 6) != "Basic ") {
            res.status = 401;
            res.set_header("WWW-Authenticate", "Basic realm=\"budgetwarrior\"");

            return false;
        }

        auto sub_authorization = authorization.substr(6, authorization.size());
        auto decoded           = base64_decode(sub_authorization);

        if (decoded.find(':') == std::string::npos) {
            res.status = 401;
            res.set_header("WWW-Authenticate", "Basic realm=\"budgetwarrior\"");

            return false;
        }

        auto username = decoded.substr(0, decoded.find(':'));
        auto password = decoded.substr(decoded.find(':') + 1, decoded.size());

        if (username != get_web_user()) {
            res.status = 401;
            res.set_header("WWW-Authenticate", "Basic realm=\"budgetwarrior\"");

            return false;
        }

        if (password != get_web_password()) {
            res.status = 401;
            res.set_header("WWW-Authenticate", "Basic realm=\"budgetwarrior\"");

            return false;
        }
    } else {
        res.status = 401;
        res.set_header("WWW-Authenticate", "Basic realm=\"budgetwarrior\"");

        return false;
    }

    content_stream << header(title);

    budget::html_writer w(content_stream);
    display_message(w, req);

    return true;
}

void replace_all(std::string& source, const std::string& from, const std::string& to) {
    size_t current_pos = 0;

    while ((current_pos = source.find(from, current_pos)) != std::string::npos) {
        source.replace(current_pos, from.length(), to);
        current_pos += to.length();
    }
}

void filter_html(std::string& html, const httplib::Request& req) {
    replace_all(html, "__budget_this_page__", req.path);
    replace_all(html, "__currency__", get_default_currency());
}

//Note: This must be synchronized with page_end
std::string footer() {
    return "</div></main></body></html>";
}

void page_end(budget::html_writer& w, std::stringstream& content_stream, const httplib::Request& req, httplib::Response& res) {
    w << "</div></main>";
    w.load_deferred_scripts();
    w << "</body></html>";

    auto result = content_stream.str();

    filter_html(result, req);

    res.set_content(result, "text/html");
}

std::stringstream start_chart_base(budget::writer& w, const std::string& chart_type, const std::string& id = "container", std::string style = "") {
    w << R"=====(<div id=")=====";
    w << id;

    if (style.empty()) {
        w << R"=====(" class="default-graph-style"></div>)=====" << end_of_line;
    } else {
        w << R"=====(" style="margin: 0 auto; )=====";
        w << style;
        w << R"=====(""></div>)=====" << end_of_line;
    }

    std::stringstream ss;
    ss.imbue(std::locale("C"));

    ss << R"=====(Highcharts.chart(')=====";
    ss << id;
    ss << R"=====(', {)=====";

    ss << R"=====(chart: {type: ')=====";
    ss << chart_type;
    ss << R"=====('},)=====";

    ss << R"=====(credits: { enabled: false },)=====";
    ss << R"=====(exporting: { enabled: false },)=====";

    return ss;
}

std::stringstream start_chart(budget::writer& w, const std::string& title, const std::string& chart_type, const std::string& id = "container", std::string style = "") {
    auto ss = start_chart_base(w, chart_type, id, style);

    ss << R"=====(title: {text: ')=====";
    ss << title;
    ss << R"=====('},)=====";

    return ss;
}

void end_chart(budget::html_writer& w, std::stringstream& ss) {
    ss << R"=====(});)=====";

    w.defer_script(ss.str());
}

void add_date_picker(budget::writer& w, const std::string& default_value = "", bool one_line = false) {
    if (one_line) {
        w << R"=====(<div class="form-group row">)=====";

        w << "<label class=\"col-sm-4 col-form-label\" for=\"input_date\">Date</label>";

        w << R"=====(<div class="col-sm-4">)=====";
    } else {
        w << R"=====(<div class="form-group">)=====";

        w << "<label for=\"input_date\">Date</label>";
    }

    auto today = budget::local_day();

    w << R"=====(<input required type="date" class="form-control" id="input_date" name="input_date" value=")=====";

    if (default_value.empty()) {
        w << today.year() << "-";

        if (today.month() < 10) {
            w << "0" << today.month().value << "-";
        } else {
            w << today.month().value << "-";
        }

        if (today.day() < 10) {
            w << "0" << today.day();
        } else {
            w << today.day();
        }
    } else {
        w << default_value;
    }

    w << "\">";

    if (one_line) {
        w << "</div>";
        w << "</div>";
    } else {
        w << "</div>";
    }
}

void add_text_picker(budget::writer& w, const std::string& title, const std::string& name, const std::string& default_value) {
    w << R"=====(<div class="form-group">)=====";

    w << "<label for=\"" << name << "\">" << title << "</label>";

    w << "<input required type=\"text\" class=\"form-control\" id=\"" << name << "\" name=\"" << name << "\" ";

    if (default_value.empty()) {
        w << " placeholder=\"Enter " << title << "\"";
    } else {
        w << " value=\"" << default_value << "\" ";
    }

    w << R"=====(
            >
         </div>
    )=====";
}

void add_name_picker(budget::writer& w, const std::string& default_value = "") {
    add_text_picker(w, "Name", "input_name", default_value);
}

void add_title_picker(budget::writer& w, const std::string& default_value = "") {
    add_text_picker(w, "Title", "input_title", default_value);
}

void add_currency_picker(budget::writer& w, const std::string& default_value = "") {
    add_text_picker(w, "Currency", "input_currency", default_value);
}

void add_percent_picker(budget::writer& w, const std::string& title, const std::string& name, size_t default_value = 0) {
    w << R"=====(<div class="form-group">)=====";

    w << "<label for=\"" << name << "\">" << title << "</label>";
    w << "<input required type=\"number\" min=\"0\" max=\"100\" step=\"1\" class=\"form-control\" id=\"" << name << "\" name=\"" << name << "\" ";
    w << " value=\"" << default_value << "\" ";
    w << R"=====(
            >
         </div>
    )=====";
}

void add_money_picker(budget::writer& w, const std::string& title, const std::string& name, const std::string& default_value, bool one_line = false) {
    if (one_line) {
        w << R"=====(<div class="form-group row">)=====";

        w << "<label class=\"col-sm-4 col-form-label\" for=\"" << name << "\">" << title << "</label>";

        w << R"=====(<div class="col-sm-4">)=====";
    } else {
        w << R"=====(<div class="form-group">)=====";

        w << "<label for=\"" << name << "\">" << title << "</label>";
    }

    w << "<input required type=\"number\" step=\"0.01\" class=\"form-control\" id=\"" << name << "\" name=\"" << name << "\" ";

    if (default_value.empty()) {
        w << " placeholder=\"Enter " << title << "\" ";
    } else {
        w << " value=\"" << default_value << "\" ";
    }

    w << ">";

    if (one_line) {
        w << "</div>";
        w << "</div>";
    } else {
        w << "</div>";
    }
}

void add_amount_picker(budget::writer& w, const std::string& default_value = "") {
    add_money_picker(w, "amount", "input_amount", default_value);
}

void add_paid_amount_picker(budget::writer& w, const std::string& default_value = "") {
    add_money_picker(w, "paid amount", "input_paid_amount", default_value);
}

void add_account_picker(budget::writer& w, const std::string& default_value = "") {
    auto today = budget::local_day();

    w << R"=====(
            <div class="form-group">
                <label for="input_account">Account</label>
                <select class="form-control" id="input_account" name="input_account">
    )=====";

    for (auto& account : all_accounts(today.year(), today.month())) {
        if (budget::to_string(account.id) == default_value) {
            w << "<option selected value=\"" << account.id << "\">" << account.name << "</option>";
        } else {
            w << "<option value=\"" << account.id << "\">" << account.name << "</option>";
        }
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_asset_picker(budget::writer& w, const std::string& default_value = "") {
    w << R"=====(
            <div class="form-group">
                <label for="input_asset">Asset</label>
                <select class="form-control" id="input_asset" name="input_asset">
    )=====";

    for (auto& asset : all_assets()) {
        if (budget::to_string(asset.id) == default_value) {
            w << "<option selected value=\"" << asset.id << "\">" << asset.name << "</option>";
        } else {
            w << "<option value=\"" << asset.id << "\">" << asset.name << "</option>";
        }
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_objective_operator_picker(budget::writer& w, const std::string& default_value = "") {
    w << R"=====(
            <div class="form-group">
                <label for="input_operator">Operator</label>
                <select class="form-control" id="input_operator" name="input_operator">
    )=====";

    if ("min" == default_value) {
        w << "<option selected value=\"min\">Min</option>";
    } else {
        w << "<option value=\"min\">Min</option>";
    }

    if ("max" == default_value) {
        w << "<option selected value=\"max\">Max</option>";
    } else {
        w << "<option value=\"max\">Max</option>";
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_objective_type_picker(budget::writer& w, const std::string& default_value = "") {
    w << R"=====(
            <div class="form-group">
                <label for="input_type">Type</label>
                <select class="form-control" id="input_type" name="input_type">
    )=====";

    if ("monthly" == default_value) {
        w << "<option selected value=\"monthly\">Monthly</option>";
    } else {
        w << "<option value=\"monthly\">Monthly</option>";
    }

    if ("yearly" == default_value) {
        w << "<option selected value=\"yearly\">Yearly</option>";
    } else {
        w << "<option value=\"yearly\">Yearly</option>";
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_objective_source_picker(budget::writer& w, const std::string& default_value = "") {
    w << R"=====(
            <div class="form-group">
                <label for="input_source">Source</label>
                <select class="form-control" id="input_source" name="input_source">
    )=====";

    if ("balance" == default_value) {
        w << "<option selected value=\"balance\">Balance</option>";
    } else {
        w << "<option value=\"balance\">Balance</option>";
    }

    if ("earnings" == default_value) {
        w << "<option selected value=\"earnings\">Earnings</option>";
    } else {
        w << "<option value=\"earnings\">Earnings</option>";
    }

    if ("expenses" == default_value) {
        w << "<option selected value=\"expenses\">Expenses</option>";
    } else {
        w << "<option value=\"expenses\">Expenses</option>";
    }

    if ("savings_rate" == default_value) {
        w << "<option selected value=\"savings_rate\">Savings Rate</option>";
    } else {
        w << "<option value=\"savings_rate\">Savings Rate</option>";
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_direction_picker(budget::writer& w, const std::string& default_value = "") {
    w << R"=====(
            <div class="form-group">
                <label for="input_direction">Direction</label>
                <select class="form-control" id="input_direction" name="input_direction">
    )=====";

    if ("to" == default_value) {
        w << "<option selected value=\"to\">To</option>";
    } else {
        w << "<option value=\"to\">To</option>";
    }

    if ("from" == default_value) {
        w << "<option selected value=\"from\">From</option>";
    } else {
        w << "<option value=\"from\">From</option>";
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_importance_picker(budget::writer& w, int importance) {
    w << R"=====(
            <div class="form-group">
                <label for="input_importance">Importance</label>
                <select class="form-control" id="input_importance" name="input_importance">
    )=====";

    if (importance == 1) {
        w << "<option selected value=\"1\">Low</option>";
    } else {
        w << "<option value=\"1\">Low</option>";
    }

    if (importance == 2) {
        w << "<option selected value=\"2\">Medium</option>";
    } else {
        w << "<option value=\"2\">Medium</option>";
    }

    if (importance == 3) {
        w << "<option selected value=\"3\">High</option>";
    } else {
        w << "<option value=\"3\">High</option>";
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_urgency_picker(budget::writer& w, int urgency) {
    w << R"=====(
            <div class="form-group">
                <label for="input_urgency">Urgency</label>
                <select class="form-control" id="input_urgency" name="input_urgency">
    )=====";

    if (urgency == 1) {
        w << "<option selected value=\"1\">Low</option>";
    } else {
        w << "<option value=\"1\">Low</option>";
    }

    if (urgency == 2) {
        w << "<option selected value=\"2\">Medium</option>";
    } else {
        w << "<option value=\"2\">Medium</option>";
    }

    if (urgency == 3) {
        w << "<option selected value=\"3\">High</option>";
    } else {
        w << "<option value=\"3\">High</option>";
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_yes_no_picker(budget::writer& w, const std::string& title, const std::string& name, bool default_value) {
    w << R"=====(<div class="form-group">)=====";

    w << "<label for=\"" << name << "\">" << title << "</label>";

    if (default_value) {
        w << R"=====(<label class="radio-inline"><input type="radio" name=")=====";
        w << name;
        w << R"=====(" value="yes" checked>Yes</label>)=====";
    } else {
        w << R"=====(<label class="radio-inline"><input type="radio" name=")=====";
        w << name;
        w << R"=====(" value="yes">Yes</label>)=====";
    }

    if (!default_value) {
        w << R"=====(<label class="radio-inline"><input type="radio" name=")=====";
        w << name;
        w << R"=====(" value="no" checked>No</label>)=====";
    } else {
        w << R"=====(<label class="radio-inline"><input type="radio" name=")=====";
        w << name;
        w << R"=====(" value="no">No</label>)=====";
    }

    w << R"=====(
                </select>
            </div>
    )=====";
}

void add_paid_picker(budget::writer& w, bool paid) {
    add_yes_no_picker(w, "Paid", "input_paid", paid);
}

void add_portfolio_picker(budget::writer& w, bool portfolio) {
    add_yes_no_picker(w, "Part of the portfolio", "input_portfolio", portfolio);
}

void form_begin(budget::writer& w, const std::string& action, const std::string& back_page) {
    w << R"=====(<form method="POST" action=")=====";
    w << action;
    w << R"=====(">)=====";
    w << R"=====(<input type="hidden" name="server" value="yes">)=====";
    w << R"=====(<input type="hidden" name="back_page" value=")=====";
    w << back_page;
    w << R"=====(">)=====";
}

void form_begin_edit(budget::writer& w, const std::string& action, const std::string& back_page, const std::string& input_id) {
    form_begin(w, action, back_page);

    w << R"=====(<input type="hidden" name="input_id" value=")=====";
    w << input_id;
    w << R"=====(">)=====";
}

void form_end(budget::writer& w, const std::string& button = "") {
    if (button.empty()) {
        w << R"=====(<button type="submit" class="btn btn-primary">Submit</button>)=====";
    } else {
        w << R"=====(<button type="submit" class="btn btn-primary">)=====";
        w << button;
        w << R"=====(</button>)=====";
    }

    w << "</form>";
}

void make_tables_sortable(budget::html_writer& w){
    w.defer_script(R"=====(
        $(".table").DataTable({
         "columnDefs": [ {
          "targets": 'not-sortable',
          "orderable": false,
         }]
        });
    )=====");
}

budget::money monthly_income(budget::month month, budget::year year) {
    std::map<size_t, budget::money> account_sum;

    for (auto& earning : all_earnings()) {
        if (earning.date.year() == year && earning.date.month() == month) {
            account_sum[earning.account] += earning.amount;
        }
    }

    budget::money total = get_base_income();

    for (auto& sum : account_sum) {
        total += sum.second;
    }

    return total;
}

budget::money monthly_spending(budget::month month, budget::year year) {
    std::map<size_t, budget::money> account_sum;

    for (auto& expense : all_expenses()) {
        if (expense.date.year() == year && expense.date.month() == month) {
            account_sum[expense.account] += expense.amount;
        }
    }

    budget::money total;

    for (auto& sum : account_sum) {
        total += sum.second;
    }

    return total;
}

void month_breakdown_income_graph(budget::html_writer& w, const std::string& title, budget::month month, budget::year year, bool mono = false, const std::string& style = "") {
    if (mono) {
        w.defer_script(R"=====(
            breakdown_income_colors = (function () {
                var colors = [], base = Highcharts.getOptions().colors[0], i;
                for (i = 0; i < 10; i += 1) {
                    colors.push(Highcharts.Color(base).brighten((i - 3) / 7).get());
                }
                return colors;
            }());
        )=====");
    }

    auto ss = start_chart_base(w, "pie", "month_breakdown_income_graph", style);

    ss << R"=====(tooltip: { pointFormat: '<b>{point.y} __currency__ ({point.percentage:.1f}%)</b>' },)=====";

    if (mono) {
        ss << R"=====(plotOptions: { pie: { dataLabels: {enabled: false},  colors: breakdown_income_colors, innerSize: '60%' }},)=====";
    }

    ss << "series: [";

    ss << "{ name: 'Income',";
    ss << "colorByPoint: true,";
    ss << "data: [";

    std::map<size_t, budget::money> account_sum;

    for (auto& earning : all_earnings()) {
        if (earning.date.year() == year && earning.date.month() == month) {
            account_sum[earning.account] += earning.amount;
        }
    }

    budget::money total = get_base_income();

    if (!total.zero()) {
        ss << "{";
        ss << "name: 'Salary',";
        ss << "y: " << budget::to_flat_string(total);
        ss << "},";
    }

    for (auto& sum : account_sum) {
        ss << "{";
        ss << "name: '" << get_account(sum.first).name << "',";
        ss << "y: " << budget::to_flat_string(sum.second);
        ss << "},";

        total += sum.second;
    }

    ss << "]},";

    ss << "],";

    if (mono) {
        ss << R"=====(title: {verticalAlign: 'middle', useHTML: true, text: ')=====";

        ss << R"=====(<div class="gauge-cash-flow-title"><strong>)=====";
        ss << title;
        ss << R"=====(</strong><br/><hr class="flat-hr" />)=====";

        ss << R"=====(<span class="text-success">)=====";
        ss << total << " __currency__";
        ss << R"=====(</span></div>)=====";
        ss << R"=====('},)=====";
    } else {
        ss << R"=====(title: {text: ')=====";
        ss << title;
        ss << R"=====('},)=====";
    }

    end_chart(w, ss);
}

void month_breakdown_expenses_graph(budget::html_writer& w, const std::string& title, budget::month month, budget::year year, bool mono = false, const std::string& style = "") {
    if (mono) {
        w.defer_script(R"=====(
            breakdown_expense_colors = (function () {
                var colors = [], base = Highcharts.getOptions().colors[3], i;
                for (i = 0; i < 10; i += 1) {
                    colors.push(Highcharts.Color(base).brighten((i - 3) / 7).get());
                }
                return colors;
            }());
        )=====");
    }

    auto ss = start_chart_base(w, "pie", "month_breakdown_expenses_graph", style);

    ss << R"=====(tooltip: { pointFormat: '<b>{point.y} __currency__ ({point.percentage:.1f}%)</b>' },)=====";

    if (mono) {
        ss << R"=====(plotOptions: {pie: { dataLabels: {enabled: false},  colors: breakdown_expense_colors, innerSize: '60%' }},)=====";
    }

    ss << "series: [";

    ss << "{ name: 'Expenses',";
    ss << "colorByPoint: true,";
    ss << "data: [";

    std::map<size_t, budget::money> account_sum;

    for (auto& expense : all_expenses()) {
        if (expense.date.year() == year && expense.date.month() == month) {
            account_sum[expense.account] += expense.amount;
        }
    }

    budget::money total;

    for (auto& sum : account_sum) {
        ss << "{";
        ss << "name: '" << get_account(sum.first).name << "',";
        ss << "y: " << budget::to_flat_string(sum.second);
        ss << "},";

        total += sum.second;
    }

    ss << "]},";

    ss << "],";

    if (mono) {
        ss << R"=====(title: {verticalAlign: 'middle', useHTML: true, text: ')=====";

        ss << R"=====(<div class="gauge-cash-flow-title"><strong>)=====";
        ss << title;
        ss << R"=====(</strong><br/><hr class="flat-hr" />)=====";

        ss << R"=====(<span class="text-danger">)=====";
        ss << total << " __currency__";
        ss << R"=====(</span></div>)=====";
        ss << R"=====('},)=====";
    } else {
        ss << R"=====(title: {text: ')=====";
        ss << title;
        ss << R"=====('},)=====";
    }

    end_chart(w, ss);
}

void net_worth_graph(budget::html_writer& w, const std::string style = "", bool card = false) {
    // if the user does not use assets, this graph does not make sense
    if(all_assets().empty() || all_asset_values().empty()){
        return;
    }

    if (card) {
        w << R"=====(<div class="card">)=====";

        w << R"=====(<div class="card-header card-header-primary">)=====";
        w << R"=====(<div class="float-left">Net Worth</div>)=====";
        w << R"=====(<div class="float-right">)=====";
        w << get_net_worth() << " __currency__";
        w << R"=====(</div>)=====";
        w << R"=====(<div class="clearfix"></div>)=====";
        w << R"=====(</div>)====="; // card-header

        w << R"=====(<div class="card-body">)=====";
    }

    auto ss = start_chart(w, card ? "" : "Net worth", "area", "net_worth_graph", style);

    ss << R"=====(xAxis: { type: 'datetime', title: { text: 'Date' }},)=====";
    ss << R"=====(yAxis: { min: 0, title: { text: 'Net Worth' }},)=====";
    ss << R"=====(legend: { enabled: false },)=====";

    if (!card) {
        ss << R"=====(subtitle: {)=====";
        ss << "text: '" << get_net_worth() << " __currency__',";
        ss << R"=====(floating:true, align:"right", verticalAlign: "top", style: { fontWeight: "bold", fontSize: "inherit" })=====";
        ss << R"=====(},)=====";
    }

    ss << "series: [";

    ss << "{ name: 'Net Worth',";
    ss << "data: [";

    std::map<size_t, budget::money> asset_amounts;

    auto sorted_asset_values = all_asset_values();

    std::sort(sorted_asset_values.begin(), sorted_asset_values.end(),
              [](const budget::asset_value& a, const budget::asset_value& b) { return a.set_date < b.set_date; });

    auto it  = sorted_asset_values.begin();
    auto end = sorted_asset_values.end();

    while (it != end) {
        auto date = it->set_date;

        while (it->set_date == date) {
            asset_amounts[it->asset_id] = it->amount;

            ++it;
        }

        budget::money sum;

        for (auto& asset : asset_amounts) {
            sum += asset.second;
        }

        ss << "[Date.UTC(" << date.year() << "," << date.month().value - 1 << "," << date.day() << ") ," << budget::to_flat_string(sum) << "],";
    }

    ss << "]},";

    ss << "]";

    end_chart(w, ss);

    if (card) {
        w << R"=====(</div>)====="; //card-body
        w << R"=====(</div>)====="; //card
    }
}

void cash_flow_card(budget::html_writer& w){
    const auto today = budget::local_day();

    const auto m = today.month();
    const auto y = today.year();

    w << R"=====(<div class="card">)=====";

    w << R"=====(<div class="card-header card-header-primary">)=====";
    w << R"=====(<div class="float-left">Cash Flow</div>)=====";
    w << R"=====(<div class="float-right">)=====";
    w << monthly_income(m, y) - monthly_spending(m, y) << " __currency__";
    w << R"=====(</div>)=====";
    w << R"=====(<div class="clearfix"></div>)=====";
    w << R"=====(</div>)====="; // card-header

    w << R"=====(<div class="row card-body">)=====";

    w << R"=====(<div class="col-lg-4 col-md-6 col-sm-12">)=====";
    month_breakdown_income_graph(w, "Income", m, y, true, "min-width:300px; height: 300px;");
    w << R"=====(</div>)====="; //column

    w << R"=====(<div class="col-lg-4 col-md-6 col-sm-12">)=====";
    month_breakdown_expenses_graph(w, "Expenses", m, y, true, "min-width:300px; height: 300px;");
    w << R"=====(</div>)====="; //column

    w << R"=====(</div>)====="; //card-body
    w << R"=====(</div>)====="; //card
}

void objectives_card(budget::html_writer& w){
    auto& objectives = all_objectives();

    // if the user does not use objectives, this card does not make sense
    if(objectives.empty()){
        return;
    }

    const auto today = budget::local_day();

    const auto m = today.month();
    const auto y = today.year();

    //Compute the year/month status
    auto year_status  = budget::compute_year_status();
    auto month_status = budget::compute_month_status(y, m);

    w << R"=====(<div class="card">)=====";
    w << R"=====(<div class="card-header card-header-primary">Objectives</div>)=====";

    w << R"=====(<div class="row card-body">)=====";

    for (size_t i = 0; i < objectives.size(); ++i) {
        auto& objective = objectives[i];

        w << R"=====(<div class="col-lg-2 col-md-3 col-sm-4 col-xs-6">)=====";

        auto ss = start_chart_base(w, "solidgauge", "objective_gauge_" + budget::to_string(i), "height: 200px");

        ss << R"=====(title: {style: {color: "rgb(124, 181, 236)", fontWeight: "bold" }, text: ')=====";
        ss << objective.name;
        ss << R"=====('},)=====";

        ss << R"=====(tooltip: { enabled: false },)=====";
        ss << R"=====(yAxis: { min: 0, max: 100, lineWidth: 0, tickPositions: [], },)=====";

        std::string status;
        std::string success;
        int success_int;

        if (objective.type == "yearly") {
            status      = budget::get_status(year_status, objective);
            success_int = budget::compute_success(year_status, objective);
        } else if (objective.type == "monthly") {
            status      = budget::get_status(month_status, objective);
            success_int = budget::compute_success(month_status, objective);
        } else {
            cpp_unreachable("Invalid objective type");
        }

        ss << R"=====(plotOptions: {)=====";
        ss << R"=====(solidgauge: {)=====";
        ss << R"=====(dataLabels: {)=====";
        ss << R"=====(enabled: true, verticalAlign: "middle", borderWidth: "0px", useHTML: true, )=====";

        ss << R"=====(format: '<div class="gauge-objective-title"><span class="lead""><strong>)=====";
        ss << success_int;
        ss << R"=====(%</strong></span> <br />)=====";
        ss << status;
        ss << R"=====(</div>')=====";

        ss << R"=====(},)=====";
        ss << R"=====(rounded: true)=====";
        ss << R"=====(})=====";
        ss << R"=====(},)=====";

        ss << R"=====(series: [{)=====";
        ss << "name: '" << objective.name << "',";
        ss << R"=====(data: [{)=====";
        ss << R"=====(radius: '112%',)=====";
        ss << R"=====(innerRadius: '88%',)=====";
        ss << "y: " << std::min(success_int, 100);
        ss << R"=====(}])=====";
        ss << R"=====(}])=====";

        end_chart(w, ss);

        w << R"=====(</div>)=====";
    }

    w << R"=====(</div>)=====";
    w << R"=====(</div>)=====";
}

void assets_card(budget::html_writer& w){
    w << R"=====(<div class="card">)=====";

    w << R"=====(<div class="card-header card-header-primary">)=====";
    w << R"=====(<div>Assets</div>)=====";
    w << R"=====(</div>)====="; // card-header

    w << R"=====(<div class="card-body">)=====";

    bool first = true;

    for(auto& asset : all_assets()){
        auto id = asset.id;

        size_t asset_value_id = 0;
        bool asset_value_found = false;

        for (auto& asset_value : all_asset_values()) {
            if (asset_value.asset_id == id) {
                if(!asset_value_found){
                    asset_value_found = true;
                    asset_value_id    = asset_value.id;
                } else if(asset_value.set_date >= get_asset_value(asset_value_id).set_date){
                    asset_value_id    = asset_value.id;
                }
            }
        }

        if (asset_value_found) {
            auto& asset_value = get_asset_value(asset_value_id);
            auto amount       = asset_value.amount;

            if (amount) {
                if(!first){
                    w << R"=====(<hr />)=====";
                }

                w << R"=====(<div class="row">)=====";
                w << R"=====(<div class="col-md-8 col-xl-9 small">)=====";
                w << asset.name;
                w << R"=====(</div>)=====";
                w << R"=====(<div class="col-md-4 col-xl-3 text-right small">)=====";
                w << budget::to_string(amount) << " " << asset.currency;
                w << R"=====(<br />)=====";
                w << budget::to_string(asset_value.set_date);
                w << R"=====(</div>)=====";
                w << R"=====(</div>)=====";

                first = false;
            }
        }
    }

    w << R"=====(</div>)====="; //card-body
    w << R"=====(</div>)====="; //card
}

void index_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "")) {
        return;
    }

    budget::html_writer w(content_stream);

    bool left_column = !all_assets().empty() && !all_asset_values().empty();

    if (left_column) {
        // A. The left column

        w << R"=====(<div class="row">)=====";

        w << R"=====(<div class="col-lg-4 d-none d-lg-block">)====="; // left column

        assets_card(w);

        w << R"=====(</div>)====="; // left column

        // B. The right column

        w << R"=====(<div class="col-lg-8 col-md-12">)====="; // right column
    }

    // 1. Display the net worth graph
    net_worth_graph(w, "min-width: 300px; width: 100%; height: 300px;", true);

    // 2. Cash flow
    cash_flow_card(w);

    // 3. Display the objectives status
    objectives_card(w);

    if (left_column) {
        w << R"=====(</div>)====="; // right column

        w << R"=====(</div>)====="; // row
    }

    // end the page
    page_end(w, content_stream, req, res);
}

void accounts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Accounts")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_accounts(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void all_accounts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "All Accounts")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_all_accounts(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void add_accounts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New account")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New account" << title_end;

    form_begin(w, "/api/accounts/add/", "/accounts/add/");

    add_name_picker(w);
    add_amount_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_accounts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit account")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!account_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The account " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit account " << input_id << title_end;

            form_begin_edit(w, "/api/accounts/edit/", back_page, input_id);

            auto& account = account_get(budget::to_number<size_t>(input_id));

            add_name_picker(w, account.name);
            add_amount_picker(w, budget::to_flat_string(account.amount));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void archive_accounts_month_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Archive accounts from the beginning of the month")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "Archive accounts from the beginning of the month" << title_end;

    form_begin(w, "/api/accounts/archive/month/", "/accounts/");

    w << "<p>This will create new accounts that will be used starting from the beginning of the current month. Are you sure you want to proceed ? </p>";

    form_end(w, "Confirm");

    page_end(w, content_stream, req, res);
}

void archive_accounts_year_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Archive accounts from the beginning of the year")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "Archive accounts from the beginning of the year" << title_end;

    form_begin(w, "/api/accounts/archive/year/", "/accounts/");

    w << "<p>This will create new accounts that will be used starting from the beginning of the current year. Are you sure you want to proceed ? </p>";

    form_end(w, "Confirm");

    page_end(w, content_stream, req, res);
}

void overview_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Overview")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (req.matches.size() == 3) {
        display_month_overview(to_number<size_t>(req.matches[2]), to_number<size_t>(req.matches[1]), w);
    } else {
        display_month_overview(w);
    }

    page_end(w, content_stream, req, res);
}

void overview_aggregate_year_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Overview Aggregate")) {
        return;
    }

    budget::html_writer w(content_stream);

    //Default values
    bool full             = false;
    bool disable_groups   = false;
    std::string separator = "/";

    //Get defaults from config

    if (budget::config_contains("aggregate_full")) {
        if (budget::config_value("aggregate_full") == "true") {
            full = true;
        }
    }

    if (budget::config_contains("aggregate_no_group")) {
        if (budget::config_value("aggregate_no_group") == "true") {
            disable_groups = true;
        }
    }

    if (budget::config_contains("aggregate_separator")) {
        separator = budget::config_value("aggregate_separator");
    }

    if (req.matches.size() == 2) {
        aggregate_year_overview(w, full, disable_groups, separator, to_number<size_t>(req.matches[1]));
    } else {
        auto today = budget::local_day();
        aggregate_year_overview(w, full, disable_groups, separator, today.year());
    }

    page_end(w, content_stream, req, res);
}

void overview_aggregate_month_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Overview Aggregate")) {
        return;
    }

    budget::html_writer w(content_stream);

    //Default values
    bool full             = false;
    bool disable_groups   = false;
    std::string separator = "/";

    //Get defaults from config

    if (budget::config_contains("aggregate_full")) {
        if (budget::config_value("aggregate_full") == "true") {
            full = true;
        }
    }

    if (budget::config_contains("aggregate_no_group")) {
        if (budget::config_value("aggregate_no_group") == "true") {
            disable_groups = true;
        }
    }

    if (budget::config_contains("aggregate_separator")) {
        separator = budget::config_value("aggregate_separator");
    }

    if (req.matches.size() == 3) {
        aggregate_month_overview(w, full, disable_groups, separator, to_number<size_t>(req.matches[2]), to_number<size_t>(req.matches[1]));
    } else {
        auto today = budget::local_day();
        aggregate_month_overview(w, full, disable_groups, separator, today.month(), today.year());
    }

    page_end(w, content_stream, req, res);
}

void overview_year_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Overview Year")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (req.matches.size() == 2) {
        display_year_overview(to_number<size_t>(req.matches[1]), w);
    } else {
        display_year_overview(w);
    }

    page_end(w, content_stream, req, res);
}

void report_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Report")) {
        return;
    }

    budget::html_writer w(content_stream);

    auto today = budget::local_day();
    report(w, today.year(), false, "");

    page_end(w, content_stream, req, res);
}

void expenses_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Expenses")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (req.matches.size() == 3) {
        show_expenses(to_number<size_t>(req.matches[2]), to_number<size_t>(req.matches[1]), w);
    } else {
        show_expenses(w);
    }

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void all_expenses_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "All Expenses")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_all_expenses(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void month_breakdown_expenses_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Expenses Breakdown")) {
        return;
    }

    auto today = budget::local_day();

    auto month = today.month();
    auto year  = today.year();

    if (req.matches.size() == 3) {
        year  = to_number<size_t>(req.matches[1]);
        month = to_number<size_t>(req.matches[2]);
    }

    budget::html_writer w(content_stream);

    w << title_begin << "Expenses Breakdown of " << month << " " << year << budget::year_month_selector{"expenses/breakdown/month", year, month} << title_end;

    month_breakdown_expenses_graph(w, "Expenses Breakdown", month, year);

    page_end(w, content_stream, req, res);
}

void year_breakdown_expenses_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Expenses Breakdown")) {
        return;
    }

    auto today = budget::local_day();

    auto year = today.year();

    if (req.matches.size() == 2) {
        year = to_number<size_t>(req.matches[1]);
    }

    budget::html_writer w(content_stream);

    w << title_begin << "Expenses Breakdown of " << year << budget::year_selector{"expenses/breakdown/year", year} << title_end;

    auto ss = start_chart(w, "Expenses Breakdown", "pie");

    ss << R"=====(tooltip: { pointFormat: '<b>{point.y} __currency__ ({point.percentage:.1f}%)</b>' },)=====";

    ss << "series: [";

    ss << "{ name: 'Expenses',";
    ss << "colorByPoint: true,";
    ss << "data: [";

    std::map<size_t, budget::money> account_sum;

    for (auto& expense : all_expenses()) {
        if (expense.date.year() == year) {
            account_sum[expense.account] += expense.amount;
        }
    }

    for (auto& sum : account_sum) {
        ss << "{";
        ss << "name: '" << get_account(sum.first).name << "',";
        ss << "y: " << budget::to_flat_string(sum.second);
        ss << "},";
    }

    ss << "]},";

    ss << "]";

    end_chart(w, ss);

    page_end(w, content_stream, req, res);
}

void add_expenses_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New Expense")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New Expense" << title_end;

    form_begin(w, "/api/expenses/add/", "/expenses/add/");

    add_date_picker(w);
    add_name_picker(w);
    add_amount_picker(w);
    add_account_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_expenses_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit Expense")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!expense_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The expense " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit Expense " << input_id << title_end;

            form_begin_edit(w, "/api/expenses/edit/", back_page, input_id);

            auto& expense = expense_get(budget::to_number<size_t>(input_id));

            add_date_picker(w, budget::to_string(expense.date));
            add_name_picker(w, expense.name);
            add_amount_picker(w, budget::to_flat_string(expense.amount));
            add_account_picker(w, budget::to_string(expense.account));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void add_earnings_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New earning")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New earning" << title_end;

    form_begin(w, "/api/earnings/add/", "/earnings/add/");

    add_date_picker(w);
    add_name_picker(w);
    add_amount_picker(w);
    add_account_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_earnings_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit earning")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!earning_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The earning " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit earning " << input_id << title_end;

            form_begin_edit(w, "/api/earnings/edit/", back_page, input_id);

            auto& earning = earning_get(budget::to_number<size_t>(input_id));

            add_date_picker(w, budget::to_string(earning.date));
            add_name_picker(w, earning.name);
            add_amount_picker(w, budget::to_flat_string(earning.amount));
            add_account_picker(w, budget::to_string(earning.account));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void earnings_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Earnings")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (req.matches.size() == 3) {
        show_earnings(to_number<size_t>(req.matches[2]), to_number<size_t>(req.matches[1]), w);
    } else {
        show_earnings(w);
    }

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void all_earnings_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "All Earnings")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_all_earnings(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void portfolio_status_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Portfolio")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_asset_portfolio(w);

    page_end(w, content_stream, req, res);
}

void portfolio_currency_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Portfolio Graph")) {
        return;
    }

    std::set<std::string> currencies;

    for (auto& asset : all_assets()) {
        if (asset.name != "DESIRED" && asset.portfolio) {
            currencies.insert(asset.currency);
        }
    }

    budget::html_writer w(content_stream);

    // 1. Display the currency breakdown over time

    auto ss = start_chart(w, "Portfolio by currency", "area", "portfolio_currency_graph");

    ss << R"=====(xAxis: { type: 'datetime', title: { text: 'Date' }},)=====";
    ss << R"=====(yAxis: { min: 0, title: { text: 'Sum' }},)=====";
    ss << R"=====(tooltip: {split: true},)=====";
    ss << R"=====(plotOptions: {area: {stacking: 'percent'}},)=====";

    ss << "series: [";

    auto sorted_asset_values = all_asset_values();

    std::sort(sorted_asset_values.begin(), sorted_asset_values.end(),
              [](const budget::asset_value& a, const budget::asset_value& b) { return a.set_date < b.set_date; });

    for (auto& currency : currencies) {
        ss << "{ name: '" << currency << "',";
        ss << "data: [";

        std::map<size_t, budget::money> asset_amounts;

        auto it  = sorted_asset_values.begin();
        auto end = sorted_asset_values.end();

        while (it != end) {
            auto date = it->set_date;

            while (it->set_date == date) {
                auto& asset = get_asset(it->asset_id);

                if (asset.currency == currency && asset.portfolio) {
                    asset_amounts[it->asset_id] = it->amount;
                }

                ++it;
            }

            budget::money sum;

            for (auto& asset : asset_amounts) {
                sum += asset.second;
            }

            ss << "[Date.UTC(" << date.year() << "," << date.month().value - 1 << "," << date.day() << ") ," << budget::to_flat_string(sum) << "],";
        }

        ss << "]},";
    }

    ss << "]";

    end_chart(w, ss);

    // 2. Display the current currency breakdown

    auto ss2 = start_chart(w, "Current Currency Breakdown", "pie", "currency_breakdown_graph");

    ss2 << R"=====(tooltip: { pointFormat: '<b>{point.y} __currency__ ({point.percentage:.1f}%)</b>' },)=====";

    ss2 << "series: [";

    ss2 << "{ name: 'Currencies',";
    ss2 << "colorByPoint: true,";
    ss2 << "data: [";

    for (auto& currency : currencies) {
        ss2 << "{ name: '" << currency << "',";
        ss2 << "y: ";

        std::map<size_t, budget::money> asset_amounts;

        for (auto& asset_value : sorted_asset_values) {
            auto& asset = get_asset(asset_value.asset_id);

            if (asset.currency == currency && asset.portfolio) {
                asset_amounts[asset_value.asset_id] = asset_value.amount;
            }
        }

        budget::money sum;

        for (auto& asset : asset_amounts) {
            sum += asset.second;
        }

        ss2 << budget::to_flat_string(sum);

        ss2 << "},";
    }

    ss2 << "]},";

    ss2 << "]";

    end_chart(w, ss);

    page_end(w, content_stream, req, res);
}

void portfolio_graph_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Portfolio Graph")) {
        return;
    }

    budget::html_writer w(content_stream);

    auto ss = start_chart(w, "Portfolio", "area");

    ss << R"=====(xAxis: { type: 'datetime', title: { text: 'Date' }},)=====";
    ss << R"=====(yAxis: { min: 0, title: { text: 'Portfolio' }},)=====";

    ss << R"=====(subtitle: {)=====";
    ss << "text: '" << get_portfolio_value() << " __currency__',";
    ss << R"=====(floating:true, align:"right", verticalAlign: "top", style: { fontWeight: "bold", fontSize: "inherit" })=====";
    ss << R"=====(},)=====";

    ss << "series: [";

    ss << "{ name: 'Portfolio',";
    ss << "data: [";

    std::map<size_t, budget::money> asset_amounts;

    auto sorted_asset_values = all_asset_values();

    std::sort(sorted_asset_values.begin(), sorted_asset_values.end(),
              [](const budget::asset_value& a, const budget::asset_value& b) { return a.set_date < b.set_date; });

    auto it  = sorted_asset_values.begin();
    auto end = sorted_asset_values.end();

    while (it != end) {
        auto date = it->set_date;

        while (it->set_date == date) {
            auto& asset = get_asset(it->asset_id);

            if (asset.portfolio) {
                asset_amounts[it->asset_id] = it->amount;
            }

            ++it;
        }

        budget::money sum;

        for (auto& asset : asset_amounts) {
            sum += asset.second;
        }

        ss << "[Date.UTC(" << date.year() << "," << date.month().value - 1 << "," << date.day() << ") ," << budget::to_flat_string(sum) << "],";
    }

    ss << "]},";

    ss << "]";

    end_chart(w, ss);

    page_end(w, content_stream, req, res);
}

void rebalance_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Rebalance")) {
        return;
    }

    // 1. Display the rebalance table

    budget::html_writer w(content_stream);
    budget::show_asset_rebalance(w);

    make_tables_sortable(w);

    w << R"=====(<div class="row">)=====";

    // 2. Display the current allocation

    w << R"=====(<div class="col-lg-6 col-md-12">)=====";

    auto ss = start_chart(w, "Current Allocation", "pie", "current_allocation_graph");

    ss << R"=====(tooltip: { pointFormat: '<b>{point.y} __currency__ ({point.percentage:.1f}%)</b>' },)=====";

    ss << "series: [";

    auto sorted_asset_values = all_asset_values();

    std::sort(sorted_asset_values.begin(), sorted_asset_values.end(),
              [](const budget::asset_value& a, const budget::asset_value& b) { return a.set_date < b.set_date; });

    ss << "{ name: 'Assets',";
    ss << "colorByPoint: true,";
    ss << "data: [";

    std::map<size_t, budget::money> asset_amounts;

    for (auto& asset_value : sorted_asset_values) {
        if (get_asset(asset_value.asset_id).portfolio) {
            asset_amounts[asset_value.asset_id] = asset_value.amount;
        }
    }

    budget::money sum;

    for (auto& asset_amount : asset_amounts) {
        if (!asset_amount.second.zero()) {
            ss << "{ name: '" << get_asset(asset_amount.first).name << "',";
            ss << "y: ";
            ss << budget::to_flat_string(asset_amount.second);
            ss << "},";

            sum += asset_amount.second;
        }
    }

    ss << "]},";

    ss << "]";

    end_chart(w, ss);

    w << R"=====(</div>)=====";

    // 3. Display the desired allocation

    w << R"=====(<div class="col-lg-6 col-md-12">)=====";

    auto ss2 = start_chart(w, "Desired Allocation", "pie", "desired_allocation_graph");

    ss2 << R"=====(tooltip: { pointFormat: '<b>{point.y} __currency__ ({point.percentage:.1f}%)</b>' },)=====";

    ss2 << "series: [";

    ss2 << "{ name: 'Assets',";
    ss2 << "colorByPoint: true,";
    ss2 << "data: [";

    for (auto& asset : all_assets()) {
        if (asset.portfolio && !asset.portfolio_alloc.zero()) {
            ss2 << "{ name: '" << asset.name << "',";
            ss2 << "y: ";
            ss2 << budget::to_flat_string(sum * (static_cast<float>(asset.portfolio_alloc) / 100.0f));
            ss2 << "},";
        }
    }

    ss2 << "]},";

    ss2 << "]";

    end_chart(w, ss2);

    w << R"=====(</div>)=====";

    w << R"=====(</div>)=====";

    page_end(w, content_stream, req, res);
}

void assets_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Assets")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_assets(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void add_assets_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New asset")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New asset" << title_end;

    form_begin(w, "/api/assets/add/", "/assets/add/");

    add_name_picker(w);
    add_money_picker(w, "International Stocks (%)", "input_int_stocks", "");
    add_money_picker(w, "Domestic Stocks (%)", "input_dom_stocks", "");
    add_money_picker(w, "Bonds (%)", "input_bonds", "");
    add_money_picker(w, "Cash (%)", "input_cash", "");
    add_currency_picker(w);
    add_portfolio_picker(w, false);
    add_money_picker(w, "Percent of portfolio (%)", "input_alloc", "");

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_assets_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit asset")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!asset_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The asset " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit asset " << input_id << title_end;

            form_begin_edit(w, "/api/assets/edit/", back_page, input_id);

            auto& asset = asset_get(budget::to_number<size_t>(input_id));

            add_name_picker(w, asset.name);
            add_money_picker(w, "International Stocks (%)", "input_int_stocks", budget::to_flat_string(asset.int_stocks));
            add_money_picker(w, "Domestic Stocks (%)", "input_dom_stocks", budget::to_flat_string(asset.dom_stocks));
            add_money_picker(w, "Bonds (%)", "input_bonds", budget::to_flat_string(asset.bonds));
            add_money_picker(w, "Cash (%)", "input_cash", budget::to_flat_string(asset.cash));
            add_currency_picker(w, asset.currency);
            add_portfolio_picker(w, asset.portfolio);
            add_money_picker(w, "Percent of portfolio (%)", "input_alloc", budget::to_flat_string(asset.portfolio_alloc));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void net_worth_status_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Net Worth Status")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_asset_values(w);

    page_end(w, content_stream, req, res);
}

void net_worth_graph_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Net Worth Graph")) {
        return;
    }

    budget::html_writer w(content_stream);

    net_worth_graph(w);

    page_end(w, content_stream, req, res);
}

void net_worth_currency_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Net Worth Graph")) {
        return;
    }

    std::set<std::string> currencies;

    for (auto& asset : all_assets()) {
        if (asset.name != "DESIRED") {
            currencies.insert(asset.currency);
        }
    }

    budget::html_writer w(content_stream);

    // 1. Display the currency breakdown over time

    auto ss = start_chart(w, "Net worth by currency", "area", "currency_time_graph");

    ss << R"=====(xAxis: { type: 'datetime', title: { text: 'Date' }},)=====";
    ss << R"=====(yAxis: { min: 0, title: { text: 'Net Worth' }},)=====";
    ss << R"=====(tooltip: {split: true},)=====";
    ss << R"=====(plotOptions: {area: {stacking: 'percent'}},)=====";

    ss << "series: [";

    auto sorted_asset_values = all_asset_values();

    std::sort(sorted_asset_values.begin(), sorted_asset_values.end(),
              [](const budget::asset_value& a, const budget::asset_value& b) { return a.set_date < b.set_date; });

    for (auto& currency : currencies) {
        ss << "{ name: '" << currency << "',";
        ss << "data: [";

        std::map<size_t, budget::money> asset_amounts;

        auto it  = sorted_asset_values.begin();
        auto end = sorted_asset_values.end();

        while (it != end) {
            auto date = it->set_date;

            while (it->set_date == date) {
                if (get_asset(it->asset_id).currency == currency) {
                    asset_amounts[it->asset_id] = it->amount;
                }

                ++it;
            }

            budget::money sum;

            for (auto& asset : asset_amounts) {
                sum += asset.second;
            }

            ss << "[Date.UTC(" << date.year() << "," << date.month().value - 1 << "," << date.day() << ") ," << budget::to_flat_string(sum) << "],";
        }

        ss << "]},";
    }

    ss << "]";

    end_chart(w, ss);

    // 2. Display the current currency breakdown

    auto ss2 = start_chart(w, "Current Currency Breakdown", "pie", "currency_breakdown_graph");

    ss2 << R"=====(tooltip: { pointFormat: '<b>{point.y} __currency__ ({point.percentage:.1f}%)</b>' },)=====";

    ss2 << "series: [";

    ss2 << "{ name: 'Currencies',";
    ss2 << "colorByPoint: true,";
    ss2 << "data: [";

    for (auto& currency : currencies) {
        ss2 << "{ name: '" << currency << "',";
        ss2 << "y: ";

        std::map<size_t, budget::money> asset_amounts;

        for (auto& asset_value : sorted_asset_values) {
            if (get_asset(asset_value.asset_id).currency == currency) {
                asset_amounts[asset_value.asset_id] = asset_value.amount;
            }
        }

        budget::money sum;

        for (auto& asset : asset_amounts) {
            sum += asset.second;
        }

        ss2 << budget::to_flat_string(sum);

        ss2 << "},";
    }

    ss2 << "]},";

    ss2 << "]";

    end_chart(w, ss2);

    page_end(w, content_stream, req, res);
}

void list_asset_values_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "List asset values")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::list_asset_values(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void add_asset_values_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New asset value")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New asset value" << title_end;

    form_begin(w, "/api/asset_values/add/", "/asset_values/add/");

    add_asset_picker(w);
    add_amount_picker(w);
    add_date_picker(w, budget::to_string(budget::local_day()));

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_asset_values_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit asset value")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!asset_value_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The asset value " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit asset " << input_id << title_end;

            form_begin_edit(w, "/api/asset_values/edit/", back_page, input_id);

            auto& asset_value = asset_value_get(budget::to_number<size_t>(input_id));

            add_asset_picker(w, budget::to_string(asset_value.asset_id));
            add_amount_picker(w, budget::to_flat_string(asset_value.amount));
            add_date_picker(w, budget::to_string(asset_value.set_date));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void batch_asset_values_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Batch update asset values")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "Batch update asset values" << title_end;

    form_begin(w, "/api/asset_values/batch/", "/asset_values/batch/");

    add_date_picker(w, budget::to_string(budget::local_day()), true);

    auto sorted_asset_values = all_asset_values();

    std::sort(sorted_asset_values.begin(), sorted_asset_values.end(),
              [](const budget::asset_value& a, const budget::asset_value& b) { return a.set_date < b.set_date; });

    for (auto& asset : all_assets()) {
        budget::money amount;
        for (auto& asset_value : sorted_asset_values) {
            if (asset_value.asset_id == asset.id) {
                amount = asset_value.amount;
            }
        }

        add_money_picker(w, asset.name, "input_amount_" + budget::to_string(asset.id), budget::to_flat_string(amount), true);
    }

    form_end(w);

    page_end(w, content_stream, req, res);
}

void list_objectives_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Objectives List")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::list_objectives(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void status_objectives_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Objectives Status")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::status_objectives(w);

    page_end(w, content_stream, req, res);
}

void add_objectives_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New objective")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New objective" << title_end;

    form_begin(w, "/api/objectives/add/", "/objectives/add/");

    add_name_picker(w);
    add_objective_type_picker(w);
    add_objective_source_picker(w);
    add_objective_operator_picker(w);
    add_amount_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_objectives_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit objective")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!objective_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The objective " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit objective " << input_id << title_end;

            form_begin_edit(w, "/api/objectives/edit/", back_page, input_id);

            auto& objective = objective_get(budget::to_number<size_t>(input_id));

            add_name_picker(w, objective.name);
            add_objective_type_picker(w, objective.type);
            add_objective_source_picker(w, objective.source);
            add_objective_operator_picker(w, objective.op);
            add_amount_picker(w, budget::to_flat_string(objective.amount));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void wishes_list_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Objectives List")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::list_wishes(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void wishes_status_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Objectives Status")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::status_wishes(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void wishes_estimate_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Objectives Status")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::estimate_wishes(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void add_wishes_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New Wish")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New Wish" << title_end;

    form_begin(w, "/api/wishes/add/", "/wishes/add/");

    add_name_picker(w);
    add_importance_picker(w, 2);
    add_urgency_picker(w, 2);
    add_amount_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_wishes_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit Wish")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!wish_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The wish " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit wish " << input_id << title_end;

            form_begin_edit(w, "/api/wishes/edit/", back_page, input_id);

            auto& wish = wish_get(budget::to_number<size_t>(input_id));

            add_name_picker(w, wish.name);
            add_importance_picker(w, wish.importance);
            add_urgency_picker(w, wish.urgency);
            add_amount_picker(w, budget::to_flat_string(wish.amount));
            add_paid_picker(w, wish.paid);
            add_paid_amount_picker(w, budget::to_flat_string(wish.paid_amount));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void list_fortunes_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Objectives List")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::list_fortunes(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void graph_fortunes_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Fortune")) {
        return;
    }

    budget::html_writer w(content_stream);

    auto ss = start_chart(w, "Fortune", "spline");

    ss << R"=====(xAxis: { type: 'datetime', title: { text: 'Date' }},)=====";
    ss << R"=====(yAxis: { min: 0, title: { text: 'Fortune' }},)=====";

    ss << "series: [";

    ss << "{ name: 'Fortune',";
    ss << "data: [";

    auto sorted_fortunes = all_fortunes();

    std::sort(sorted_fortunes.begin(), sorted_fortunes.end(),
              [](const budget::fortune& a, const budget::fortune& b) { return a.check_date < b.check_date; });

    for (auto& value : sorted_fortunes) {
        auto& date = value.check_date;

        ss << "[Date.UTC(" << date.year() << "," << date.month().value - 1 << "," << date.day() << ") ," << budget::to_flat_string(value.amount) << "],";
    }

    ss << "]},";

    ss << "]";

    end_chart(w, ss);

    page_end(w, content_stream, req, res);
}

void status_fortunes_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Objectives Status")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::status_fortunes(w, false);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void add_fortunes_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New fortune")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New fortune" << title_end;

    form_begin(w, "/api/fortunes/add/", "/fortunes/add/");

    add_date_picker(w);
    add_amount_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_fortunes_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit fortune")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!fortune_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The fortune " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit fortune " << input_id << title_end;

            form_begin_edit(w, "/api/fortunes/edit/", back_page, input_id);

            auto& fortune = fortune_get(budget::to_number<size_t>(input_id));

            add_date_picker(w, budget::to_string(fortune.check_date));
            add_amount_picker(w, budget::to_flat_string(fortune.amount));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void recurrings_list_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Recurrings List")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::show_recurrings(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void add_recurrings_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New Recurring Expense")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New Recurring Expense" << title_end;

    form_begin(w, "/api/recurrings/add/", "/recurrings/add/");

    add_name_picker(w);
    add_amount_picker(w);
    add_account_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_recurrings_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit Recurring Expense")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!recurring_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The recurring expense " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit Recurring Expense " << input_id << title_end;

            form_begin_edit(w, "/api/recurrings/edit/", back_page, input_id);

            auto& recurring = recurring_get(budget::to_number<size_t>(input_id));

            add_name_picker(w, recurring.name);
            add_amount_picker(w, budget::to_flat_string(recurring.amount));
            add_account_picker(w, budget::to_string(recurring.account));

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

void list_debts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Debts")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::list_debts(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void all_debts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "All Debts")) {
        return;
    }

    budget::html_writer w(content_stream);
    budget::display_all_debts(w);

    make_tables_sortable(w);

    page_end(w, content_stream, req, res);
}

void add_debts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "New Debt")) {
        return;
    }

    budget::html_writer w(content_stream);

    w << title_begin << "New Debt" << title_end;

    form_begin(w, "/api/debts/add/", "/debts/add/");

    add_direction_picker(w);
    add_name_picker(w);
    add_amount_picker(w);
    add_title_picker(w);

    form_end(w);

    page_end(w, content_stream, req, res);
}

void edit_debts_page(const httplib::Request& req, httplib::Response& res) {
    std::stringstream content_stream;
    if (!page_start(req, res, content_stream, "Edit Debt")) {
        return;
    }

    budget::html_writer w(content_stream);

    if (!req.has_param("input_id") || !req.has_param("back_page")) {
        display_error_message(w, "Invalid parameter for the request");
    } else {
        auto input_id = req.params.at("input_id");

        if (!debt_exists(budget::to_number<size_t>(input_id))) {
            display_error_message(w, "The debt " + input_id + " does not exist");
        } else {
            auto back_page = req.params.at("back_page");

            w << title_begin << "Edit Debt " << input_id << title_end;

            form_begin_edit(w, "/api/debts/edit/", back_page, input_id);

            auto& debt = debt_get(budget::to_number<size_t>(input_id));

            add_direction_picker(w, debt.direction ? "to" : "from");
            add_name_picker(w, debt.name);
            add_amount_picker(w, budget::to_flat_string(debt.amount));
            add_title_picker(w, debt.title);
            add_paid_picker(w, debt.state == 1);

            form_end(w);
        }
    }

    page_end(w, content_stream, req, res);
}

} //end of anonymous namespace

void budget::load_pages(httplib::Server& server) {
    // Declare all the pages
    server.get("/", &index_page);

    server.get("/overview/year/", &overview_year_page);
    server.get(R"(/overview/year/(\d+)/)", &overview_year_page);
    server.get("/overview/", &overview_page);
    server.get(R"(/overview/(\d+)/(\d+)/)", &overview_page);
    server.get("/overview/aggregate/year/", &overview_aggregate_year_page);
    server.get(R"(/overview/aggregate/year/(\d+)/)", &overview_aggregate_year_page);
    server.get("/overview/aggregate/month/", &overview_aggregate_month_page);
    server.get(R"(/overview/aggregate/month/(\d+)/(\d+)/)", &overview_aggregate_month_page);

    server.get("/report/", &report_page);

    server.get("/accounts/", &accounts_page);
    server.get("/accounts/all/", &all_accounts_page);
    server.get("/accounts/add/", &add_accounts_page);
    server.post("/accounts/edit/", &edit_accounts_page);
    server.get("/accounts/archive/month/", &archive_accounts_month_page);
    server.get("/accounts/archive/year/", &archive_accounts_year_page);

    server.get(R"(/expenses/(\d+)/(\d+)/)", &expenses_page);
    server.get("/expenses/", &expenses_page);

    server.get(R"(/expenses/breakdown/month/(\d+)/(\d+)/)", &month_breakdown_expenses_page);
    server.get("/expenses/breakdown/month/", &month_breakdown_expenses_page);

    server.get(R"(/expenses/breakdown/year/(\d+)/)", &year_breakdown_expenses_page);
    server.get("/expenses/breakdown/year/", &year_breakdown_expenses_page);

    server.get("/expenses/all/", &all_expenses_page);
    server.get("/expenses/add/", &add_expenses_page);
    server.post("/expenses/edit/", &edit_expenses_page);

    server.get(R"(/earnings/(\d+)/(\d+)/)", &earnings_page);
    server.get("/earnings/", &earnings_page);
    server.get("/earnings/all/", &all_earnings_page);
    server.get("/earnings/add/", &add_earnings_page);
    server.post("/earnings/edit/", &edit_earnings_page);

    server.get("/portfolio/status/", &portfolio_status_page);
    server.get("/portfolio/graph/", &portfolio_graph_page);
    server.get("/portfolio/currency/", &portfolio_currency_page);
    server.get("/rebalance/", &rebalance_page);
    server.get("/assets/", &assets_page);
    server.get("/net_worth/status/", &net_worth_status_page);
    server.get("/net_worth/graph/", &net_worth_graph_page);
    server.get("/net_worth/currency/", &net_worth_currency_page);
    server.get("/assets/add/", &add_assets_page);
    server.post("/assets/edit/", &edit_assets_page);

    server.get("/asset_values/list/", &list_asset_values_page);
    server.get("/asset_values/add/", &add_asset_values_page);
    server.get("/asset_values/batch/", &batch_asset_values_page);
    server.post("/asset_values/edit/", &edit_asset_values_page);

    server.get("/objectives/list/", &list_objectives_page);
    server.get("/objectives/status/", &status_objectives_page);
    server.get("/objectives/add/", &add_objectives_page);
    server.post("/objectives/edit/", &edit_objectives_page);

    server.get("/wishes/list/", &wishes_list_page);
    server.get("/wishes/status/", &wishes_status_page);
    server.get("/wishes/estimate/", &wishes_estimate_page);
    server.get("/wishes/add/", &add_wishes_page);
    server.post("/wishes/edit/", &edit_wishes_page);

    server.get("/recurrings/list/", &recurrings_list_page);
    server.get("/recurrings/add/", &add_recurrings_page);
    server.post("/recurrings/edit/", &edit_recurrings_page);

    server.get("/debts/list/", &list_debts_page);
    server.get("/debts/all/", &all_debts_page);
    server.get("/debts/add/", &add_debts_page);
    server.post("/debts/edit/", &edit_debts_page);

    server.get("/fortunes/graph/", &graph_fortunes_page);
    server.get("/fortunes/status/", &status_fortunes_page);
    server.get("/fortunes/list/", &list_fortunes_page);
    server.get("/fortunes/add/", &add_fortunes_page);
    server.post("/fortunes/edit/", &edit_fortunes_page);

    // Handle error

    server.set_error_handler([](const auto& req, auto& res) {
        std::stringstream content_stream;
        content_stream.imbue(std::locale("C"));

        if (res.status == 401 || res.status == 403) {
            content_stream << header("", false);
        } else {
            content_stream << header("", true);
        }

        content_stream << "<p>Error Status: <span class='text-danger'>";
        content_stream << res.status;
        content_stream << "</span></p>";

        content_stream << "<p>On Page: <span class='text-success'>";
        content_stream << req.path;
        content_stream << "</span></p>";

        content_stream << footer();

        res.set_content(content_stream.str(), "text/html");
    });
}
