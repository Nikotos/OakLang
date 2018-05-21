#ifndef PARSER_CPP
#define PARSER_CPP

#include "Parser.h"



/* -------------------------------------------------------
 * @about
 *        Macros for avoiding spaces
 *
 /--------------------------------------------------------*/
#define SPACE_CORRECTION\
    while (string_[pos_] == ' ')\
    {\
        pos_++;\
    }

/* -------------------------------------------------------
 * @about
 *        Macros check if string_[pos_] is digit
 *
 /--------------------------------------------------------*/
#define IS_DIGIT\
    ((string_[pos_] >= '0') && (string_[pos_] <= '9'))



/* -------------------------------------------------------
 * @about
 *        Macros check if string_[pos_] is Letter
 *
 /--------------------------------------------------------*/
#define IS_LETTER\
    ( ((string_[pos_] >= 'a') && (string_[pos_] <= 'z'))    \
    || ((string_[pos_] >= 'A') && (string_[pos_] <= 'Z'))   \
    || ((string_[pos_] >= 'а') && (string_[pos_] <= 'я'))   \
    || ((string_[pos_] >= 'А') && (string_[pos_] <= 'Я'))  \
    || (string_[pos_] == '_')  )


/* -------------------------------------------------------
 * @about
 *        Macros check if string_[pos_] is closing bracket
 *
 /--------------------------------------------------------*/
#define CHECK_CLOSING_BRACKET\
    if (string_[pos_] == ')')\
    {\
        pos_ ++;\
    }\
\
    else\
    {\
        THROW(SYNTAX_ERROR, "Alone standing [ ( - bracket]", nullptr);\
    }



/*=========================================================================================================
 *
 *
 *                                          Parser Functions
 *
 *
 /==========================================================================================================*/


Parser::Parser()
{
    func_list_ = new Var_list(MIN_VAR_LIST_SIZE);

    /* setting std functions here*/

    func_list_->add_function("input", 0);
    func_list_->add_function("output", 1);

}


Parser::~Parser()
{
    delete [] heap_;
    heap_ = nullptr;

    delete [] text_;
    text_ = nullptr;

    delete func_list_;
    func_list_ = nullptr;

}

/* -------------------------------------------------------
 * @about
 *        Get Variable, worker's slave
 *        We call him, when situation is really bad.....
 *
 /--------------------------------------------------------*/
Node* Parser::GetV()
{
    SPACE_CORRECTION

    /* preparing internal buffer for reading*/
    func_list_->clean_name_buffer();


    for (int i = 0; (IS_LETTER || IS_DIGIT) && (i < MAX_VAR_NAME_LEN); pos_++, i++)
    {
        func_list_->name_buffer_[i] = string_[pos_];
    }

    SPACE_CORRECTION

    /* such explicit form of code for clarity of code*/
    /* (to upgrade later) */
    if(var_list_->is_in(func_list_->name_buffer_))
    {
        return new Node(var_list_->get_var(func_list_->name_buffer_));
    }
    else if (current_function_->param_var_list_->is_in(func_list_->name_buffer_) )
    {
        return new Node(current_function_->param_var_list_->get_var(func_list_->name_buffer_));
    }
    else if(func_list_->is_in(func_list_->name_buffer_))
    {
        if (string_[pos_] != '(')
        {
            THROW(SYNTAX_ERROR, "Missing ( - bracket before function call!", nullptr);
        }

        pos_++;

        int needed_paramount = func_list_->get_param_amount(func_list_->name_buffer_);

        Node* node = new Node(new Func_call(func_list_->name_buffer_, needed_paramount));

        int exact_paramount = Get_func_call(node);

        if (exact_paramount != needed_paramount)
        {
            printf("we have - [%d]\n", exact_paramount);
            printf("expected - [%d]\n",needed_paramount);
            THROW(LOGIC_ERROR, "Invalid amount of parameters in function call!", nullptr);
        }

        CHECK_CLOSING_BRACKET

        SPACE_CORRECTION

        //printf("\n");
        //node->print();
        //printf("\n");

        return node;

    }
    else if (string_[pos_] == '(')
    {
        printf("[%s]", func_list_->name_buffer_ );
        THROW(SYNTAX_ERROR, "Undefined function!", nullptr);
    }
    else
    {
        printf("[%s]", func_list_->name_buffer_ );
        THROW(SYNTAX_ERROR, "Undefined variable!", nullptr);
    }

}


