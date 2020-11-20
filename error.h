/**********************************
* File:     error.h
*
* Author:   caipeng
*
* Email:    iiicp@outlook.com
*
* Date:     2020/11/19
***********************************/

#ifndef CPTOYC_ERROR_H
#define CPTOYC_ERROR_H
#include <string>
namespace CPToyC {
    namespace Compiler {
        class Error {
        public:
            std::string Filename;
            std::string Msg;
            int Row;
            int Col;

            Error() {
                Row = -1;
                Col = -1;
            }

            Error(const std::string &filename, const std::string &msg, int row = -1, int col = -1) {
                Filename = filename;
                Msg = msg;
                Row = row;
                Col = col;
            }
        };
    }
}

#endif //CPTOYC_ERROR_H
