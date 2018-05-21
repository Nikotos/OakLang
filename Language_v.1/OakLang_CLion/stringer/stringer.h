#ifndef STRINGER_H
#define STRINGER_H

#include <iostream>
#include <cstring>

#include "acorn.h"

namespace oak
{
    struct String
    {
        char* body_ = nullptr;
        int len_ = 0;
    };

    struct Stringer
    {
        /* heap to put symbols from the file*/
        char* heap_ = nullptr;

        /* array of strings from input file*/
        oak::String* text_ = nullptr;

        int strings_amount_ = 0;

        int symbols_amount_ = 0;


        Stringer(){}

        ~Stringer();

        void Read_file(const char* filename);

        size_t sizeof_file(FILE* file);

        size_t strings_amount(const char* heap);

        void split_text();
    };
}



#include "stringer.hpp"

#endif //STRINGER_H