/* -------------------------------------------------------
 * @about
 *        Get Function call parameters
 *        Create sub-tree (right-tree list)
 *        with function and parameters
 *
 * @input
 *        Function Node without parameters
 *
 *
 /--------------------------------------------------------*/
int Parser::Get_func_call(Node *function)
{
    Node* one = nullptr;

    int param_amount = 0;

    while (string_[pos_] != 0)
    {

        if ((string_[pos_] == ')'))
        {
            break;
        }

        if ((string_[pos_] == ','))
        {
            pos_++;
        }

        param_amount++;

        if (one == nullptr)
        {
            function->right_node_ = GetE();
            one = function->right_node_;
        }
        else
        {
            one->extra_node_ = GetE();
            one = one->extra_node_;
        }

    }

    return param_amount;
}



/* -------------------------------------------------------
 * @about
 *        Get Number, usual worker
 *
 /--------------------------------------------------------*/
Node* Parser::GetN()
{
    SPACE_CORRECTION

    if (!IS_DIGIT && IS_LETTER)
    {
        return GetV();
    }

    int val = 0;

    /* Probably more, that one roundabout, because of checking above*/
    while (IS_DIGIT)
    {
        val = 10 * val + (string_[pos_] - '0');
        pos_ ++;
    }

    if (string_[pos_] == '.')
    {
        pos_++;

        double real = 0;
        double multiplier = 0.1;

        while (IS_DIGIT)
        {
            real += multiplier * (string_[pos_] - '0');
            pos_ ++;
            multiplier /= 10;
        }

        real += val;

        SPACE_CORRECTION

        return new Node(real);
    }

    else
    {
        SPACE_CORRECTION

        return new Node(val);
    }

}




/* ---------------------------------------------------------------------
 * @about
 *        Modified 'Get Number', secret agent
 *        Makes 'trash', when notices bracket
 *        (To be honest Calculating expression in brackets)
 *        also, SIN and COS specialist
 *
 *
 /-------------------------------------------------------------------------*/
Node* Parser::GetP()
{
    SPACE_CORRECTION

    if(string_[pos_] == '(')
    {
        pos_ ++;

        Node* current_node = nullptr;
        current_node = GetE();


        CHECK_CLOSING_BRACKET

        SPACE_CORRECTION

        return current_node;
    }

    else if (strncmp(string_ + pos_, "sin(", 4) == 0)
    {
        pos_ += 4;    // "SIN(Expression)" - 4 sym before Expression

        Node* current_node = new Node(SIN);
        current_node->left_node_ = GetE();
        current_node->right_node_ = nullptr;    //explicit form to write unary operator


        CHECK_CLOSING_BRACKET

        SPACE_CORRECTION

        return current_node;
    }
    else if (strncmp(string_ + pos_, "cos(", 4) == 0)
    {
        pos_ += 4;    // "COS(Expression)" - 4 sym before Expression

        Node* current_node = new Node(COS);
        current_node->left_node_ = GetE();
        current_node->right_node_ = nullptr;    //explicit form to write unary operator



        CHECK_CLOSING_BRACKET

        SPACE_CORRECTION

        return current_node;
    }
    else if (strncmp(string_ + pos_, "-(", 1) == 0)
    {
        pos_ += 2;                          // "- (expression in brackets)

        Node* current_node = new Node(COS);
        current_node->left_node_ = GetE();
        current_node->right_node_ = nullptr;    //explicit form to write unary operator



        CHECK_CLOSING_BRACKET

        SPACE_CORRECTION

        return current_node;
    }
    else
    {
        return GetN();
    }
}



/* -------------------------------------------------------
 * @about
 *        one more 'Get Expression', Maestro of '*' and '/'
 *        Petty officer in our project
 *        Calculating expressions (* or /)
 *
 *
 /--------------------------------------------------------*/
Node* Parser::GetT()
{

    Node* current_node = nullptr;
    current_node = GetP();

    while ((string_[pos_] == '*') || (string_[pos_] == '/'))
    {

        Node* node = new Node(OPERATOR_NAMES_DEFAULT);
        node->left_node_ = current_node;

        if (string_[pos_] == '*')
        {
            node->knot_.storage_.oper_ = MULTIPLY;
        }
        else
        {
            node->knot_.storage_.oper_ = DIVIDE;
        }

        pos_++;

        node->right_node_  = GetP();

        current_node = node;

    }

    return current_node;

}

/* -------------------------------------------------------
 * @about
 *        'Get Expression', Maestro of '+' and '-'
 *        Foreman in our project
 *        Calculating expressions (+ or -)
 *
 *
 /--------------------------------------------------------*/
