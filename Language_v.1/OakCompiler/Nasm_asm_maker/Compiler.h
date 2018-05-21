#ifndef COMPILER_H
#define COMPILER_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unordered_map>

#include "Parser/Parser.h"
#include "stringer/stringer.h"

/*=========================================================================================================
 *
 *
 *                              Compiler
 *                              Code Generator from my language to Asm x86 (Intel noprefix)
 *
 *
 *
 *  made by Nikita Dubovik
 /==========================================================================================================*/
struct Asm_Compiler;

namespace asm_oak
{
    const int ASM_BUFFER_SIZE = 16384;

    const int LOCAL_BUFFER_SIZE = 8;

    const char* STDLIB = "oaklib.asm";


    struct Asm_Buffer
    {
        char* buffer_ = nullptr;

        size_t  buffer_pos_ = 0;

        char* local_buffer_ = nullptr;


        /*-------------------------------------------*/

        Asm_Buffer() noexcept;

        ~Asm_Buffer() noexcept;

        void add_string(const char* str);

        void add_data(const char* data);

        void add_number(int numb);

        void make_asm_file();

        void clean_local_buffer();
    };


    struct Function_Handler
    {
        Asm_Compiler* boss_ptr_ = nullptr;

        std::unordered_map<char*, int> var_table_{};

        Command* current_command_ = nullptr;

        int current_indent_ = 8;

        /*-------------------------------------------*/

        Function_Handler() = default;

        explicit Function_Handler(Asm_Compiler* boss) noexcept ;

        void reset_boss(Asm_Compiler* boss);

        ~Function_Handler() noexcept ;

        void handle_function(Function* function);

        void handle_command();

        void handle_var_def(Command* command);

        void handle_node(Node* node);

        void handle_variable_assignment(Node* node);  // dig up the variable pointer

        void handle_if(Node* node);

        void handle_while(Node* node);

        void handle_operation(Node* node);

        void handle_func_call(Node* node);

        void handle_procedure_call(Node* node);

        void handle_variable_usage(Node* node);
    };

}




/*=========================================================================================================
 *
 *  struct Asm_Compiler
 *  Compiler to ".intel_syntax noprefix" ASM
 *
 *
 *
 /==========================================================================================================*/
struct Asm_Compiler
{
    Parser parser_{};

    asm_oak::Function_Handler foreman_;

    asm_oak::Asm_Buffer asm_buffer_;

    Function* func_list_ = nullptr;

    size_t if_amount_ = 0;

    size_t while_amount_ = 0;

    /*-------------------------------------------*/

    Asm_Compiler() = default;

    ~Asm_Compiler() noexcept ;

    void compile(const char* filename);

    void handle_func_list();

    void add_std_functions();
};

#include "Compiler.cpp"

#endif //COMPILER_H