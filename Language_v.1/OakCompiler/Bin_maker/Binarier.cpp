#ifndef BINARIER_CPP
#define BINARIER_CPP

#include "Binarier.h"




/*==========================================================================================================
 *
 *                       Welcome to Compiler to elf-64-86 executable files
 *
 *
 *                 Here you can see Implementation of functions
 *
 *
 *
 *
 /==========================================================================================================*/




/*==========================================================================================================
 *
 *                  Task struct Implementation
 *
 *                  Jump and call Assistants Node
 *
 /==========================================================================================================*/

void bin_oak::Task::set_label(const char *name, size_t pos)
{
    uint16_t len = strlen(name);

    if (len <= bin_oak::LABEL_NAME_SIZE)
    {
        label_name_ = new char [len];
        strcpy(label_name_, name);

        pos_ = pos;
    }
    else
    {
        FTHROW(LOGIC_ERROR, "Too big label name!");
    }
}


bin_oak::Task::~Task()
{
    delete [] label_name_;
    label_name_ = nullptr;

    pos_ = 0;

}



/*=========================================================================================================
 *
 *                  Task_Manager Implementation
 *
 *                  Jump and call Assistant
 *
 /==========================================================================================================*/

bin_oak::Task_Manager::Task_Manager()
{
    task_list_ = new Task [bin_oak::LABEL_NAME_SIZE];

    labels_list_ = new Task [bin_oak::TASK_LIST_SIZE];

}


bin_oak::Task_Manager::~Task_Manager()
{
    delete [] task_list_;
    task_list_ = nullptr;

    delete [] labels_list_;
    labels_list_ = nullptr;

}

/*-----------------------------------------------------------------------------
 * @about
 *          Adding task to paste coordinates of label later
 *         (to be exact, indent between the end of jump command
 *          and label's position)
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Task_Manager::add_task(const char *name, int pos)
{
    task_list_[current_pos_].set_label(name, pos);
    current_pos_++;

    if (current_pos_ == bin_oak::TASK_LIST_SIZE)
    {
        FTHROW(LOGIC_ERROR, "Task buffer overflow!");
    }
}


/*-----------------------------------------------------------------------------
 * @about
 *          Adding label and its position
 *
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Task_Manager::add_label(const char* name, int pos)
{
    uint16_t len = strlen(name);

    if (len <= bin_oak::LABEL_NAME_SIZE)
    {
        labels_list_[labels_list_pos_].label_name_ = new char [len];
        strcpy(labels_list_[labels_list_pos_].label_name_, name);

        labels_list_[labels_list_pos_].pos_ = pos;

        labels_list_pos_++;

    }
    else
    {
        FTHROW(LOGIC_ERROR, "Too big label name!");
    }


}

/*-----------------------------------------------------------------------------
 * @about
 *          Returning position of label
 *          (if you know name of label)
 *
 /----------------------------------------------------------------------------*/
int bin_oak::Task_Manager::get_label_pos(const char *name)
{
    for (int i = 0; i < bin_oak::TASK_LIST_SIZE; i++)
    {
        if (strcmp(name, labels_list_[i].label_name_) == 0)
        {
            return labels_list_[i].pos_;
        }
    }

    return -1;
}


/*=========================================================================================================
 *
 *                  Bin_Buffer Implementation
 *
 *                  Buffer with Assembler "text"
 *
 *
 *
 /==========================================================================================================*/
bin_oak::Bin_Buffer::Bin_Buffer() noexcept
{
    /* calloc - to fill memory with zeros*/

    buffer_ = (uint8_t *) calloc(sizeof(char), bin_oak::BIN_BUFFER_SIZE);

    local_buffer_ = (char *) calloc(sizeof(char), bin_oak::LOCAL_BUFFER_SIZE);

    task_manager_ = new Task_Manager;

}


bin_oak::Bin_Buffer::~Bin_Buffer() noexcept
{
    free(buffer_);
    buffer_ = nullptr;

    free(local_buffer_);
    local_buffer_ = nullptr;

    buffer_pos_ = 0;

    delete task_manager_;
    task_manager_ = nullptr;

}

/*-----------------------------------------------------------------------------
 * @about
 *          Internal commands for adding bytes to binary file
 *
 *          Be careful!
 *          Invert bytes order in numbers!
 *
 * @note
 *          A little bit weird!
 *          invert numbers!
 *
 /----------------------------------------------------------------------------*/

