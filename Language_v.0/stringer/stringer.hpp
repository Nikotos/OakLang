#ifndef STRINGER_HPP
#define STRINGER_HPP

#include "stringer.h"


oak::Stringer::~Stringer()
{
    delete [] heap_;
    delete [] text_;

    heap_ = nullptr;
    text_ = nullptr;
    strings_amount_ = 0;
    symbols_amount_ = 0;
}


void oak::Stringer::Read_file(const char *filename)
{
    FILE* input = fopen(filename,"r");

    if (input == nullptr)
    {
        THROW(UNKNOWN_ERROR, "Can't open File!", nullptr);
    }


    const int file_len = sizeof_file(input);

    heap_ = new char [file_len + 1];

    fread (heap_, file_len, sizeof (char), input);

    strings_amount_ = strings_amount(heap_);

    text_ = new oak::String [strings_amount_];


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
size_t oak::Stringer::sizeof_file(FILE *file)
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
 *        Calculates amount of words in the text
 *
 *
 /--------------------------------------------------------*/

size_t oak::Stringer::strings_amount(const char *heap)
{
    size_t str_am = 1;

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
void oak::Stringer::split_text ()
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











#endif //STRINGER_HPP