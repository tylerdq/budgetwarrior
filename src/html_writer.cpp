//=======================================================================
// Copyright (c) 2013-2017 Baptiste Wicht.
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#include "cpp_utils/assert.hpp"
#include "cpp_utils/string.hpp"

#include "writer.hpp"
#include "console.hpp"

namespace {

std::string success_to_string(int success) {
    success = std::min(success, 100);
    success = std::max(success, 0);

    std::stringstream ss;

    ss << R"=====(<div class="progress">)=====";
    ss << R"=====(<div class="progress-bar" role="progressbar" style="width:)=====" << success << R"=====(%;" aria-valuenow=")=====" << success << R"=====(" aria-valuemin="0" aria-valuemax="100">)=====" << success << R"=====(%</div>)=====";
    ss << R"=====(</div>)=====";

    return ss.str();
}

std::string edit_to_string(const std::string& module, const std::string& id){
    std::stringstream ss;

    // Add the delete button
    ss << R"=====(<form class="small-form-inline" method="POST" action="/api/)=====";
    ss << module;
    ss << R"=====(/delete/">)=====";
    ss << R"=====(<input type="hidden" name="server" value="yes">)=====";
    ss << R"=====(<input type="hidden" name="back_page" value="__budget_this_page__">)=====";
    ss << R"=====(<input type="hidden" name="input_id" value=")=====";
    ss << id;
    ss << R"=====(">)=====";
    ss << R"=====(<button type="submit" class="btn btn-sm btn-danger">Delete</button>)=====";
    ss << R"=====(</form>)=====";

    // Add the edit button
    ss << R"=====(<form class="small-form-inline" method="POST" action="/)=====";
    ss << module;
    ss << R"=====(/edit/">)=====";
    ss << R"=====(<input type="hidden" name="server" value="yes">)=====";
    ss << R"=====(<input type="hidden" name="back_page" value="__budget_this_page__">)=====";
    ss << R"=====(<input type="hidden" name="input_id" value=")=====";
    ss << id;
    ss << R"=====(">)=====";
    ss << R"=====(<button type="submit" class="btn btn-sm btn-warning">Edit</button>)=====";
    ss << R"=====(</form>)=====";

    return ss.str();
}

std::string html_format(const std::string& v){
    if(v.substr(0, 5) == "::red"){
        auto value = v.substr(5);

        return "<span style=\"color:red;\">" + value + "</span>";
    } else if(v.substr(0, 6) == "::blue"){
        auto value = v.substr(6);

        return "<span style=\"color:blue;\">" + value + "</span>";
    } else if(v.substr(0, 7) == "::green"){
        auto value = v.substr(7);

        return "<span style=\"color:green;\">" + value + "</span>";
    } else if(v.substr(0, 9) == "::success"){
        auto value = v.substr(9);
        auto success = budget::to_number<unsigned long>(value);
        return success_to_string(success);
    } else if(v.substr(0, 8) == "::edit::"){
        auto value = v.substr(8);

        if(value.find("::") == std::string::npos){
            return v;
        }

        auto module = value.substr(0, value.find("::"));
        auto id     = value.substr(value.find("::") + 2, value.size());

        return edit_to_string(module, id);
    }

    return v;
}

} // end of anonymous namespace

budget::html_writer::html_writer(std::ostream& os) : os(os) {}

budget::writer& budget::html_writer::operator<<(const std::string& value){
    os << html_format(value);

    return *this;
}

budget::writer& budget::html_writer::operator<<(const double& value){
    os << value;

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::money& m) {
    os << m;

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::month& m) {
    os << m.as_short_string();

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::year& y) {
    os << y.value;

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::end_of_line_t&) {
    os << "\n";
    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::p_begin_t&) {
    os << "<p>";

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::p_end_t&) {
    os << "</p>";

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::title_begin_t&) {
    os << "<h1>";

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::title_end_t&) {
    os << "</h1>";

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::year_month_selector& m) {
    os << "<div class=\"selector\">";

    auto previous_month = m.current_month;
    auto previous_year = m.current_year;
    auto next_month = m.current_month;
    auto next_year = m.current_year;

    if (m.current_month == 1) {
        previous_month = 12;
        previous_year  = m.current_year - 1;
    } else {
        previous_month = m.current_month - 1;
        previous_year  = m.current_year;
    }

    if (m.current_month == 12) {
        next_month = 1;
        next_year  = m.current_year + 1;
    } else {
        next_month = m.current_month + 1;
        next_year  = m.current_year;
    }

    os << "<a href=\"/" << m.page << "/" << previous_year << "/" << previous_month.value << "/\">&lt;&lt;</a>";
    os << "&nbsp;";
    os << "<a href=\"/" << m.page << "/" << next_year << "/" << next_month.value << "/\">&gt;&gt;</a>";

    os << "</div>";

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::year_selector& m) {
    os << "<div class=\"selector\">";

    auto previous_year = m.current_year - 1;
    auto next_year     = m.current_year + 1;

    os << "<a href=\"/" << m.page << "/" << previous_year << "/\">&lt;&lt;</a>";
    os << "&nbsp;";
    os << "<a href=\"/" << m.page << "/" << next_year << "/\">&gt;&gt;</a>";

    os << "</div>";

    return *this;
}

