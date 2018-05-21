#ifndef BINARIER_H
#define BINARIER_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unordered_map>

#include "Parser/Parser.h"
#include "stringer/stringer.h"

/*=========================================================================================================
 *
 *
 *                                      OAK-COMPILER
 *
 *                  Code Generator from my language.oak to .elf-64-86 executable files
 *
 *
 *
 *  made by Nikita Dubovik
 /==========================================================================================================*/

/*-----------------------------------------------------------------------------
 * @about
 *          useful Macro to fill your code
 *          with debugger interruptions
 *
 /----------------------------------------------------------------------------*/
#define INT_3                                   \
    if (GDB_MODE)                               \
    {                                           \
        ADD_DB(0xCC);  /*debug interruption */  \
    }


struct Bin_Compiler;

namespace bin_oak
{
    /*-----------------------------------------------------------------------------
     * @about
     *          some essential configuration constants
     *          with debugger interruptions
     *
     /----------------------------------------------------------------------------*/

    const char* STDLIB = "oaklib.asm";

    const int LABEL_NAME_SIZE = 32;

    const int TASK_LIST_SIZE = 1024;

    const int STACK_VARIABLES_INDENT = 256;

    const char* BASE_FILE_NAME = "base";

    const char* OUTPUT_FILE_NAME = "test";

    const char* MAIN_NAME = "main";

    const char* INPUT_FUNC_NAME = "input";

    const char* OUTPUT_FUNC_NAME = "output";

    const int JUMP_COORD_SIZE = 4;

    const bool GDB_MODE = false;



    /*-----------------------------------------------------------------------------
     * @about
     *          Labels Manager
     *          builds up connections between jumps/calls and labels
     *
     /----------------------------------------------------------------------------*/
    struct Task
    {
        char* label_name_ = nullptr;

        size_t pos_  = 0;

        /*-------------------------------------------*/

        Task() = default;

        ~Task();

        void set_label(const char* name, size_t pos);

    };

    struct Task_Manager
    {

        Task* task_list_ = nullptr;     // for tasks

        size_t current_pos_ = 0;


        Task* labels_list_ = nullptr;   // data with coordinates of labels

        size_t labels_list_pos_ = 0;

        /*-------------------------------------------*/

        Task_Manager();

        ~Task_Manager();

        void add_task(const char* name, int pos);

        void add_label(const char* name, int pos);

        int get_label_pos(const char* name);
    };



    /*-----------------------------------------------------------------------------
     * @about
     *          Smart Buffer with binary-code
     *
     *
     /----------------------------------------------------------------------------*/
    const int BIN_BUFFER_SIZE = 16384;

    const int LOCAL_BUFFER_SIZE = 20;

    struct Bin_Buffer
    {
        Task_Manager* task_manager_ = nullptr;

        uint8_t* buffer_ = nullptr;

        size_t  buffer_pos_ = 0;

        char* local_buffer_ = nullptr;


        /*-------------------------------------------*/

        Bin_Buffer() noexcept;

        ~Bin_Buffer() noexcept;

        void add_byte(const uint8_t byte);

        void add_word(const uint16_t word);

        void add_dword(const uint32_t dword);

        void add_qword(const uint64_t qword);

        void insert_byte(const uint8_t byte, size_t pos);

        void insert_word(const uint16_t word, size_t pos);

        void insert_dword(const uint32_t dword, size_t pos);

        void insert_qword(const uint64_t qword, size_t pos);

        void add_label(const char* label_name);

        void add_jump(const uint8_t byte, const char* label_name);

        void make_wonder();          // setting all jumps and calls

        void make_bin_file();

        void link_stdlib();

        void download_base_file(FILE* out);

        void clean_local_buffer();
    };


    /*-----------------------------------------------------------------------------
     * @about
     *          Main Manager of generating code
     *          Fills program with semantics
     *
     *
     /----------------------------------------------------------------------------*/
    struct Function_Handler
    {
        Bin_Compiler* boss_ptr_ = nullptr;

        std::unordered_map<char*, int> var_table_{};

        Command* current_command_ = nullptr;

        int current_indent_ = 8;

        /*-------------------------------------------*/

        Function_Handler() = default;

        explicit Function_Handler(Bin_Compiler* boss) noexcept ;

        void reset_boss(Bin_Compiler* boss);

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
 *  struct Bin_Compiler
 *  Compiler to .elf-64-86 executable files
 *
 *
 *
 /==========================================================================================================*/
struct Bin_Compiler
{
    Parser parser_{};

    bin_oak::Function_Handler foreman_;

    bin_oak::Bin_Buffer bin_buffer_;

    Function* func_list_ = nullptr;

    size_t if_amount_ = 0;

    size_t while_amount_ = 0;

    /*-------------------------------------------*/

    Bin_Compiler() = default;

    ~Bin_Compiler() noexcept ;

    void compile(const char* filename);

    void handle_func_list();
};

#include "Binarier.cpp"

#endif //BINARIER_H