void bin_oak::Bin_Buffer::add_byte(const uint8_t byte)
{
    buffer_[buffer_pos_] = byte;
    buffer_pos_++;
}



void bin_oak::Bin_Buffer::add_word(const uint16_t word)
{
    add_byte((uint8_t) (word));              // lower bytes first!
    add_byte((uint8_t) (word >> 8));
}


void bin_oak::Bin_Buffer::add_dword(const uint32_t dword)
{
    add_word((uint16_t) (dword));            // lower bytes first!
    add_word((uint16_t) (dword >> 16));

}


void bin_oak::Bin_Buffer::add_qword(const uint64_t qword)
{
    add_dword((uint32_t) (qword));           // lower bytes first!
    add_dword((uint32_t) (qword >> 32));
}



/*-----------------------------------------------------------------------------
 * @about
 *          weird commands for setting indents
 *          in binary buffer
 *
 * @note
 *          really weird!
 *          invert numbers!
 *
 *
 /----------------------------------------------------------------------------*/

void bin_oak::Bin_Buffer::insert_byte(const uint8_t byte, size_t pos)
{
    buffer_[pos] = byte;
}

void bin_oak::Bin_Buffer::insert_word(const uint16_t word, size_t pos)
{
    insert_byte((uint8_t)(word), pos);        // lower bytes first!
    insert_byte((uint8_t)(word >> 8), pos + 1);
}

void bin_oak::Bin_Buffer::insert_dword(const uint32_t dword, size_t pos)
{
    insert_word((uint16_t)(dword), pos);      // lower bytes first!
    insert_word((uint16_t)(dword >> 16), pos + 2);
}

void bin_oak::Bin_Buffer::insert_qword(const uint64_t qword, size_t pos)
{
    insert_dword((uint32_t)(qword), pos);      // lower bytes first!
    insert_dword((uint32_t)(qword >> 32), pos + 4);
}


void bin_oak::Bin_Buffer::add_label(const char *label_name)
{
    task_manager_->add_label(label_name, buffer_pos_);

}


/*-----------------------------------------------------------------------------
 * @about
 *          handle jump and call commands
 *
 *
 * @note
 *          Here I'm using long jumps
 *          that's why 4 bytes after jump command
 *
 *          Indents in jumps will be set later
 /----------------------------------------------------------------------------*/

void bin_oak::Bin_Buffer::add_jump(const uint8_t byte, const char *label_name)
{
    add_byte(byte);
    task_manager_->add_task(label_name, buffer_pos_);

    buffer_pos_ += JUMP_COORD_SIZE;

}


/*-----------------------------------------------------------------------------
 * @about
 *          setting jumps indents
 *          using structures in task manager
 *
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Bin_Buffer::make_wonder()
{
    uint32_t start;
    uint32_t finish;

    for(int i = 0; i < task_manager_->current_pos_; i++)
    {
        start = (uint32_t) task_manager_->task_list_[i].pos_;
        finish = (uint32_t) task_manager_->get_label_pos(task_manager_->task_list_[i].label_name_);

        if ((start > 0) && (start < BIN_BUFFER_SIZE))
        {
            insert_dword(finish - (start + JUMP_COORD_SIZE), start);
        }

    }
}




/*-----------------------------------------------------------------------------
 * @about
 *          Linking my standard lib to executable file
 *          Be careful!
 *          some exact data is here
 *
 *
 * @note
 *          We are counting from START_COORD!
 *          START_COORD is a zero in our bin_buffer
 *
 /----------------------------------------------------------------------------*/
#define JUMP_MAIN_COORD 177
#define INPUT_COORD 195
#define OUTPUT_COORD 285
#define START_COORD 459


/*-------------------------------------------------------------------------------------
 * @about
 *          Add "coordinates" of stdlib functions
 *          to Label-relation-handling structure
 *
 /-------------------------------------------------------------------------------------*/
void bin_oak::Bin_Buffer::link_stdlib()
{
    task_manager_->add_label(bin_oak::INPUT_FUNC_NAME, INPUT_COORD - START_COORD);
    task_manager_->add_label(bin_oak::OUTPUT_FUNC_NAME, OUTPUT_COORD - START_COORD);
}

