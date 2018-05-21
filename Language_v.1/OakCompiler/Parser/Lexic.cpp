#ifndef LEXIC_CPP
#define LEXIC_CPP



/* =================================================================================================
 *
 *                                  Lex_t struct, Implementation
 *
 *
 *
 /==================================================================================================*/
#include "Lexic.h"


/* -------------------------------------------------------
 *  @about
 *          Constructor for operators
 *
 /------------------------------------------------------*/
Lex_t::Lex_t(enum OPERATOR_NAMES oper):
        type_ (OPERATOR_TYPE)
{
    storage_.oper_ = oper;
}



/* -------------------------------------------------------
 *  @about
 *          Constructor for double data
 *
 /------------------------------------------------------*/
Lex_t::Lex_t(double val):
        type_(DOUBLE_CONSTANT)
{
    storage_.real_ = val;
}


/* -------------------------------------------------------
 *  @about
 *          Constructor for Integer data
 *
 /------------------------------------------------------*/
Lex_t::Lex_t(int numb):
        type_(INT_CONSTANT)
{
    storage_.number_ = numb;
}

/* -------------------------------------------------------
 *  @about
 *          Constructor for Variables
 *
 /------------------------------------------------------*/
Lex_t::Lex_t(Variable* var):
        type_(VARIABLE)
{
    storage_.var_ = var ;
}

/* -------------------------------------------------------
 *  @about
 *          Constructor for Function calls
 *
 /------------------------------------------------------*/
Lex_t::Lex_t(Func_call* call):
        type_(CALL)
{
    storage_.call_ = call ;
}

/* -----------------------------------------------------------------------------------
 *  @about
 *          Special overload with aim to Throw exception, if type is unknown
 *
 *
 *
 /---------------------------------------------------------------------------------*/
Lex_t::Lex_t(...)
{
    THROW(LOGIC_ERROR, "This type of input is inappropriate!", nullptr);
}


/* --------------------------------------------------------------------
 *  @about
 *         Lex_t destructor
 *
 *
 /---------------------------------------------------------------------*/
Lex_t::~Lex_t()
{
    if (this!= nullptr)
    {
        type_ = LEX_TYPE_DEFAULT;
        storage_.real_ = 0;
    }
}


/* --------------------------------------------------------------------
 *  @about
 *         operator =
 *
 *
 /---------------------------------------------------------------------*/
inline Lex_t& Lex_t::operator=(const Lex_t& that)
{
    type_ = that.type_;
    storage_ = that.storage_;
}


inline void Lex_t::print() const
{
    switch(type_)
    {
        case OPERATOR_TYPE:
        {
            printf("type is - [%s], data - [%s]\n", "OPERATOR", OP_NAMES[storage_.oper_]);
        }
            break;

        case INT_CONSTANT:
        {
            printf("type is - [%s], data - [%d]\n", "INT_CONSTANT", storage_.number_);
        }
            break;

        case DOUBLE_CONSTANT:
        {
            printf("type is - [%s], data - [%f]\n", "DOUBLE_CONSTANT", storage_.real_);
        }
            break;

        case VARIABLE:
        {
            printf("type is - [%s], data - [%s]\n", "VARIABLE", storage_.var_->name_);
        }
            break;

        case CONDITION:
        {
            printf("type is - [%s], data - [%s]\n", "CONDITION", OP_NAMES[storage_.oper_]);
        }
            break;

        case CALL:
        {
            printf("type is - [%s], name - [%s],"
                  " param - [%d]\n", "CALL", storage_.call_->name_, storage_.call_->param_amount_);
        }
            break;

    }

}



/* =================================================================================================
 *
 *                                  Node struct, Implementation
 *
 *
 *
 /==================================================================================================*/


/* -------------------------------------------------------
 *  @about
 *      Template  Node constructor
 *
 *
 *      Template, because we have several types
 *
 /------------------------------------------------------*/
