/***********************************
* File:     Process.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/12/6
***********************************/

#ifndef CPTOYC_PROCESS_H
#define CPTOYC_PROCESS_H

namespace CPToyC {
    namespace Compiler {
        class Process {
        public:
            static bool ColorNeedsFlush();
            static const char *ResetColor();
            static const char *OutputBold(bool bg);
            static const char *OutputColor(char code, bool bold, bool bg);
        };
    }
}


#endif //CPTOYC_PROCESS_H