/*-----------------------------------------------------------------------------
 * @about
 *          Creating binary file with header and byte code,
 *          which was generated by us
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Bin_Buffer::make_bin_file()
{
    link_stdlib();

    printf("\n");


    make_wonder();

    FILE* out = fopen(bin_oak::OUTPUT_FILE_NAME, "wb");

    if (out == nullptr)
    {
        clean_local_buffer();
        sprintf(local_buffer_, "rm %s",bin_oak::OUTPUT_FILE_NAME);
        system(local_buffer_);

        FILE* out = fopen(bin_oak::OUTPUT_FILE_NAME, "wb");
    }
    else if (out == nullptr)
    {
        FTHROW(MEMORY_ACCES_ERROR, "Can't open file!");
    }

    download_base_file(out);

    fseek(out, START_COORD - 2, SEEK_CUR);

    fwrite(buffer_, sizeof(uint8_t), buffer_pos_, out);

    fclose(out);

}

/*-----------------------------------------------------------------------------
 * @about
 *          Downloading file with elf-64-86 header
 *          and several "STDLIB functions" to our binary file
 *
 *
 * @note
 *          Also linking our main to "call main" in a header
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Bin_Buffer::download_base_file(FILE* out)
{
    FILE* base = fopen(bin_oak::BASE_FILE_NAME, "r+b");

    if (base == nullptr)
    {
        FTHROW(MEMORY_ACCES_ERROR, "Can't open base file!");
    }

    /*----------------------------SIZEOF(FILE)--------------------------------------------*/
    fseek (base, 0, SEEK_END);

    size_t file_length = ftell (base);

    rewind(base);
    /*------------------------------------------------------------------------------------*/


    auto bin_buffer = new uint8_t [file_length];

    fread(bin_buffer, sizeof(uint8_t), file_length, base);


    /*--------------------LINKING MAIN TO HEADER------------------------------------------*/

    uint32_t indent = task_manager_->get_label_pos(MAIN_NAME) + (START_COORD - JUMP_MAIN_COORD) - 6;

    bin_buffer[JUMP_MAIN_COORD + 0] = (uint8_t)(indent >> 8 * 0);
    bin_buffer[JUMP_MAIN_COORD + 1] = (uint8_t)(indent >> 8 * 1);
    bin_buffer[JUMP_MAIN_COORD + 2] = (uint8_t)(indent >> 8 * 2);
    bin_buffer[JUMP_MAIN_COORD + 3] = (uint8_t)(indent >> 8 * 3);

    /*------------------------------------------------------------------------------------*/


    fwrite(bin_buffer, sizeof(uint8_t), file_length, out);
    rewind(out);

    delete [] bin_buffer;

}

void bin_oak::Bin_Buffer::clean_local_buffer()
{
    for (int i = 0; i < bin_oak::LOCAL_BUFFER_SIZE; i++)
    {
        local_buffer_[i] = 0x0;
    }
}

#undef JUMP_MAIN_COORD
#undef START_COORD


/*=========================================================================================================
 *
 *                  bin_oak::Function_Handler Implementation
 *
 *
 *
 /==========================================================================================================*/
#define ADD_DB(BYTE)\
    boss_ptr_->bin_buffer_.add_byte(BYTE)

#define ADD_3DB(BYTE1, BYTE2, BYTE3)        \
    ADD_DB(BYTE1);                          \
    ADD_DB(BYTE2);                          \
    ADD_DB(BYTE3);


#define ADD_DW(WORD)\
    boss_ptr_->bin_buffer_.add_word(WORD)

#define ADD_DD(DWORD)\
    boss_ptr_->bin_buffer_.add_dword(DWORD)

#define ADD_DQ(WORD)\
    boss_ptr_->bin_buffer_.add_qword(QWORD)

#define ADD_LABEL(LABEL)\
    boss_ptr_->bin_buffer_.add_label(LABEL)

#define ADD_JUMP(OPCODE, TO_LABEL)\
    boss_ptr_->bin_buffer_.add_jump(OPCODE, TO_LABEL)


bin_oak::Function_Handler::Function_Handler(Bin_Compiler* boss) noexcept
{
    boss_ptr_ = boss;
}


void bin_oak::Function_Handler::reset_boss(Bin_Compiler *boss)
{
    boss_ptr_ = boss;
}