Node* Parser::GetE()
{
    SPACE_CORRECTION

    Node* current_node = nullptr;
    current_node = GetT();

    while ((string_[pos_] == '+') || (string_[pos_] == '-'))
    {

        Node* node = new Node(OPERATOR_NAMES_DEFAULT);
        node->left_node_ = current_node;

        if (string_[pos_] == '+')
        {
            node->knot_.storage_.oper_ = PLUS;
        }
        else
        {
            node->knot_.storage_.oper_ = MINUS;
        }

        pos_++;

        node->right_node_  = GetT();

        current_node = node;

    }

    return current_node;

}


/* -------------------------------------------------------
 * @about
 *        'Get Assignment', Maestro of '='
 *
 *
 *
 /--------------------------------------------------------*/
Node* Parser::GetA()
{
    SPACE_CORRECTION

    Node* current_node = nullptr;

    current_node= GetE();

    if (string_[pos_] == '=')
    {
        pos_++;

        SPACE_CORRECTION

        if ((current_node->knot_.type_ != VARIABLE))
        {
            THROW(SYNTAX_ERROR, "Expression before [=] !", nullptr);
        }

        else
        {
            Node* node = new Node(ASSIGN);
            node->left_node_ = current_node;
            node->right_node_ = GetE();

            return node;
        }


    }
    else if ((current_node->knot_.type_ != CALL) && (current_node->knot_.type_ != ASSIGN))
    {
        FTHROW(LOGIC_ERROR, "Hey dude, useless action!");
    }
    else
    {
        return current_node;
    }

}


/* -------------------------------------------------------
 * @about
 *        'Get Condition', Maestro of '>' '<'
 *
 * @sorry
 *        Sorry for such amount of copy-paste
 *        I was too lazy, when I were writing it
 *
 /--------------------------------------------------------*/
#define DO_TWO_SYMB_STUFF(OPERATION)    \
    pos_ += 2;                          \
    Node* node = new Node(OPERATION);   \
    node->knot_.type_ = CONDITION;      \
    node->left_node_ = current_node;    \
    node->right_node_ = GetE();         \
    return node;

Node* Parser::GetC()
{
    SPACE_CORRECTION

    Node* current_node = nullptr;
    current_node = GetE();

    SPACE_CORRECTION

    if ((strncmp(string_ + pos_, "==", 2) == 0))
    {
        DO_TWO_SYMB_STUFF(IF_EQUAL)
    }

    else if ((strncmp(string_ + pos_, "!=", 2) == 0))
    {
        DO_TWO_SYMB_STUFF(IF_NOT_EQUAL)
    }

    else if ((strncmp(string_ + pos_, ">=", 2) == 0))
    {
        DO_TWO_SYMB_STUFF(IF_EOG)
    }

    else if ((strncmp(string_ + pos_, "<=", 2) == 0))
    {
        DO_TWO_SYMB_STUFF(IF_EOL)
    }

    else if (string_[pos_] == '>')
    {
        pos_--;
        DO_TWO_SYMB_STUFF(IF_GREAT)
    }

    else if (string_[pos_] == '<')
    {
        pos_--;
        DO_TWO_SYMB_STUFF(IF_LESS)
    }

    if ((current_node->knot_.type_ == INT_CONSTANT) && (current_node->knot_.storage_.number_ != 1))
    {
        THROW(SYNTAX_ERROR, "Number instead of Condition!", nullptr);
    }

    return current_node;
}
#undef DO_TWO_SYMB_STUFF

/* -------------------------------------------------------
 * @about
 *        Fills parsing tree
 *
 * @input
 *        pointer at empty tree
 *
 * @output
 *         Parser tree
 *         (almost output, we work with a pointer)
 *
 /--------------------------------------------------------*/
