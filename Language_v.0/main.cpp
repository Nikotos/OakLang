#include <iostream>
#include"Compiler.h"
#include "Parser/acorn.h"
int main()
{
    try{

        Asm_Compiler compiler{};


        compiler.compile("proga.oak");
    }
    catch(Acorn* acorn)
    {
        acorn->tell_user();
    }
    return 0;
}