bin_oak::Function_Handler::~Function_Handler()
{
    boss_ptr_ = nullptr;
    current_indent_ = 0;
}



/*-----------------------------------------------------------------------------
 * @about
 *          Just handle one funtion from list of functions
 *          1) Make stack frame
 *          2) Handle commands
 *          3) Finish stack frae
 *
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Function_Handler::handle_function(Function *function)
{
    ADD_LABEL(function->name_);

    ADD_DB(0x55);                                //ADD_STRING("push rbp");
    ADD_3DB(0x48, 0x89, 0xE5);                   //ADD_STRING("mov rbp, rsp");
    ADD_3DB(0x48, 0x81, 0xEC);                   //ADD_STRING("sub rsp, stack_indent");


    ADD_DD(STACK_VARIABLES_INDENT);

    /* handling stack frame */
    int indent = 16;

    for (int i = 0; i < function->param_amount_; i++)
    {

        var_table_[function->param_var_list_->list_[i].name_] = indent;
        indent += 8;
    }

    /* handling all commands in function*/
    current_command_ = function->first_;

    while (current_command_ != nullptr)
    {
        handle_command();
    }

    ADD_3DB(0x48, 0x81, 0xC4);                  //ADD_STRING("add rsp, stack_indent");
    ADD_DD(STACK_VARIABLES_INDENT);
    ADD_DB(0x5D);                               //ADD_STRING("pop rbp");
    ADD_DB(0xc3);                               //ADD_STRING("ret");
}


/*-----------------------------------------------------------------------------
 * @about
 *          Just handling one element from list of commands
 *
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Function_Handler::handle_command()
{
    INT_3  // debug mode

    if (current_command_== nullptr)
    {
        return;
    }

    switch(current_command_->type_)
    {
        case VARIABLE_DEFINITION:
        {
            handle_var_def(current_command_);
            current_command_ = current_command_->next_comm_;
        }
            break;

        case RETURN:
        {
            handle_node(current_command_->root_);

            ADD_DB(0x58);       //ADD_STRING("pop rax");

            current_command_ = current_command_->next_comm_;

        }
            break;

        case TREE:
        {
            if (current_command_->root_->knot_.type_ == CALL)
            {
                handle_procedure_call(current_command_->root_);
            }
            else
            {
                handle_node(current_command_->root_);
            }

            current_command_ = current_command_->next_comm_;

        }
            break;

        case IF_COMMAND:
        {
            handle_if(current_command_->root_);
        }
            break;

        case WHILE_COMMAND:
        {
            handle_while(current_command_->root_);
        }
            break;
    }


}


/*-----------------------------------------------------------------------------
 * @about
 *          just handling definition of variable
 *          "allocates" memory in stack (We have made stack indent in the begining)
 *
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Function_Handler::handle_var_def(Command* command)
{
    var_table_[command->root_->knot_.storage_.var_->name_] = -current_indent_;

    switch(command->root_->knot_.storage_.var_->type_)
    {
        case INTEGER:
        {
            current_indent_ += 8;
        }
            break;
    }
}

/*-----------------------------------------------------------------------------
 * @about
 *          Just handling node of our tree
 *
 /----------------------------------------------------------------------------*/

void bin_oak::Function_Handler::handle_node(Node *node)
{
    INT_3  // debug mode

    if (node != nullptr)
    {
        switch (node->knot_.type_)
        {
            case OPERATOR_TYPE:
            {
                handle_operation(node);
            }
                break;

            case INT_CONSTANT:
            {
                ADD_DB(0x68);                                //ADD_DATA("push ");
                ADD_DD(node->knot_.storage_.number_);        //ADD_NUMBER(node->knot_.storage_.number_);
            }
                break;

            case VARIABLE:
            {
                handle_variable_usage(node);
            }
                break;

            case CALL:
            {
                handle_func_call(node);
            }
                break;
        }
    }

}