Command* Parser::Get_command()
{
    SPACE_CORRECTION

    if((strncmp(string_ + pos_, "int", 3) == 0))
    {
        pos_ += 3;

        SetVar(INTEGER);

        Command* comm = new Command(VARIABLE_DEFINITION);
        comm->root_ = new Node(var_list_->get_var(func_list_->name_buffer_));

        return comm;

    }
    else if((strncmp(string_ + pos_, "if(", 3) == 0))
    {
        pos_ += 3;

        Node* node = new Node(IF);
        node->left_node_ = GetC();

        auto first = new Command(node);
        first->type_ = IF_COMMAND;

        CHECK_CLOSING_BRACKET

        text_pos_++;

        auto second = Get_action();
        first->root_->right_node_ = new Node(second->Get_len()); // right node is amount of instructions

        first->next_comm_ = second;
        second->prev_comm_ = first;

        return first;
    }
    else if((strncmp(string_ + pos_, "while(", 6) == 0))
    {
        pos_ += 6;

        Node* node = new Node(WHILE);
        node->left_node_ = GetC();

        auto first = new Command(node);
        first->type_ = WHILE_COMMAND;

        CHECK_CLOSING_BRACKET

        text_pos_++;

        auto second = Get_action();
        first->root_->right_node_ = new Node(second->Get_len()); // right node is amount of instructions

        first->next_comm_ = second;
        second->prev_comm_ = first;

        return first;
    }
    else if((strncmp(string_ + pos_, "return", 6) == 0))
    {
        pos_ += 6;

        auto comm = new Command(RETURN);
        comm->root_ = GetE();


        return comm;
    }

    else
    {
        return new Command(GetA());
    }

}



/* -------------------------------------------------------
 * @about
 *        Setting new variable
 *        Adding variable to list of variables to catch some errors
 *
 * @input
 *        type of Variable
 *
 * @output
 *         We add variable with read name to list of variables
 *         (almost output, we work with a pointer)
 *
 /--------------------------------------------------------*/
void Parser::SetVar(enum VAR_TYPES type)
{
    func_list_->clean_name_buffer();

    SPACE_CORRECTION

    for (int i = 0; (IS_LETTER || IS_DIGIT); pos_++, i++)
    {
        func_list_->name_buffer_[i] = string_[pos_];
    }

    SPACE_CORRECTION

    var_list_->add_variable(type, func_list_->name_buffer_);
}




/* -------------------------------------------------------
 * @about
 *        Fills Function with commands
 *
 *
 * @output
 *         Function - list of commands
 *
 /--------------------------------------------------------*/
Command* Parser::Get_action()
{

    Command* one = nullptr;
    Command* two = nullptr;

    if (text_[text_pos_].body_[0] != '{')
    {
        THROW(SYNTAX_ERROR, "Missing bracket - {!", nullptr);
    }
    else
    {
        text_pos_++;
    }

    while ((text_[text_pos_].body_[0] != '}') && (text_pos_ < strings_amount_ - 1))
    {
        if((text_[text_pos_].len_ != 1))
        {
            string_ = text_[text_pos_].body_;
            pos_ = 0;

            one = Get_command();
            one->prev_comm_ = two;

            if (two != nullptr)
            {
                two->next_comm_ = one;
            }


            while(one->next_comm_ != nullptr)
            {
                one = one->next_comm_;
            }

            two = one;
            one = nullptr;


            if ((string_[pos_] != 0) && (string_[pos_] != '}'))
            {
                printf("Incorrect symbol - [%s]!!!\n", string_ + pos_);
                THROW(SYNTAX_ERROR, "Incorrect symbol!", nullptr);
            }

        }


        text_pos_++;
    }


    while ((two->prev_comm_ != nullptr) && (two != nullptr))
    {
        two = two->prev_comm_;
    }

    return two;
}


/* -------------------------------------------------------
 * @about
 *        Fills one function with commands
 *
 *
 * @output
 *         pointer at Function
 *
 /--------------------------------------------------------*/
Function* Parser::Get_function()
{

    Function* one = nullptr;

    string_ = text_[text_pos_].body_;
    pos_ = 0;
    
    if ((text_[text_pos_].len_ > 7) && (strncmp(string_ + pos_, "def_int", 7) == 0))
    {
        pos_ += 8;
        one = new Function(Get_func_name());

        var_list_ = one->param_var_list_;
        one->param_amount_ = Get_func_decl();

        var_list_ = one->func_var_list_;
        func_list_->get_var(one->name_)->extra_data_ = one->param_amount_;

        text_pos_++;

        current_function_ = one;

        one->first_ = Get_action();

    }
    else
    {
        printf("incorrect symbol in this string - [%s]\n", text_[text_pos_]);
        THROW(SYNTAX_ERROR, "FUNCTION without declaration", nullptr);
    }


    return one;
}


/* -------------------------------------------------------
 * @about
 *        Gets function declaration
 *
 *
 * @output
 *         amount of parameters
 *
 /--------------------------------------------------------*/