template <typename LT>
Node::Node(LT object):
        knot_(Lex_t(object)),
        left_node_(nullptr),
        right_node_(nullptr),
        extra_node_(nullptr)
{};



/* -------------------------------------------------------
 *  @about
 *      Adding left node
 *
 *
 *      Template, because we have several types
 *
 /------------------------------------------------------*/
template <typename LT>
void Node::set_left(LT object)
{
    if (left_node_ != nullptr)
    {
        THROW(LOGIC_ERROR, "Node if already exist!", nullptr);
    }

    else
    {
        left_node_ = new Node(object);
    }
}


/* -------------------------------------------------------
 *  @about
 *      Adding right node
 *
 *
 *      Template, because we have several types
 *
 /------------------------------------------------------*/
template <typename LT>
void Node::set_right(LT object)
{
    if (right_node_ != nullptr)
    {
        THROW(LOGIC_ERROR, "Node if already exist!", nullptr);
    }

    else
    {
        right_node_ = new Node(object);
    }
}


/* -------------------------------------------------------
 *  @about
 *        Tree destructor
 *
 *
 *      Destructs node and all its sub-nodes
 *
 /------------------------------------------------------*/
Node::~Node()
{

    if (this != nullptr)
    {
        delete right_node_;
        delete left_node_;
        delete extra_node_;

        left_node_ = nullptr;
        right_node_ = nullptr;
        extra_node_ = nullptr;
    }
};


/* --------------------------------------------------------------------------------------
 *  @about
 *      operator = is NOT Implemented!
 *
 *
 *      Just "catches" errors and throws exceptions
 *
 /-------------------------------------------------------------------------------------*/
Node& Node::operator = (const Node& that)
{
    THROW(LOGIC_ERROR,"Sorry operator= for Node is deleted because of safety", nullptr);
}



void Node::surface_copy(const Node &that)
{
    knot_ = that.knot_;
    left_node_ = that.left_node_;
    right_node_ = that.right_node_;
}

inline void Node::print() const
{
    if (this != nullptr)
    {
        extra_node_->print();

        left_node_->print();

        right_node_->print();

        knot_.print();

    }
}

/*===========================================================================================================
 *
 *
 *                                       Variable functions
 *
 *
 *
 /==========================================================================================================*/

/* -------------------------------------------------------------------
 * @about
 *        Constructor for variable
 *
 /--------------------------------------------------------------------*/
Variable::Variable(enum VAR_TYPES type, const char *name)
{
    type_ = type;
    extra_data_ = 0;
    size_t len = strlen(name);

    if (len > MAX_VAR_NAME_LEN)
    {
        THROW(SYNTAX_ERROR, "Variable name is too big!", nullptr);
    }
    else
    {
        name_ = new char [len];
        strcpy(name_, name);
    }
}


/* -------------------------------------------------------------------
 * @about
 *        Copy constructor for variable
 *
 /--------------------------------------------------------------------*/
Variable::Variable(const Variable& that)
{

    name_ = new char [sizeof(that.name_)];

    strcpy(name_, that.name_);

    type_ = that.type_;

}


/* -------------------------------------------------------------------
 * @about
 *        Destructor for variable
 *
 /--------------------------------------------------------------------*/
Variable::~Variable()
{

    type_ = VAR_TYPES_DEFAULT;
    extra_data_ = 0;

    delete [] name_;
    name_ = nullptr;

}


/* -------------------------------------------------------------------
 * @about
 *        Operator =  for Variable
 *        for long-lived objects
 *
 /--------------------------------------------------------------------*/
Variable& Variable::operator= (Variable that) noexcept
{
    swap(that);
}




/* -------------------------------------------------------------------
 * @about
 *        swap procedure for variable
 *
 /--------------------------------------------------------------------*/
void Variable::swap(Variable& that) noexcept
{
    std::swap(type_, that.type_);
    std::swap(name_, that.name_);
    std::swap(extra_data_, that.extra_data_);
}

