#ifndef COMPILER_CPP
#define COMPILER_CPP

#include "Compiler.h"





/*=========================================================================================================
 *
 *                  Asm_Buffer Implementation
 *
 *                  Buffer with Assembler "text"
 *
 *
 *
 /==========================================================================================================*/
asm_oak::Asm_Buffer::Asm_Buffer() noexcept
{
    buffer_ = (char *) calloc(sizeof(char), asm_oak::ASM_BUFFER_SIZE);
    local_buffer_ = (char *) calloc(sizeof(char), asm_oak::LOCAL_BUFFER_SIZE);

}

asm_oak::Asm_Buffer::~Asm_Buffer() noexcept
{
    free(buffer_);
    buffer_ = nullptr;
    free(local_buffer_);
    local_buffer_ = nullptr;
    buffer_pos_ = 0;

}

void asm_oak::Asm_Buffer::add_string(const char *str)
{
    int i = 0;

    if (str[i] == '\n')
    {
        buffer_[buffer_pos_] = '\n';
        buffer_pos_++;
    }
    else
    {
        add_data(str);

        buffer_[buffer_pos_] = '\n';
        buffer_pos_++;
    }
}


void asm_oak::Asm_Buffer::add_data(const char *data)
{
    int i = 0;

    while(data[i] != 0)
    {
        buffer_[buffer_pos_] = data[i];
        buffer_pos_++;
        i++;
    }
}


void asm_oak::Asm_Buffer::add_number(int numb)
{
    if (numb == 0)
    {
        buffer_[buffer_pos_] = '0';
        buffer_pos_++;
    }
    else
    {
        clean_local_buffer();

        int pos = asm_oak::LOCAL_BUFFER_SIZE - 1;

        sprintf(local_buffer_, "%d", abs(numb));

        for (int i = 0; i < asm_oak::LOCAL_BUFFER_SIZE; i++)
        {
            if (local_buffer_[i] != 0)
            {
                buffer_[buffer_pos_] = local_buffer_[i];
                buffer_pos_++;
            }
        }

    }

}


void asm_oak::Asm_Buffer::make_asm_file()
{
    FILE* out = fopen("test_c.asm", "w");

    fprintf(out, "%s", buffer_);

    fclose(out);
}


void asm_oak::Asm_Buffer::clean_local_buffer()
{
    for(int i = 0; i < asm_oak::LOCAL_BUFFER_SIZE; i++)
    {
        local_buffer_[i] = 0;
    }
}


/*=========================================================================================================
 *
 *                  asm_oak::Function_Handler Implementation
 *
 *
 *
 /==========================================================================================================*/
#define ADD_STRING(STR)\
    boss_ptr_->asm_buffer_.add_string(STR)


/*without '\n' */
#define ADD_DATA(DATA)\
    boss_ptr_->asm_buffer_.add_data(DATA)


#define ADD_NUMBER(NUMBER)\
    boss_ptr_->asm_buffer_.add_number(NUMBER)


asm_oak::Function_Handler::Function_Handler(Asm_Compiler* boss) noexcept
{
    boss_ptr_ = boss;
}


void asm_oak::Function_Handler::reset_boss(Asm_Compiler *boss)
{
    boss_ptr_ = boss;
}

asm_oak::Function_Handler::~Function_Handler()
{
    boss_ptr_ = nullptr;
    current_indent_ = 0;
}



//TODO fix this awesome shit
void asm_oak::Function_Handler::handle_function(Function *function)
{
    ADD_DATA(function->name_);
    ADD_DATA(":\n");

    ADD_STRING("push rbp");
    ADD_STRING("mov rbp, rsp");
    ADD_STRING("sub rsp, 256");

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

    ADD_STRING("add rsp, 256");
    ADD_STRING("pop rbp");
    ADD_STRING("ret");
    ADD_DATA("\n");
}