/*-----------------------------------------------------------------------------
 * @about
 *          "returns" pointer at variable
 *          (in quotes, because i mean returning in generating code)
 *
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Function_Handler::handle_variable_assignment(Node *node)
{

    int indent = var_table_[node->knot_.storage_.var_->name_];

    if (indent >= 0)
    {

        ADD_3DB(0x48, 0x89, 0xE8)       //ADD_STRING("mov rax, rbp");

        ADD_DB(0x48);                   //ADD_DATA("add rax, ");
        ADD_DB(0x05);

        ADD_DD(indent);                 //ADD_NUMBER(indent);


        ADD_DB(0x50);                   //ADD_STRING("push rax");
    }
    else
    {
        ADD_3DB(0x48, 0x89, 0xE8)       //ADD_STRING("mov rax, rbp");

        ADD_DB(0x48);                   //ADD_DATA("sub rax, ");
        ADD_DB(0x2D);

        ADD_DD(abs(indent));             //ADD_NUMBER(indent);

        ADD_DB(0x50);                   //ADD_STRING("push rax");
    }
}


/*-----------------------------------------------------------------------------
 * @about
 *          Handle if
 *          The structure, which is generating by this code,
 *          includes checking condition and bypassing jump
 *          if condition is wrong
 *
 *
 /----------------------------------------------------------------------------*/

#define DO_IF_HANDLER_STUFF(JUMP_OPCODE)                                                \
    handle_node(node->left_node_->left_node_);                                          \
    handle_node(node->left_node_->right_node_);                                         \
                                                                                        \
    ADD_DB(0x58);                   /*ADD_STRING("pop rax");*/                          \
    ADD_DB(0x5B);                   /*ADD_STRING("pop rbx");*/                          \
    ADD_3DB(0x48, 0x39, 0xD8);      /*ADD_STRING("cmp rax, rbx");*/                     \
                                                                                        \
    int if_pass_number = boss_ptr_->if_amount_;                                         \
    boss_ptr_->if_amount_++;                                                            \
                                                                                        \
    sprintf(label_buffer, "if_pass_%d", if_pass_number);                                \
    ADD_DB(0x0F);               /*identifier of long jump*/                             \
    ADD_JUMP(JUMP_OPCODE, label_buffer);                                                \
                                                                                        \
    int com_am = node->right_node_->knot_.storage_.number_;                             \
                                                                                        \
    current_command_ = current_command_->next_comm_;                                    \
                                                                                        \
    for (int i = 0; i < com_am; i++)                                                    \
    {                                                                                   \
        handle_command();                                                               \
    }                                                                                   \
                                                                                        \
    ADD_LABEL(label_buffer);



void bin_oak::Function_Handler::handle_if(Node *node)
{
    auto label_buffer = (char*) calloc(sizeof(char), LABEL_NAME_SIZE);

    switch (node->left_node_->knot_.storage_.oper_)
    {
        case IF_EQUAL:
        {
            DO_IF_HANDLER_STUFF(0x85)               //"jne if_pass_"
        }
            break;

        case IF_NOT_EQUAL:
        {   
            DO_IF_HANDLER_STUFF(0x84)               //"je if_pass_"
        }
            break;

        case IF_GREAT:
        {
            DO_IF_HANDLER_STUFF(0x8E)               //"jle if_pass_"
        }
            break;

        case IF_LESS:
        {
            DO_IF_HANDLER_STUFF(0x8D)               //"jge if_pass_"
        }
            break;

        case IF_EOG:
        {
            DO_IF_HANDLER_STUFF(0x8C)               //"jl if_pass_"
        }
            break;

        case IF_EOL:
        {
            DO_IF_HANDLER_STUFF(0x8F)               //"jg if_pass_"
        }
            break;
    }

    free(label_buffer);
}

#undef DO_IF_HANDLER_STUFF




/*-----------------------------------------------------------------------------
 * @about
 *          Handle while loop
 *          The structure, which is generating by this code,
 *          includes bypassing jump for first check of condition
 *          and loop with checking condition at the end of each roundabout
 *
 *
 *
 /----------------------------------------------------------------------------*/


