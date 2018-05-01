#include <iostream>
#include"Compiler.h"
#include "Parser/acorn.h"
int main()
{
    try{

        Asm_Compiler compiler{};


        compiler.compile("Examples/factorial.oak");
    }
    catch(Acorn* acorn)
    {
        acorn->tell_user();
    }
    return 0;
}
