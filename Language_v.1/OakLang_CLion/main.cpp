#include <iostream>
#include "Bin_maker/Binarier.h"
#include "Nasm_asm_maker/Compiler.h"
#include "Parser/acorn.h"
int main()
{
    try{
        Bin_Compiler compiler{};

        compiler.compile("proga.oak");

        Asm_Compiler compiler1{};

        compiler1.compile("proga.oak");
    }
    catch(Acorn* acorn)
    {
        acorn->tell_user();
    }
    return 0;
}