#define DO_WHILE_HANDLER_STUFF(BYPASS_JUMP, LOOP_JUMP)                                  \
    handle_node(node->left_node_->left_node_);                                          \
    handle_node(node->left_node_->right_node_);                                         \
                                                                                        \
    ADD_DB(0x58);                   /*ADD_STRING("pop rax");*/                          \
    ADD_DB(0x5B);                   /*ADD_STRING("pop rbx");*/                          \
    ADD_3DB(0x48, 0x39, 0xD8);      /*ADD_STRING("cmp rax, rbx");*/                     \
                                                                                        \
    int while_number = boss_ptr_->while_amount_;                                        \
    boss_ptr_->while_amount_++;                                                         \
                                                                                        \
    sprintf(label_buffer, "while_pass_%d", while_number);                               \
    ADD_DB(0x0F);               /*identifier of long jump*/                             \
    ADD_JUMP(BYPASS_JUMP, label_buffer);                                                \
                                                                                        \
    sprintf(loop_buffer, "while_loop_%d", while_number);                                \
    ADD_LABEL(loop_buffer);                                                             \
                                                                                        \
    int com_am = node->right_node_->knot_.storage_.number_;                             \
                                                                                        \
    current_command_ = current_command_->next_comm_;                                    \
                                                                                        \
    for (int i = 0; i < com_am; i++)                                                    \
    {                                                                                   \
        handle_command();                                                               \
    }                                                                                   \
                                                                                        \
    handle_node(node->left_node_->left_node_);                                          \
    handle_node(node->left_node_->right_node_);                                         \
                                                                                        \
    ADD_DB(0x58);                   /*ADD_STRING("pop rax");*/                          \
    ADD_DB(0x5B);                   /*ADD_STRING("pop rbx");*/                          \
    ADD_3DB(0x48, 0x39, 0xD8);      /*ADD_STRING("cmp rax, rbx");*/                     \
    ADD_DB(0x0F);               /*identifier of long jump*/                             \
    ADD_JUMP(LOOP_JUMP, loop_buffer);                                                   \
                                                                                        \
    ADD_LABEL(label_buffer);



void bin_oak::Function_Handler::handle_while(Node *node)
{
    auto label_buffer = (char*) calloc(sizeof(char), LABEL_NAME_SIZE);
    auto loop_buffer = (char*) calloc(sizeof(char), LABEL_NAME_SIZE);

    switch (node->left_node_->knot_.storage_.oper_)
    {
        case IF_EQUAL:
        {
            DO_WHILE_HANDLER_STUFF(0x85, 0x84)        //"jne while_pass_", "je while_loop_"
        }
            break;

        case IF_NOT_EQUAL:
        {
            DO_WHILE_HANDLER_STUFF(0x84, 0x85)          //"je while_pass_", "jne while_loop_"
        }
            break;

        case IF_GREAT:
        {
            DO_WHILE_HANDLER_STUFF(0x8E, 0x8F)          //"jle while_pass_", "jg while_loop_"
        }
            break;

        case IF_LESS:
        {
            DO_WHILE_HANDLER_STUFF(0x8D, 0x8C)          //"jge while_pass_", "jl while_loop_"
        }
            break;

        case IF_EOG:
        {
            DO_WHILE_HANDLER_STUFF(0x8C, 0x8D)          //"jl while_pass_", "jge while_loop_"
        }
            break;

        case IF_EOL:
        {
            DO_WHILE_HANDLER_STUFF(0x8F, 0x8E)          //"jg while_pass_", "jle while_loop_"
        }
            break;
    }

    free(label_buffer);
    free(loop_buffer);
}


/*-----------------------------------------------------------------------------
 * @about
 *          Handle one operation
 *          (you can find enum OPERATOR_NAMES in Lexic.h)
 *
 *
 * @note
 *          Byte-codes with assembler listing
 /----------------------------------------------------------------------------*/
void bin_oak::Function_Handler::handle_operation(Node *node)
{

    switch (node->knot_.storage_.oper_)
    {
        case PLUS:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);

            ADD_DB(0x58);               //ADD_STRING("pop rax");
            ADD_DB(0x5B);               //ADD_STRING("pop rbx");
            ADD_3DB(0x48, 0x01, 0xD8);  //ADD_STRING("add rax, rbx");
            ADD_DB(0x50);               //ADD_STRING("push rax");
        }
            break;

        case MINUS:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);

            ADD_DB(0x58);               //ADD_STRING("pop rax");
            ADD_DB(0x5B);               //ADD_STRING("pop rbx");
            ADD_3DB(0x48, 0x29, 0xD8);  //ADD_STRING("sub rax, rbx");
            ADD_DB(0x50);               //ADD_STRING("push rax");
        }
            break;

        case MULTIPLY:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);

            ADD_DB(0x58);               //ADD_STRING("pop rax");
            ADD_DB(0x5B);               //ADD_STRING("pop rbx");
            ADD_DB(0xF7);               //ADD_STRING("mul ebx");
            ADD_DB(0xE3);

            ADD_DB(0x50);               //ADD_STRING("push rax");
        }
            break;

        case DIVIDE:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);

            ADD_DB(0x58);               //ADD_STRING("pop rax");
            ADD_DB(0x5B);               //ADD_STRING("pop rbx");
            ADD_3DB(0x48, 0x31, 0xD2);  //ADD_STRING("xor rdx, rdx");
            ADD_DB(0xF7);               //ADD_STRING("idiv ebx");
            ADD_DB(0xFB);

            ADD_DB(0x50);               //ADD_STRING("push rax");
        }
            break;

            /* here we need to push in sub-trees */
        case ASSIGN:
        {
            handle_node(node->right_node_);
            handle_variable_assignment(node->left_node_);

            ADD_DB(0x58);               //ADD_STRING("pop rax");
            ADD_DB(0x5B);               //ADD_STRING("pop rbx");
            ADD_3DB(0x48, 0x89, 0x18);  //ADD_STRING("mov [rax], rbx");

        }
            break;

    }

}