budget::writer& budget::html_writer::operator<<(const budget::add_button& b) {
    os << "<a href=\"/" << b.module << "/add/\" class=\"btn btn-info\" role=\"button\">New</a>\n";

    return *this;
}

void budget::html_writer::display_table(std::vector<std::string>& columns, std::vector<std::vector<std::string>>& contents, size_t groups, std::vector<size_t> lines, size_t left){
    cpp_assert(groups > 0, "There must be at least 1 group");
    cpp_unused(left);
    cpp_unused(lines);

    for (auto& row : contents) {
        if (row.size() < columns.size()) {
            std::cerr << "Invalid number of columns in row" << std::endl;
            return;
        }

        for (auto& cell : row) {
            cpp::trim(cell);
        }
    }

    size_t extend = columns.size();

    for (size_t i = 0; i < columns.size(); ++i) {
        for (auto& row : contents) {
            if (row[i].size() >= 9 && row[i].substr(0, 9) == "::success") {
                extend = i;
                break;
            }
        }

        if (extend == i) {
            break;
        }
    }

    bool small = columns.empty(); // TODO Improve this heuristic!

    if(small){
        os << "<div class=\"row\">";
        os << "<div class=\"col-md-4\">&nbsp;</div>";
        os << "<div class=\"col-md-4\">";
    }

    os << "<table class=\"table table-sm small-text\">";

    // Display the header

    if (columns.size()) {
        os << "<thead>";
        os << "<tr>";

        for (size_t i = 0; i < columns.size(); ++i) {
            auto& column = columns[i];

            std::string style;

            // TODO: This is only a bad hack, at best
            if(i == extend){
                style = " class=\"extend-only\"";
            }

            if (groups > 1) {
                os << "<th colspan=\"" << groups << "\"" << style << ">" << column << "</th>";
            } else {
                os << "<th" << style << ">" << column << "</th>";
            }
        }

        os << "</tr>";
        os << "</thead>";
    }

    // Display the contents

    os << "<tbody>";

    for(size_t i = 0; i < contents.size(); ++i){
        auto& row = contents[i];

        os << "<tr>";

        for(size_t j = 0; j < row.size(); ++j){
            std::string value = html_format(row[j]);

            if(value.empty()){
                os << "<td>&nbsp;</td>";
            } else {
                if(columns.empty() && j == 0){
                    os << "<th scope=\"row\">" << value << "</th>";
                } else {
                    os << "<td>" << value << "</td>";
                }
            }
        }

        os << "</tr>";
    }

    os << "</tbody>";

    os << "</table>";

    if(small){
        os << "</div>"; // middle column
        os << "<div class=\"col-md-4\">&nbsp;</div>";
        os << "</div>"; // row
    }
}

bool budget::html_writer::is_web() {
    return true;
}

void budget::html_writer::display_graph(const std::string& title, std::vector<std::string>& categories, std::vector<std::string> series_names, std::vector<std::vector<float>>& series_values){
    os << R"=====(<div id="container" style="min-width: 310px; height: 400px; margin: 0 auto"></div>)=====";

    os << R"=====(<script langage="javascript">)=====";

    os << R"=====(Highcharts.chart('container', {)=====";
    os << R"=====(chart: {type: 'column'},)=====";
    os << R"=====(credits: {enabled: true},)=====";

    os << "title: { text: '" << title << "'},";
    os << "xAxis: { categories: [";

    for (auto& category : categories) {
        os << "'" << category << "',";
    }

    os << "]},";

    os << "series: [";

    for(size_t i = 0; i < series_names.size(); ++i){
        os << "{ name: '" << series_names[i] << "',";
        os << "data: [";

        for(auto& value : series_values[i]){
            os << value << ",";
        }

        os << "]},";
    }

    os << "]";

    os << R"=====(});)=====";

    os << R"=====(</script>)=====";
}
