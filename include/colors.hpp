#pragma once

#include <ostream>
namespace TermColor {
    enum Code {
        FG_RED      = 31,
        FG_GREEN    = 32,
        FG_BLUE     = 34,
        FG_MAGENTA  = 35,
        FG_DEFAULT  = 39,
        FG_CYAN     = 36,
        FG_YELLOW   = 33,
        BG_RED      = 41,
        BG_GREEN    = 42,
        BG_BLUE     = 44,
        BG_DEFAULT  = 49,
        BG_CYAN     = 46,
        BG_YELLOW   = 43,
        BOLD        = 1,
        UNBOLD      = 0,
    };
    class Modifier {
        Code code;
    public:
        Modifier(Code pCode) : code(pCode) {}
        friend std::ostream&
        operator<<(std::ostream& os, const Modifier& mod) {
            return os << "\033[" << mod.code << "m";
        }
    };
}