/*-----------------------------------------------------------------------------
 * @about
 *          Handle functions calls
 *
 /----------------------------------------------------------------------------*/
void bin_oak::Function_Handler::handle_func_call(Node *node)
{

    handle_procedure_call(node);
    ADD_DB(0x50);               //ADD_STRING("push rax");
}

void bin_oak::Function_Handler::handle_procedure_call(Node *node)
{
    int roundabout = 0;
    Node* current = node->right_node_;

    while(current != nullptr)
    {
        handle_node(current);
        current = current->extra_node_;
        roundabout++;
    }

    ADD_JUMP(0xE8, node->knot_.storage_.call_->name_);
        //ADD_DATA("call ");
        //ADD_DATA(node->knot_.storage_.call_->name_);

    for (int i = 0; i < roundabout; i++)
    {
        ADD_DB(0x5B);               //ADD_STRING("pop rbx");
    }
}


/*-----------------------------------------------------------------------------
 * @about
 *          Handle usage of variable
 *
 *
 *
 * @note
 *          Indent could be as one-byte or dword
 /----------------------------------------------------------------------------*/
void bin_oak::Function_Handler::handle_variable_usage(Node *node)
{
    int indent = var_table_[node->knot_.storage_.var_->name_];

    /* ------One-byte-indent mode-----------------------

    ADD_3DB(0x48, 0x8B, 0x45);  //ADD_DATA("mov rax, [rbp + ");
    ADD_DB(indent);             //ADD_NUMBER(indent);

     *---------------------------------------------------*/


    /*--------------------------dword-indent mode-----------------------*/
    ADD_3DB(0x48, 0x8B, 0x85);  //ADD_DATA("mov rax, [rbp + ");
    ADD_DD(indent);             //ADD_NUMBER(indent);
    /*-------------------------------------------------------------------*/

    ADD_DB(0x50);               //ADD_STRING("push rax");

}


#undef ADD_DD
#undef ADD_3DB
#undef ADD_JUMP
#undef ADD_DW
#undef ADD_DD
#undef ADD_DQ
#undef ADD_LABEL
/*=========================================================================================================
 *
 *                  Bin_Compiler Implementation
 *
 *
 *
 /==========================================================================================================*/

Bin_Compiler::~Bin_Compiler()
{
    delete func_list_;
    func_list_ = nullptr;

}


/*-----------------------------------------------------------------------------
 * @about
 *          1) Parses the file
 *          2) Generates file with binary code
 *          3) Changes mode of this file to executable
 *
 *
 /----------------------------------------------------------------------------*/
void Bin_Compiler::compile(const char *filename)
{
    parser_.Read_file(filename);
    func_list_ = parser_.Get_program();

    func_list_->print();
    foreman_.reset_boss(this);

    handle_func_list();

    bin_buffer_.make_bin_file();

    system("chmod 777 ./test");

}



/*-----------------------------------------------------------------------------
 * @about
 *          Handling list of functions (structure from Parser)
 *
 *
 /----------------------------------------------------------------------------*/
void Bin_Compiler::handle_func_list()
{
    /*to make header*/

    Function* func = func_list_;

    while (func != nullptr)
    {
        foreman_.handle_function(func);
        func = func->next_func_;
    }

}


#undef Bin_Compiler_PRINTF








#endif //BINARIER_CPP