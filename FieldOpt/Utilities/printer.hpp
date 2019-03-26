/*! This file contains helper functions for "prettyfied" printing
 *  to console.
 */
#ifndef PRINTER_FUNCTIONS_H
#define PRINTER_FUNCTIONS_H

#include <string>
#include <vector>
#include <iomanip>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <Utilities/colors.hpp>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace Printer {

/*!
 * @brief Convert a number to its string representation.
 * @param num Number to convert.
 * @return String representation of num.
 */
template<typename T>
inline std::string num2str(const T num) {
    return boost::lexical_cast<std::string>(num);
}


/*!
 * @brief Truncate a string, adding an ellipsis to the end.
 * @param text Text to truncate.
 * @param width Width to to truncate to (including the added ellipsis).
 */
inline void truncate_text(std::string &text, const int &width=66) {
    if (text.size() > width) {
        text = text.substr(0, width - 3) + "...";
    }
}

/*!
 * @brief Pad a string, adding spaces at the _end_ to make it the required length.
 * @param text Text to pad.
 * @param width Width to pad to.
 */
inline void pad_text(std::string &text, const int &width=66) {
    if (text.size() < width) {
        text.insert(text.size(), width - text.size(), ' ');
    }
}

/*!
 * @brief Split a string into multiple lines of the required width, padding each with spaces at the end.
 * Note that the | character in the input string will insert a linebreak.
 * @param text Text to split.
 * @param width Width to split at (and pad to).
 * @return
 */
inline std::vector<std::string> split_line(const std::string text, const int &width=67) {
    std::vector<std::string> strv;
    if (text.size() > width) {
        std::size_t start_idx = 0;
        std::size_t end_idx = 1;
        int line_nr = 0;
        std::string remainder = text;
        while (remainder.size() > width) {
            if (remainder.find_first_of("|") < width) {
                end_idx = remainder.find_first_of("|");
            }
            else {
                end_idx = remainder.find_last_of(".,;/ ", width);
            }
            std::string line = remainder.substr(start_idx, end_idx);
            pad_text(line, width);
            strv.push_back(line);
            remainder = remainder.substr(end_idx+1, remainder.size());
            if (end_idx >= width) {
                break;
            }
        }
        pad_text(remainder, width);
        strv.push_back(remainder);
    }
    else {
        std::string padded = text;
        pad_text(padded, width);
        strv.push_back(padded);
    }
    return strv;
}


/*! @brief Print a compact infobox.

 Example:
 ┌─────────────────────────────────────────────────────────────────────┐
 │ This is a compact info box                                         │
 └─────────────────────────────────────────────────────────────────────┘
 */
inline void info(const std::string &text) {
    std::stringstream ss;
    ss << FLGREEN;
    std::string content = text;
    truncate_text(content);
    pad_text(content);
    ss << "┌─────────────────────────────────────────────────────────────────────┐" << "\n";
    ss << "│ " << content <<                                                  " │" << "\n";
    ss << "└─────────────────────────────────────────────────────────────────────┘" << "\n";
    ss << "\n";
    ss << AEND;
    std::cout << ss.str();
}

/* Extended info box.
Example:
 ┌───────┬──────────────────────┬──────────────────────────────────────┐
 │ INFO │ Module: Optimization │ Class: Optimizer                     │
 ├───────┴──────────────────────┴──────────────────────────────────────┤
 │ This box can contain more information and a significantly larger    │
 │ amount of details.                                                  │
 └─────────────────────────────────────────────────────────────────────┘
 */
inline void ext_info(const std::string &text,
                     const std::string &modulen="",
                     const std::string &classn="") {
    std::string module_name = modulen;
    std::string class_name = classn;
    truncate_text(module_name, 12);
    truncate_text(class_name, 29);
    pad_text(module_name, 12);
    pad_text(class_name, 29);
    auto lines = split_line(text);
    std::stringstream ss;
    ss << FLGREEN;
    ss << "┌───────┬──────────────────────┬──────────────────────────────────────┐" << "\n";
    ss << "│ INFO │ Module: " << module_name << " │ Class: " << class_name << " │" << "\n";
    ss << "├───────┴──────────────────────┴──────────────────────────────────────┤" << "\n";
    for (auto line : lines) {
        ss << "│ " << line << " │" << "\n";
    }
    ss << "└─────────────────────────────────────────────────────────────────────┘" << "\n";
    ss << "\n";
    ss << AEND;
    std::cout << ss.str();
}

/* Extended warning box.
Example:
 ┏━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
 ┃ WARNING  ┃ Module: Optimization ┃ Class: GeneticAlgorit...         ┃
 ┣━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
 ┃ This is a warning box.                                              ┃
 ┃ This is a warning box.                                              ┃
 ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
 */
inline void ext_warn(const std::string &text,
                     const std::string &modulen="",
                     const std::string &classn="") {
    std::string module_name = modulen;
    std::string class_name = classn;
    truncate_text(module_name, 12);
    truncate_text(class_name, 25);
    pad_text(module_name, 12);
    pad_text(class_name, 25);
    auto lines = split_line(text);
    std::stringstream ss;
    ss << FLYELLOW;
    ss << "┏━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓" << "\n";
    ss << "┃ WARNING  ┃ Module: " << module_name << " ┃ Class: " << class_name << " ┃" << "\n";
    ss << "┣━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫" << "\n";
    for (auto line : lines) {
        ss << "┃ " << line << " ┃" << "\n";
    }
    ss << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛" << "\n";
    ss << "\n";
    ss << AEND;
    std::cout << ss.str();
}
/* Error box.
Example:
 ╔═════════════════════════════════════════════════════════════════════╗
 ║ ERROR: This is an error message.                                   ║
 ╚═════════════════════════════════════════════════════════════════════╝
 */
inline void error(const std::string &text) {
    std::stringstream ss;
    ss << FLRED;
    std::string content = text;
    pad_text(content, 59);
    ss << "╔═════════════════════════════════════════════════════════════════════╗" << "\n";
    ss << "║ ERROR: " << content <<                                                  " ║" << "\n";
    ss << "╚═════════════════════════════════════════════════════════════════════╝" << "\n";
    ss << "\n";
    ss << AEND;
    std::cout << ss.str();
}

}


#endif // PRINTER_FUNCTIONS_H

