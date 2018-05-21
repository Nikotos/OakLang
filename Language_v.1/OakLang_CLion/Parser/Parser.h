#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <cstring>

/*=========================================================================================================
 *
 *
 *                                          Parser
 *
 *
 *
 /==========================================================================================================*/

#include "acorn.h"
#include "Lexic.h"

using namespace std;



/* ---------------------------------------------------------------------------------
 *
 *          struct Parser
 *
 *
 /-------------------------------------------------------------------------------------*/
struct Parser
{
    const char* string_ = nullptr;
    int pos_ = -1;              /* Position in string*/


    char* heap_ = nullptr;  /* heap to put symbols from file*/


    oak::string* text_ = nullptr;    /* array of strings from input file*/
    int strings_amount_ = 0;
    int text_pos_ = 0;


    Var_list* var_list_;        /* Variables list, to "catch" undefined variables*/

    Var_list* func_list_;       /* Functions list to "catch undefined functions */

    Function* current_function_ = nullptr;


    /*-------------------------------Functions-------------------------------*/

    Parser();

    ~Parser();

    void Read_file(const char* filename);

    size_t sizeof_file(FILE* file);

    size_t str_amount(const char* heap);

    void split_text();

    Function* Get_program();

    Function* Get_function();

    Command* Get_action();

    Command* Get_command();

    void SetVar(enum VAR_TYPES type);

    char* Get_func_name(); // Get function name

    int Get_func_decl(); // Get parameters declaration (for function)

    Node* GetC(); // Get Condition

    Node* GetA(); // Get Assignment Operation

    Node* GetE(); // Get Expression

    Node* GetT(); // Get * or / expression

    Node* GetP(); // Get expression in brackets

    Node* GetN(); // Get Number

    int Get_func_call(Node* function); // Get function call

    Node* GetV(); // Get Variable

};



#include "Parser.cpp"

#endif //PARSER_H