/* =================================================================================================
*
*                                 struct Func_call
*
*        declaration of function call
*
*
*
/==================================================================================================*/


Func_call::Func_call(const char *name, const int param_amount)
{
    param_amount_ = param_amount;
    size_t len = strlen(name);

    if (len > MAX_VAR_NAME_LEN)
    {
        THROW(SYNTAX_ERROR, "Function name is too big!", nullptr);
    }
    else
    {
        name_ = new char [len];
        strcpy(name_, name);
    }
}

Func_call::~Func_call()
{
    delete [] name_;
    name_ = nullptr;

    param_amount_ = -1;
}

/*===========================================================================================================
 *
 *
 *                                       Variables List functions
 *
 *
 *
 /==========================================================================================================*/
/* -------------------------------------------------------
 * @about
 *        Constructor for variables list
 *
 /--------------------------------------------------------*/
Var_list::Var_list(size_t capacity)
{
    if (capacity <= MIN_VAR_LIST_SIZE)
    {
        list_ = new Variable [MIN_VAR_LIST_SIZE];
    }
    else
    {
        list_ = new Variable [capacity];
    }


    name_buffer_ = new char [MAX_VAR_NAME_LEN];

    amount_ = 0;
}


/* -------------------------------------------------------
 * @about
 *        Check if variable is already in list
 *
 * @input
 *        Variable name
 *
 * @output
 *         true or false
 *
 /--------------------------------------------------------*/
inline bool Var_list::is_in(const char *name) const
{
    bool yeah_it_is = false;

    for(int i = 0; i < amount_; i++)
    {
        if(strcmp(list_[i].name_, name) == 0)
        {
            yeah_it_is = true;
        }
    }

    return yeah_it_is;
}


/* -------------------------------------------------------------------
 * @about
 *        returns Variable from Variables list
 *
 *
 /--------------------------------------------------------------------*/
Variable* Var_list::get_var(const char* name) const
{
    if (!is_in(name))
    {
        THROW(SYNTAX_ERROR, "Unknown variable!", nullptr);
    }
    else
    {
        for(int i = 0; i < amount_; i++)
        {
            if(strcmp(list_[i].name_, name) == 0)
            {
                return &list_[i];
            }
        }
    }

}


/* -------------------------------------------------------
 * @about
 *       Adds variable to list
 *       Throws if variable is already in list
 *
 /--------------------------------------------------------*/
void Var_list::add_variable(enum VAR_TYPES type, const char *name)
{
    if (is_in(name))
    {
        THROW(SYNTAX_ERROR, "Duplicate declaration of variable! (or function)", nullptr);
    }
    else
    {
        list_[amount_] = Variable(type, name);
        amount_++;
    }

}


void Var_list::add_function(const char *name, int param_amount)
{
    if (is_in(name))
    {
        THROW(SYNTAX_ERROR, "Duplicate declaration of function!", nullptr);
    }
    else
    {
        list_[amount_] = Variable(FUNC_CALL_FIXME, name);
        list_[amount_].extra_data_ = param_amount;
        amount_++;
    }

}



/* -------------------------------------------------------
 * @about
 *       Destructor for Variables list
 *
 /--------------------------------------------------------*/
Var_list::~Var_list()
{
    amount_ = 0;


    if (name_buffer_ != nullptr)
    {
        delete [] name_buffer_;
    }

    if (list_ != nullptr)
    {
        delete [] list_;
        list_ = nullptr;
    }

}




void Var_list::clean_name_buffer()
{
    for(int i = 0; i < MAX_VAR_NAME_LEN; i++)
    {
        name_buffer_[i] = 0;
    }
}


void Var_list::swap(Var_list& that)
{
    std::swap(list_, that.list_);
    std::swap(amount_, that.amount_);
    std::swap(name_buffer_, that.name_buffer_);
}


