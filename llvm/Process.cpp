/***********************************
* File:     Process.cpp
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/6
***********************************/

#include "Process.h"

using namespace CPToyC::Compiler;

#define COLOR(FGBG, CODE, BOLD) "\033[0;" BOLD FGBG CODE "m"

#define ALLCOLORS(FGBG,BOLD) {\
    COLOR(FGBG, "0", BOLD),\
    COLOR(FGBG, "1", BOLD),\
    COLOR(FGBG, "2", BOLD),\
    COLOR(FGBG, "3", BOLD),\
    COLOR(FGBG, "4", BOLD),\
    COLOR(FGBG, "5", BOLD),\
    COLOR(FGBG, "6", BOLD),\
    COLOR(FGBG, "7", BOLD)\
  }

static const char* colorcodes[2][2][8] = {
        { ALLCOLORS("3",""), ALLCOLORS("3","1;") },
        { ALLCOLORS("4",""), ALLCOLORS("4","1;") }
};

bool Process::ColorNeedsFlush() {
    // No, we use ANSI escape sequences.
    return false;
}

const char *Process::ResetColor() {
    return "\033[0m";
}

const char *Process::OutputBold(bool bg) {
    return "\033[1m";
}

const char *Process::OutputColor(char code, bool bold, bool bg) {
    return colorcodes[bg?1:0][bold?1:0][code&7];
}