void asm_oak::Function_Handler::handle_command()
{
    if (current_command_ == nullptr)
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
            ADD_STRING("pop rax");
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



void asm_oak::Function_Handler::handle_var_def(Command* command)
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



void asm_oak::Function_Handler::handle_node(Node *node)
{
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
                ADD_DATA("push ");
                ADD_NUMBER(node->knot_.storage_.number_);
                ADD_DATA("\n");

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

void asm_oak::Function_Handler::handle_variable_assignment(Node *node)
{

    int indent = var_table_[node->knot_.storage_.var_->name_];

    printf("%d", indent);
    if (indent >= 0)
    {
        ADD_STRING("mov rax, rbp");
        ADD_DATA("add rax, ");
        ADD_NUMBER(indent);
        ADD_DATA("\n");
        ADD_STRING("push rax");
    }
    else
    {
        ADD_STRING("mov rax, rbp");
        ADD_DATA("sub rax, ");
        ADD_NUMBER(indent);
        ADD_DATA("\n");
        ADD_STRING("push rax");
    }
}




#define DO_IF_HANDLER_STUFF(JUMP_COMMAND)                                               \
    handle_node(node->left_node_->left_node_);                                          \
    handle_node(node->left_node_->right_node_);                                         \
                                                                                        \
    ADD_STRING("pop rax");                                                              \
    ADD_STRING("pop rbx");                                                              \
    ADD_STRING("cmp rax, rbx");                                                         \
    ADD_DATA(JUMP_COMMAND);                                                             \
                                                                                        \
    int if_pass_number = boss_ptr_->if_amount_;                                         \    
    boss_ptr_->if_amount_++;                                                            \
                                                                                        \
    ADD_NUMBER(if_pass_number);                                                         \
    ADD_DATA("\n");                                                                     \
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
    ADD_DATA("if_pass_");                                                               \
    ADD_NUMBER(if_pass_number);                                                         \
    ADD_DATA(":\n");



void asm_oak::Function_Handler::handle_if(Node *node)
{
    switch (node->left_node_->knot_.storage_.oper_)
    {
        case IF_EQUAL:
        {
            DO_IF_HANDLER_STUFF("jne if_pass_")
        }
            break;

        case IF_NOT_EQUAL:
        {   
            DO_IF_HANDLER_STUFF("je if_pass_")
        }
            break;

        case IF_GREAT:
        {
            DO_IF_HANDLER_STUFF("jle if_pass_")
        }
            break;

        case IF_LESS:
        {
            DO_IF_HANDLER_STUFF("jge if_pass_")
        }
            break;

        case IF_EOG:
        {
            DO_IF_HANDLER_STUFF("jl if_pass_")
        }
            break;

        case IF_EOL:
        {
            DO_IF_HANDLER_STUFF("jg if_pass_")
        }
            break;
    }
}
#undef DO_IF_HANDLER_STUFF



#define DO_WHILE_HANDLER_STUFF(BYPASS_JUMP, LOOP_JUMP)                                  \
    handle_node(node->left_node_->left_node_);                                          \
    handle_node(node->left_node_->right_node_);                                         \
                                                                                        \
    ADD_STRING("pop rax");                                                              \
    ADD_STRING("pop rbx");                                                              \
    ADD_STRING("cmp rax, rbx");                                                         \
    ADD_DATA(BYPASS_JUMP);                                                              \
                                                                                        \
    int while_number = boss_ptr_->while_amount_;                                        \
    boss_ptr_->while_amount_++;                                                         \
                                                                                        \
    ADD_NUMBER(while_number);                                                           \
    ADD_DATA("\n");                                                                     \
                                                                                        \
    ADD_DATA("while_loop_");                                                            \
    ADD_NUMBER(while_number);                                                           \
    ADD_DATA(":\n");                                                                    \
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
    ADD_STRING("pop rax");                                                              \
    ADD_STRING("pop rbx");                                                              \
    ADD_STRING("cmp rax, rbx");                                                         \
    ADD_DATA(LOOP_JUMP);                                                                \
    ADD_NUMBER(while_number);                                                           \
    ADD_DATA("\n");                                                                     \
                                                                                        \
    ADD_DATA("while_pass_");                                                            \
    ADD_NUMBER(while_number);                                                           \
    ADD_DATA(":\n");                                                                    \


void asm_oak::Function_Handler::handle_while(Node *node)
{
    switch (node->left_node_->knot_.storage_.oper_)
    {
        case IF_EQUAL:
        {
            DO_WHILE_HANDLER_STUFF("jne while_pass_", "je while_loop_")
        }
            break;

        case IF_NOT_EQUAL:
        {
            DO_WHILE_HANDLER_STUFF("je while_pass_", "jne while_loop_")
        }
            break;

        case IF_GREAT:
        {
            DO_WHILE_HANDLER_STUFF("jle while_pass_", "jg while_loop_")
        }
            break;

        case IF_LESS:
        {
            DO_WHILE_HANDLER_STUFF("jge while_pass_", "jl while_loop_")
        }
            break;

        case IF_EOG:
        {
            DO_WHILE_HANDLER_STUFF("jl while_pass_", "jge while_loop_")
        }
            break;

        case IF_EOL:
        {
            DO_WHILE_HANDLER_STUFF("jg while_pass_", "jle while_loop_")
        }
            break;
    }
}


void asm_oak::Function_Handler::handle_operation(Node *node)
{

    switch (node->knot_.storage_.oper_)
    {
        case PLUS:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);


            ADD_STRING("pop rax");
            ADD_STRING("pop rbx");
            ADD_STRING("add rax, rbx");
            ADD_STRING("push rax");
        }
            break;

        case MINUS:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);

            ADD_STRING("pop rax");
            ADD_STRING("pop rbx");
            ADD_STRING("sub rax, rbx");
            ADD_STRING("push rax");
        }
            break;

        case MULTIPLY:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);

            ADD_STRING("pop rax");
            ADD_STRING("pop rbx");
            ADD_STRING("mul ebx");
            ADD_STRING("push rax");
        }
            break;

        case DIVIDE:
        {
            handle_node(node->right_node_);
            handle_node(node->left_node_);

            ADD_STRING("pop rax");
            ADD_STRING("pop rbx");
            ADD_STRING("xor rdx, rdx");
            ADD_STRING("idiv ebx");
            ADD_STRING("push rax");
        }
            break;

            /* here we need to push in sub-trees */
        case ASSIGN:
        {
            handle_node(node->right_node_);
            handle_variable_assignment(node->left_node_);

            ADD_STRING("pop rax");
            ADD_STRING("pop rbx");
            ADD_STRING("mov [rax], rbx");

        }
            break;

    }

}