int Parser::Get_func_decl()
{
    int param_amount = 0;

    while(string_[pos_] != 0)
    {
        SPACE_CORRECTION

        if (string_[pos_] == ',')
        {
            pos_++;
        }
        else if (string_[pos_] == ')')
        {
            break;
        }

        SPACE_CORRECTION

        if((strncmp(string_ + pos_, "int", 3) == 0))
        {
            pos_ += 3;

            SetVar(INTEGER);
            param_amount++;

        }

    }

   return param_amount;
}

/* -------------------------------------------------------
 * @about
 *        Getting function name
 *        Adding it to list with function names
 *
 * @output
 *         pointer at name
 *
 * @note
 *         Throws, if name is incorrect
 *         (too big or contain inappropriate symbols)
 *
 /--------------------------------------------------------*/
char* Parser::Get_func_name()
{
    func_list_->clean_name_buffer();

    int i = 0;

    while(IS_LETTER)
    {
        func_list_->name_buffer_[i] = string_[pos_];
        i++;
        pos_++;
    }

    if (string_[pos_] != '(')
    {
        printf("[%c]\n", string_[pos_]);
        THROW(SYNTAX_ERROR, "Not a letter in function name or Missing bracket!!", nullptr);
    }

    pos_++;

    func_list_->add_variable(FUNC_CALL_FIXME, func_list_->name_buffer_);

    return func_list_->name_buffer_;
}


/* -------------------------------------------------------
 * @about
 *        Fills Program with Functions
 *
 *
 * @output
 *         pointer at list of Functions
 *
 /--------------------------------------------------------*/
Function* Parser::Get_program()
{
    Function* one = nullptr;
    Function* two = nullptr;

    while(text_pos_ < strings_amount_ - 1)
    {
        two = Get_function();
        text_pos_++;

        two->prev_func_ = one;

        if (one != nullptr)
        {
            one->next_func_ = two;
        }

        one = two;
    }

    while ((two->prev_func_ != nullptr) && (two != nullptr))
    {
        two = two->prev_func_;
    }

    return two;
}



/* -------------------------------------------------------
 * @about
 *        Converts text from file to array of strings
 *
 *
 * @input
 *        file with text
 *
 * @output
 *        array of strings (text_)
 *
 /--------------------------------------------------------*/
void Parser::Read_file(const char *filename)
{
    FILE* input = fopen(filename,"r");

    if (input == nullptr)
    {
        THROW(UNKNOWN_ERROR, "Can't open File!", nullptr);
    }


    const int file_len = sizeof_file(input);

    heap_ = new char [file_len + 1];

    fread (heap_, file_len, sizeof (char), input);

    strings_amount_ = str_amount(heap_);

    text_ = new oak::string [strings_amount_];


    /* splits text from heap_ to
     * sentences and puts it into text_*/
    split_text ();

    fclose(input);
}





/* -------------------------------------------------------
 * @about
 *        Calculates amount of bytes in input file
 *
 *
 /--------------------------------------------------------*/
size_t Parser::sizeof_file(FILE *file)
{
    fseek (file, 0, SEEK_END);
    size_t file_length = ftell (file);

    if (file_length < 0)
    {
        THROW(UNKNOWN_ERROR, "Too big FILE! Change int to long int!", nullptr);
    }

    rewind (file);
    return file_length;
}


/* -------------------------------------------------------
 * @about
 *        Calculates amount of strings in text
 *
 *
 /--------------------------------------------------------*/
size_t Parser::str_amount(const char *heap)
{
    int str_am = 1;

    for(int i = 0; heap[i] != '\0'; i++)
    {
        if (heap[i] == '\n')
        {
            str_am++;
        }

    }
    return str_am;
}


/* -------------------------------------------------------
 * @about
 *        splits text to strings
 *        splits heap_ to text_
 *
 * @input
 *        heap with text
 *
 * @output
 *        array of strings
 *
 /--------------------------------------------------------*/
void Parser::split_text ()
{
    int j = 0; //index for array of strings
    text_[j].body_ = heap_;
    j++;

    int i = 0; //index for heap
    while (heap_[i] != '\0')
    {
        if (heap_[i] == '\n')
        {
            heap_[i] = '\0';
            text_[j - 1].len_ = heap_ + i - text_[j - 1].body_ + 1;

            text_[j].body_ = heap_ + i + 1;
            j++;
        }

        i++;
    }
    text_[j - 1].len_ = heap_ + i - text_[j - 1].body_ + 1;
}

#undef IS_DIGIT
#undef IS_LETTER
#undef CHECK_CLOSING_BRACKET

#endif //PARSER_CPP