int Var_list::get_param_amount(const char* name)
{
    if (!is_in(name))
    {
        THROW(SYNTAX_ERROR, "Unknown function!", nullptr);
    }
    else
    {
        for(int i = 0; i < amount_; i++)
        {
            if(strcmp(list_[i].name_, name) == 0)
            {
                return list_[i].extra_data_;
            }
        }
    }
}

/* =================================================================================================
 *
 *                                  struct Command, Implemantation
 *
 *
 * 
 *
 /==================================================================================================*/
Command::Command(enum COMMAND_TYPE type) noexcept
{
    root_ = nullptr;
    type_ = type;
    next_comm_ = nullptr;
    prev_comm_ = nullptr;
}

Command::Command(Node* node) noexcept
{
    root_ = node;
    type_ = TREE;
    next_comm_ = nullptr;
    prev_comm_ = nullptr;
}

Command::Command(enum COMMAND_TYPE type, Command* prev_node) noexcept
{
    type_ = type;
    root_ = nullptr;
    prev_comm_ = prev_node;
    next_comm_ = nullptr;
}

Command:: Command(Node* node, Command* prev_node) noexcept
{
    type_ = TREE;
    root_ = node;
    prev_comm_ = prev_node;
    next_comm_ = nullptr;
}

Command::~Command()
{
    if (this != nullptr)
    {
        delete next_comm_;
        next_comm_ = nullptr;

        if(root_ != nullptr)
        {
            delete root_;
            root_ = nullptr;
        }
    }



}

void Command::print()
{
    if (this != nullptr)
    {
        if ((type_ == TREE) || (type_ == IF_COMMAND) || (type_ == WHILE_COMMAND))
        {
            root_->print();
            printf("\n");
        }

        next_comm_->print();
    }


}



/* -------------------------------------------------------
 * @about
 *       Almost stealing constructor
 *
 /--------------------------------------------------------*/
void Command::re_set(Command *cmd)
{
    root_ = cmd->root_;
    cmd->root_ = nullptr;

    type_ = cmd->type_;
    cmd->type_ = COMMAND_TYPE_DEFAULT;
}


/* -------------------------------------------------------
 * @about
 *       Counts nodes to nullptr
 *
 /--------------------------------------------------------*/
int Command::Get_len()
{
    int i = 0;
    Command* curr_command = this;

    while ((curr_command != nullptr) && (i < 10000))
    {
        i++;
        curr_command = curr_command->next_comm_;
    }

    return i;
}

/* =================================================================================================
 *
 *                                  Commands List struct, Implementation
 *
 *
 *
 /==================================================================================================*/

Function::Function(Command* command, Function* prev_func) noexcept
{
    first_ = command;
    prev_func_ = prev_func;
    next_func_ = nullptr;

}

Function::Function(const char *name) noexcept
{
    int len = strlen(name);

    if(len > MAX_FUNC_NAME_LEN)
    {
        THROW(LOGIC_ERROR, "Too big function name!", nullptr);
    }

    name_ = new char [len];
    strcpy(name_, name);

    func_var_list_ = new Var_list(MIN_VAR_LIST_SIZE);

    param_var_list_ = new Var_list(MIN_VAR_LIST_SIZE);

}


Function::~Function() noexcept
{
    if (this != nullptr)
    {
        delete next_func_;
        next_func_ = nullptr;

        delete first_;
        first_ = nullptr;

        delete [] name_;
        name_ = nullptr;

        delete func_var_list_;
        func_var_list_ = nullptr;

        delete  param_var_list_;
        param_var_list_ = nullptr;
    }
}


void Function::print()
{
    if (this != nullptr)
    {
        printf("function name is - [%s]\n", name_);
        first_->print();
        printf("\n");

        next_func_->print();
    }
}


/* =================================================================================================
 *
 *                                  struct Functions List
 *
 *
 *
 /==================================================================================================*/


/*
Program::~Program()
{

}
*/

#endif //LEXIC_CPP