void asm_oak::Function_Handler::handle_func_call(Node *node)
{

    handle_procedure_call(node);
    ADD_STRING("push rax");
}


void asm_oak::Function_Handler::handle_procedure_call(Node *node)
{
    int roundabout = 0;
    Node* current = node->right_node_;


    while(current != nullptr)
    {
        handle_node(current);
        current = current->extra_node_;
        roundabout++;
    }

    ADD_DATA("call ");
    ADD_DATA(node->knot_.storage_.call_->name_);
    ADD_DATA("\n");

    for (int i = 0; i < roundabout; i++)
    {
        ADD_STRING("pop rbx");
    }
}

void asm_oak::Function_Handler::handle_variable_usage(Node *node)
{
    int indent = var_table_[node->knot_.storage_.var_->name_];


    if (indent >= 0)
    {
        ADD_DATA("mov rax, [rbp + ");
        ADD_NUMBER(indent);
        ADD_DATA("]\n");
        ADD_STRING("push rax");

    }
    else
    {
        ADD_DATA("mov rax, [rbp - ");
        ADD_NUMBER(indent);
        ADD_DATA("]\n");
        ADD_STRING("push rax");
    }

}


#undef ADD_STRING
#undef ADD_DATA
/*=========================================================================================================
 *
 *                  Asm_Compiler Implementation
 *
 *
 *
 /==========================================================================================================*/

Asm_Compiler::~Asm_Compiler()
{
    delete func_list_;
    func_list_ = nullptr;

}

void Asm_Compiler::compile(const char *filename)
{
    parser_.Read_file(filename);
    func_list_ = parser_.Get_program();

    func_list_->print();
    foreman_.reset_boss(this);


    handle_func_list();

    asm_buffer_.make_asm_file();

    system("nasm -f elf64 test_c.asm -o test_c.o");
    system("ld test_c.o -o test_c");
}


#define ASM_COMPILER_PRINTF(STR)\
    asm_buffer_.add_string(STR)

void Asm_Compiler::handle_func_list()
{
    /*to make header*/

    add_std_functions();

    Function* func = func_list_;

    while (func != nullptr)
    {
        foreman_.handle_function(func);
        func = func->next_func_;
    }



}



void Asm_Compiler::add_std_functions()
{

    oak::Stringer bup;

    bup.Read_file(asm_oak::STDLIB);

    for (int i = 0; i < bup.strings_amount_; i++)
    {
        ASM_COMPILER_PRINTF(bup.text_[i].body_);
    }



}


#undef ASM_COMPILER_PRINTF








#endif //COMPILER_CPP