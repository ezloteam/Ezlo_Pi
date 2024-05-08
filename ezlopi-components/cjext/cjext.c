/*
  Copyright (c) 2009-2017 Dave Gamble and cJSON contributors

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

/* cJSON */
/* JSON parser in C. */
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <float.h>

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"


typedef struct
{
    const unsigned char* json;
    size_t position;
} error;


/* define our own boolean type */
#ifdef true
#undef true
#endif
#define true ((cJSON_bool)1)

#ifdef false
#undef false
#endif
#define false ((cJSON_bool)0)

/* define isnan and isinf for ANSI C, if in C99 or above, isnan and isinf has been defined in math.h */
#ifndef isinf
#define isinf(d) (isnan((d - d)) && !isnan(d))
#endif
#ifndef isnan
#define isnan(d) (d != d)
#endif

#ifndef NAN
#ifdef _WIN32
#define NAN sqrt(-1.0)
#else
#define NAN 0.0 / 0.0
#endif
#endif

/* strlen of character literals resolved at compile time */
#define static_strlen(string_literal) (sizeof(string_literal) - 1)
static error global_error = { NULL, 0 };


static void __print_cj_object(cJSON * cj_obj)
{
    printf("\r\n");
    TRACE_D("item: %u", (uint32_t)cj_obj);
    if (cj_obj)
    {
        TRACE_D("-- next:           %u", (uint32_t)cj_obj->next);
        TRACE_D("-- prev:           %u", (uint32_t)cj_obj->prev);
        TRACE_D("-- child:          %u", (uint32_t)cj_obj->child);
        TRACE_D("-- type:           %d", cj_obj->type);
        TRACE_D("-- value-string:   %u", (uint32_t)cj_obj->valuestring);
        TRACE_D("-- value-int:      %d", cj_obj->valueint);
        TRACE_D("-- value-double:   %f", cj_obj->valuedouble);
        TRACE_D("-- string:         %u", (uint32_t)cj_obj->string);
        TRACE_D("-- is-key-ref:     %d", cj_obj->is_key_ref);
        TRACE_D("-- is-value-ref:   %d", cj_obj->is_value_ref);
        TRACE_D("-- str-key-len:    %u", (uint32_t)cj_obj->str_key_len);
        TRACE_D("-- str-value-len:  %u", (uint32_t)cj_obj->str_value_len);
    }
}


const char *cJSON_GetErrorPtr(void)
{
    return (const char*)(global_error.json + global_error.position);
}

char *cJSON_GetStringValue(const cJSON* const item)
{
    if (!cJSON_IsString(item))
    {
        return NULL;
    }

    return item->valuestring;
}

double cJSON_GetNumberValue(const cJSON* const item)
{
    if (!cJSON_IsNumber(item))
    {
        return (double)NAN;
    }

    return item->valuedouble;
}

const char *cJSON_Version(void)
{
    static char version[15];
    sprintf(version, "%i.%i.%i", CJSON_VERSION_MAJOR, CJSON_VERSION_MINOR, CJSON_VERSION_PATCH);

    return version;
}

/* Case insensitive string comparison, doesn't consider two NULL pointers equal though */
static int case_insensitive_strcmp(const unsigned char* string1, const unsigned char* string2, size_t item_key_len)
{
    if ((string1 == NULL) || (string2 == NULL))
    {
        return 1;
    }

    if (string1 == string2)
    {
        return 0;
    }

    for (; (tolower(*string1) == tolower(*string2) && --item_key_len); (void)string1++, string2++)
    {
        if (*string1 == '\0')
        {
            return 0;
        }
    }

    return tolower(*string1) - tolower(*string2);
}

// static internal_hooks global_hooks = { internal_malloc, internal_free, internal_realloc };

static unsigned char* cJSON_strdup(const char * who, const unsigned char* string, size_t string_len)
{
    size_t length = 0;
    unsigned char* copy = NULL;

    if (string == NULL && (0 == string_len))
    {
        return NULL;
    }

    length = string_len + 1;
    copy = (unsigned char*)ezlopi_malloc(who, length);
    if (copy == NULL)
    {
        return NULL;
    }

    memcpy(copy, string, length);
    return copy;
}

/* Internal constructor. */
static cJSON* cJSON_New_Item(const char * who)
{
    cJSON* node = (cJSON*)ezlopi_malloc(who, sizeof(cJSON));
    if (node)
    {
        memset(node, '\0', sizeof(cJSON));
    }

    return node;
}


/* Delete a cJSON structure. */
void cJSON_Delete(const char * who, cJSON* item)
{
    cJSON* next = NULL;
    while (item != NULL)
    {
        // __print_cj_object(item);

        next = item->next;
        if (!(item->type & cJSON_IsReference) && (item->child != NULL))
        {
            cJSON_Delete(who, item->child);
        }

        if (!(item->type & cJSON_IsReference) && (item->valuestring != NULL))
        {
            ezlopi_free(who, item->valuestring);
            item->valuestring = NULL;
        }

        if (!(item->type & cJSON_StringIsConst) && (item->string != NULL))
        {
            ezlopi_free(who, item->string);
            item->string = NULL;
        }

        if ((cJSON_False == item->is_key_ref) && (NULL != item->string))
        {
            ezlopi_free(who, item->string);
            item->string = NULL;
        }

        if ((cJSON_False == item->is_value_ref) && (NULL != item->valuestring))
        {
            ezlopi_free(who, item->valuestring);
            item->valuestring = NULL;
        }

        ezlopi_free(who, item);
        item = next;
    }
}

/* get the decimal point character of the current locale */
static unsigned char get_decimal_point(void)
{
#ifdef ENABLE_LOCALES
    struct lconv* lconv = localeconv();
    return (unsigned char)lconv->decimal_point[0];
#else
    return '.';
#endif
}

typedef struct
{
    const unsigned char* content;
    size_t length;
    size_t offset;
    size_t depth; /* How deeply nested (in arrays/objects) is the input at the current offset. */
} parse_buffer;

/* check if the given size is left to read in a given parse buffer (starting with 1) */
#define can_read(buffer, size) ((buffer != NULL) && (((buffer)->offset + size) <= (buffer)->length))
/* check if the buffer can be accessed at the given index (starting with 0) */
#define can_access_at_index(buffer, index) ((buffer != NULL) && (((buffer)->offset + index) < (buffer)->length))
#define cannot_access_at_index(buffer, index) (!can_access_at_index(buffer, index))
/* get a pointer to the buffer at the position */
#define buffer_at_offset(buffer) ((buffer)->content + (buffer)->offset)

/* Parse the input text to generate a number, and populate the result into item. */
static cJSON_bool parse_number(cJSON* const item, parse_buffer* const input_buffer)
{
    double number = 0;
    unsigned char* after_end = NULL;
    unsigned char number_c_string[64];
    unsigned char decimal_point = get_decimal_point();
    size_t i = 0;

    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return false;
    }

    /* copy the number into a temporary buffer and replace '.' with the decimal point
     * of the current locale (for strtod)
     * This also takes care of '\0' not necessarily being available for marking the end of the input */
    for (i = 0; (i < (sizeof(number_c_string) - 1)) && can_access_at_index(input_buffer, i); i++)
    {
        switch (buffer_at_offset(input_buffer)[i])
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '+':
        case '-':
        case 'e':
        case 'E':
            number_c_string[i] = buffer_at_offset(input_buffer)[i];
            break;

        case '.':
            number_c_string[i] = decimal_point;
            break;

        default:
            goto loop_end;
        }
    }
loop_end:
    number_c_string[i] = '\0';

    number = strtod((const char*)number_c_string, (char**)&after_end);
    if (number_c_string == after_end)
    {
        return false; /* parse_error */
    }

    item->valuedouble = number;

    /* use saturation in case of overflow */
    if (number >= INT_MAX)
    {
        item->valueint = INT_MAX;
    }
    else if (number <= (double)INT_MIN)
    {
        item->valueint = INT_MIN;
    }
    else
    {
        item->valueint = (int)number;
    }

    item->type = cJSON_Number;

    input_buffer->offset += (size_t)(after_end - number_c_string);
    return true;
}

/* don't ask me, but the original cJSON_SetNumberValue returns an integer or double */
double cJSON_SetNumberHelper(cJSON* object, double number)
{
    if (number >= INT_MAX)
    {
        object->valueint = INT_MAX;
    }
    else if (number <= (double)INT_MIN)
    {
        object->valueint = INT_MIN;
    }
    else
    {
        object->valueint = (int)number;
    }

    return object->valuedouble = number;
}

char *cJSON_SetValuestring(const char * who, cJSON* object, const char* valuestring)
{
    /* if object's type is not cJSON_String or is cJSON_IsReference, it should not set valuestring */
    if ((object == NULL) || !(object->type & cJSON_String) || (object->type & cJSON_IsReference))
    {
        return NULL;
    }

    /* return NULL if the object is corrupted */
    if (object->valuestring == NULL)
    {
        return NULL;
    }

    object->str_value_len = strlen(valuestring);

    if (object->str_value_len <= strlen(object->valuestring))
    {
        strcpy(object->valuestring, valuestring);
        return object->valuestring;
    }

    char* copy = (char*)cJSON_strdup(who, (const unsigned char*)valuestring, object->str_value_len);
    if (copy == NULL)
    {
        return NULL;
    }

    if (object->valuestring != NULL)
    {
        ezlopi_free(who, object->valuestring);
    }

    object->valuestring = copy;
    object->is_value_ref = cJSON_False;

    return copy;
}

typedef struct
{
    unsigned char* buffer;
    size_t length;
    size_t offset;
    size_t depth; /* current nesting depth (for formatted printing) */
    cJSON_bool noalloc;
    cJSON_bool format; /* is this print a formatted print */
} printbuffer;

/* realloc printbuffer if necessary to have at least "needed" bytes more */
static unsigned char* ensure(const char * who, printbuffer* const p, size_t needed)
{
    unsigned char* newbuffer = NULL;
    size_t newsize = 0;

    if ((p == NULL) || (p->buffer == NULL))
    {
        return NULL;
    }

    if ((p->length > 0) && (p->offset >= p->length))
    {
        /* make sure that offset is valid */
        return NULL;
    }

    if (needed > INT_MAX)
    {
        /* sizes bigger than INT_MAX are currently not supported */
        return NULL;
    }

    needed += p->offset + 1;
    if (needed <= p->length)
    {
        return p->buffer + p->offset;
    }

    if (p->noalloc)
    {
        return NULL;
    }

    /* calculate new buffer size */
    if (needed > (INT_MAX / 2))
    {
        /* overflow of int, use INT_MAX if possible */
        if (needed <= INT_MAX)
        {
            newsize = INT_MAX;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        newsize = needed * 2;
    }

    /* reallocate with realloc if available */
    newbuffer = (unsigned char*)ezlopi_realloc(who, p->buffer, newsize);
    if (newbuffer == NULL)
    {
        ezlopi_free(who, p->buffer);
        p->length = 0;
        p->buffer = NULL;

        return NULL;
    }

    p->length = newsize;
    p->buffer = newbuffer;

    return newbuffer + p->offset;
}

/* calculate the new length of the string in a printbuffer and update the offset */
static void update_offset(printbuffer* const buffer)
{
    const unsigned char* buffer_pointer = NULL;
    if ((buffer == NULL) || (buffer->buffer == NULL))
    {
        return;
    }
    buffer_pointer = buffer->buffer + buffer->offset;

    buffer->offset += strlen((const char*)buffer_pointer);
}

/* securely comparison of floating-point variables */
static cJSON_bool compare_double(double a, double b)
{
    double maxVal = fabs(a) > fabs(b) ? fabs(a) : fabs(b);
    return (fabs(a - b) <= maxVal * DBL_EPSILON);
}

/* Render the number nicely from the given item into a string. */
static cJSON_bool print_number(const char * who, const cJSON* const item, printbuffer* const output_buffer)
{
    unsigned char* output_pointer = NULL;
    double d = item->valuedouble;
    int length = 0;
    size_t i = 0;
    unsigned char number_buffer[26] = { 0 }; /* temporary buffer to print the number into */
    unsigned char decimal_point = get_decimal_point();
    double test = 0.0;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* This checks for NaN and Infinity */
    if (isnan(d) || isinf(d))
    {
        length = sprintf((char*)number_buffer, "null");
    }
    else if (d == (double)item->valueint)
    {
        length = sprintf((char*)number_buffer, "%d", item->valueint);
    }
    else
    {
        /* Try 15 decimal places of precision to avoid nonsignificant nonzero digits */
        length = sprintf((char*)number_buffer, "%1.15g", d);

        /* Check whether the original double can be recovered */
        if ((sscanf((char*)number_buffer, "%lg", &test) != 1) || !compare_double((double)test, d))
        {
            /* If not, print with 17 decimal places of precision */
            length = sprintf((char*)number_buffer, "%1.17g", d);
        }
    }

    /* sprintf failed or buffer overrun occurred */
    if ((length < 0) || (length > (int)(sizeof(number_buffer) - 1)))
    {
        return false;
    }

    /* reserve appropriate space in the output */
    output_pointer = ensure(who, output_buffer, (size_t)length + 1);
    if (output_pointer == NULL)
    {
        return false;
    }

    /* copy the printed number to the output and replace locale
     * dependent decimal point with '.' */
    for (i = 0; i < ((size_t)length); i++)
    {
        if (number_buffer[i] == decimal_point)
        {
            output_pointer[i] = '.';
            continue;
        }

        output_pointer[i] = number_buffer[i];
    }
    output_pointer[i] = '\0';

    output_buffer->offset += (size_t)length;

    return true;
}

/* parse 4 digit hexadecimal number */
static unsigned parse_hex4(const unsigned char* const input)
{
    unsigned int h = 0;
    size_t i = 0;

    for (i = 0; i < 4; i++)
    {
        /* parse digit */
        if ((input[i] >= '0') && (input[i] <= '9'))
        {
            h += (unsigned int)input[i] - '0';
        }
        else if ((input[i] >= 'A') && (input[i] <= 'F'))
        {
            h += (unsigned int)10 + input[i] - 'A';
        }
        else if ((input[i] >= 'a') && (input[i] <= 'f'))
        {
            h += (unsigned int)10 + input[i] - 'a';
        }
        else /* invalid */
        {
            return 0;
        }

        if (i < 3)
        {
            /* shift left to make place for the next nibble */
            h = h << 4;
        }
    }

    return h;
}

/* converts a UTF-16 literal to UTF-8
 * A literal can be one or two sequences of the form \uXXXX */
static unsigned char utf16_literal_to_utf8(const unsigned char* const input_pointer, const unsigned char* const input_end, unsigned char** output_pointer)
{
    long unsigned int codepoint = 0;
    unsigned int first_code = 0;
    const unsigned char* first_sequence = input_pointer;
    unsigned char utf8_length = 0;
    unsigned char utf8_position = 0;
    unsigned char sequence_length = 0;
    unsigned char first_byte_mark = 0;

    if ((input_end - first_sequence) < 6)
    {
        /* input ends unexpectedly */
        goto fail;
    }

    /* get the first utf16 sequence */
    first_code = parse_hex4(first_sequence + 2);

    /* check that the code is valid */
    if (((first_code >= 0xDC00) && (first_code <= 0xDFFF)))
    {
        goto fail;
    }

    /* UTF16 surrogate pair */
    if ((first_code >= 0xD800) && (first_code <= 0xDBFF))
    {
        const unsigned char* second_sequence = first_sequence + 6;
        unsigned int second_code = 0;
        sequence_length = 12; /* \uXXXX\uXXXX */

        if ((input_end - second_sequence) < 6)
        {
            /* input ends unexpectedly */
            goto fail;
        }

        if ((second_sequence[0] != '\\') || (second_sequence[1] != 'u'))
        {
            /* missing second half of the surrogate pair */
            goto fail;
        }

        /* get the second utf16 sequence */
        second_code = parse_hex4(second_sequence + 2);
        /* check that the code is valid */
        if ((second_code < 0xDC00) || (second_code > 0xDFFF))
        {
            /* invalid second half of the surrogate pair */
            goto fail;
        }

        /* calculate the unicode codepoint from the surrogate pair */
        codepoint = 0x10000 + (((first_code & 0x3FF) << 10) | (second_code & 0x3FF));
    }
    else
    {
        sequence_length = 6; /* \uXXXX */
        codepoint = first_code;
    }

    /* encode as UTF-8
     * takes at maximum 4 bytes to encode:
     * 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx */
    if (codepoint < 0x80)
    {
        /* normal ascii, encoding 0xxxxxxx */
        utf8_length = 1;
    }
    else if (codepoint < 0x800)
    {
        /* two bytes, encoding 110xxxxx 10xxxxxx */
        utf8_length = 2;
        first_byte_mark = 0xC0; /* 11000000 */
    }
    else if (codepoint < 0x10000)
    {
        /* three bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx */
        utf8_length = 3;
        first_byte_mark = 0xE0; /* 11100000 */
    }
    else if (codepoint <= 0x10FFFF)
    {
        /* four bytes, encoding 1110xxxx 10xxxxxx 10xxxxxx 10xxxxxx */
        utf8_length = 4;
        first_byte_mark = 0xF0; /* 11110000 */
    }
    else
    {
        /* invalid unicode codepoint */
        goto fail;
    }

    /* encode as utf8 */
    for (utf8_position = (unsigned char)(utf8_length - 1); utf8_position > 0; utf8_position--)
    {
        /* 10xxxxxx */
        (*output_pointer)[utf8_position] = (unsigned char)((codepoint | 0x80) & 0xBF);
        codepoint >>= 6;
    }
    /* encode first byte */
    if (utf8_length > 1)
    {
        (*output_pointer)[0] = (unsigned char)((codepoint | first_byte_mark) & 0xFF);
    }
    else
    {
        (*output_pointer)[0] = (unsigned char)(codepoint & 0x7F);
    }

    *output_pointer += utf8_length;

    return sequence_length;

fail:
    return 0;
}

/* Parse the input text into an unescaped cinput, and populate item. */
static cJSON_bool parse_string(const char * who, cJSON* const item, parse_buffer* const input_buffer)
{
    const unsigned char* input_pointer = buffer_at_offset(input_buffer) + 1;
    const unsigned char* input_end = buffer_at_offset(input_buffer) + 1;
    unsigned char* output_pointer = NULL;
    unsigned char* output = NULL;

    /* not a string */
    if (buffer_at_offset(input_buffer)[0] != '\"')
    {
        goto fail;
    }

    size_t allocation_length = 0;

    {
        /* calculate approximate size of the output (overestimate) */
        size_t skipped_bytes = 0;
        while (((size_t)(input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"'))
        {
            /* is escape sequence */
            if (input_end[0] == '\\')
            {
                if ((size_t)(input_end + 1 - input_buffer->content) >= input_buffer->length)
                {
                    /* prevent buffer overflow when last input character is a backslash */
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }
        if (((size_t)(input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"'))
        {
            goto fail; /* string ended unexpectedly */
        }

        /* This is at most how much we need for the output */
        allocation_length = (size_t)(input_end - buffer_at_offset(input_buffer)) - skipped_bytes;
        item->str_value_len = allocation_length;
        output = (unsigned char*)ezlopi_malloc(who, allocation_length + 1);
        if (output == NULL)
        {
            goto fail; /* allocation failure */
        }
    }

    output_pointer = output;
    /* loop through the string literal */
    while (input_pointer < input_end)
    {
        if (*input_pointer != '\\')
        {
            *output_pointer++ = *input_pointer++;
        }
        /* escape sequence */
        else
        {
            unsigned char sequence_length = 2;
            if ((input_end - input_pointer) < 1)
            {
                goto fail;
            }

            switch (input_pointer[1])
            {
            case 'b':
                *output_pointer++ = '\b';
                break;
            case 'f':
                *output_pointer++ = '\f';
                break;
            case 'n':
                *output_pointer++ = '\n';
                break;
            case 'r':
                *output_pointer++ = '\r';
                break;
            case 't':
                *output_pointer++ = '\t';
                break;
            case '\"':
            case '\\':
            case '/':
                *output_pointer++ = input_pointer[1];
                break;

                /* UTF-16 literal */
            case 'u':
                sequence_length = utf16_literal_to_utf8(input_pointer, input_end, &output_pointer);
                if (sequence_length == 0)
                {
                    /* failed to convert UTF16-literal to UTF-8 */
                    goto fail;
                }
                break;

            default:
                goto fail;
            }
            input_pointer += sequence_length;
        }
    }

    /* zero terminate the output */
    *output_pointer = '\0';

    item->type = cJSON_String;
    item->is_value_ref = cJSON_False;
    item->valuestring = (char*)output;

    input_buffer->offset = (size_t)(input_end - input_buffer->content);
    input_buffer->offset++;

    return true;

fail:
    if (output != NULL)
    {
        ezlopi_free(who, output);
    }

    if (input_pointer != NULL)
    {
        input_buffer->offset = (size_t)(input_pointer - input_buffer->content);
    }

    return false;
}

/* Render the cstring provided to an escaped version that can be printed. */
static cJSON_bool print_string_ptr(const char * who, const unsigned char* const input, size_t input_len, printbuffer* const output_buffer)
{
    const unsigned char* input_pointer = NULL;
    unsigned char* output = NULL;
    unsigned char* output_pointer = NULL;
    size_t output_length = 0;
    /* numbers of additional characters needed for escaping */
    size_t escape_characters = 0;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* empty string */
    if (input == NULL)
    {
        output = ensure(who, output_buffer, sizeof("\"\""));
        if (output == NULL)
        {
            return false;
        }
        strcpy((char*)output, "\"\"");

        return true;
    }

    /* set "flag" to 1 if something needs to be escaped */
    for (input_pointer = input; (*input_pointer && input_len--); input_pointer++)
    {
        switch (*input_pointer)
        {
        case '\"':
        case '\\':
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
            /* one character escape sequence */
            escape_characters++;
            break;
        default:
            if (*input_pointer < 32)
            {
                /* UTF-16 escape sequence uXXXX */
                escape_characters += 5;
            }
            break;
        }
    }
    output_length = (size_t)(input_pointer - input) + escape_characters;

    output = ensure(who, output_buffer, output_length + sizeof("\"\""));
    if (output == NULL)
    {
        return false;
    }

    /* no characters have to be escaped */
    if (escape_characters == 0)
    {
        output[0] = '\"';
        memcpy(output + 1, input, output_length);
        output[output_length + 1] = '\"';
        output[output_length + 2] = '\0';

        return true;
    }

    output[0] = '\"';
    output_pointer = output + 1;
    /* copy the string */
    for (input_pointer = input; *input_pointer != '\0'; (void)input_pointer++, output_pointer++)
    {
        if ((*input_pointer > 31) && (*input_pointer != '\"') && (*input_pointer != '\\'))
        {
            /* normal character, copy */
            *output_pointer = *input_pointer;
        }
        else
        {
            /* character needs to be escaped */
            *output_pointer++ = '\\';
            switch (*input_pointer)
            {
            case '\\':
                *output_pointer = '\\';
                break;
            case '\"':
                *output_pointer = '\"';
                break;
            case '\b':
                *output_pointer = 'b';
                break;
            case '\f':
                *output_pointer = 'f';
                break;
            case '\n':
                *output_pointer = 'n';
                break;
            case '\r':
                *output_pointer = 'r';
                break;
            case '\t':
                *output_pointer = 't';
                break;
            default:
                /* escape and print as unicode codepoint */
                sprintf((char*)output_pointer, "u%04x", *input_pointer);
                output_pointer += 4;
                break;
            }
        }
    }
    output[output_length + 1] = '\"';
    output[output_length + 2] = '\0';

    return true;
}

/* Invoke print_string_ptr (which is useful) on an item. */
static cJSON_bool print_string(const char * who, const cJSON* const item, printbuffer* const p)
{
    return print_string_ptr(who, (unsigned char*)item->valuestring, item->str_value_len, p);
}

/* Predeclare these prototypes. */
static cJSON_bool parse_value(const char * who, cJSON* const item, parse_buffer* const input_buffer);
static cJSON_bool print_value(const char * who, const cJSON* const item, printbuffer* const output_buffer);
static cJSON_bool parse_array(const char * who, cJSON* const item, parse_buffer* const input_buffer);
static cJSON_bool print_array(const char * who, const cJSON* const item, printbuffer* const output_buffer);
static cJSON_bool parse_object(const char *who, cJSON* const item, parse_buffer* const input_buffer);
static cJSON_bool print_object(const char * who, const cJSON* const item, printbuffer* const output_buffer);

static cJSON_bool parse_object_with_ref(const char * who, cJSON* const item, parse_buffer* const input_buffer);
static cJSON_bool parse_string_with_ref(cJSON* const item, parse_buffer* const input_buffer);
static cJSON_bool parse_value_with_ref(const char * who, cJSON* const item, parse_buffer* const input_buffer);
static cJSON_bool parse_array_with_ref(const char * who, cJSON* const item, parse_buffer* const input_buffer);

/* Utility to jump whitespace and cr/lf */
static parse_buffer* buffer_skip_whitespace(parse_buffer* const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL))
    {
        return NULL;
    }

    if (cannot_access_at_index(buffer, 0))
    {
        return buffer;
    }

    while (can_access_at_index(buffer, 0) && (buffer_at_offset(buffer)[0] <= 32))
    {
        buffer->offset++;
    }

    if (buffer->offset == buffer->length)
    {
        buffer->offset--;
    }

    return buffer;
}

/* skip the UTF-8 BOM (byte order mark) if it is at the beginning of a buffer */
static parse_buffer* skip_utf8_bom(parse_buffer* const buffer)
{
    if ((buffer == NULL) || (buffer->content == NULL) || (buffer->offset != 0))
    {
        return NULL;
    }

    if (can_access_at_index(buffer, 4) && (strncmp((const char*)buffer_at_offset(buffer), "\xEF\xBB\xBF", 3) == 0))
    {
        buffer->offset += 3;
    }

    return buffer;
}

cJSON *cJSON_ParseWithOpts(const char * who, const char* value, const char** return_parse_end, cJSON_bool require_null_terminated)
{
    size_t buffer_length;

    if (NULL == value)
    {
        return NULL;
    }

    /* Adding null character size due to require_null_terminated. */
    buffer_length = strlen(value) + 1;

    return cJSON_ParseWithLengthOpts(who, value, buffer_length, return_parse_end, require_null_terminated);
}

/* Parse an object - create a new root, and populate. */
cJSON *cJSON_ParseWithLengthOpts(const char * who, const char* value, size_t buffer_length, const char** return_parse_end, cJSON_bool require_null_terminated)
{
    parse_buffer buffer = { 0, 0, 0, 0 };
    cJSON* item = NULL;

    /* reset error position */
    global_error.json = NULL;
    global_error.position = 0;

    if (value == NULL || 0 == buffer_length)
    {
        goto fail;
    }

    buffer.content = (const unsigned char*)value;
    buffer.length = buffer_length;
    buffer.offset = 0;

    item = cJSON_New_Item(who);
    if (item == NULL) /* memory fail */
    {
        goto fail;
    }

    if (!parse_value(who, item, buffer_skip_whitespace(skip_utf8_bom(&buffer))))
    {
        /* parse failure. ep is set. */
        goto fail;
    }

    /* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
    if (require_null_terminated)
    {
        buffer_skip_whitespace(&buffer);
        if ((buffer.offset >= buffer.length) || buffer_at_offset(&buffer)[0] != '\0')
        {
            goto fail;
        }
    }
    if (return_parse_end)
    {
        *return_parse_end = (const char*)buffer_at_offset(&buffer);
    }

    return item;

fail:
    if (item != NULL)
    {
        cJSON_Delete(who, item);
    }

    if (value != NULL)
    {
        error local_error;
        local_error.json = (const unsigned char*)value;
        local_error.position = 0;

        if (buffer.offset < buffer.length)
        {
            local_error.position = buffer.offset;
        }
        else if (buffer.length > 0)
        {
            local_error.position = buffer.length - 1;
        }

        if (return_parse_end != NULL)
        {
            *return_parse_end = (const char*)local_error.json + local_error.position;
        }

        global_error = local_error;
    }

    return NULL;
}

/* Default options for cJSON_Parse */
cJSON *cJSON_Parse(const char * who, const char* value)
{
    return cJSON_ParseWithOpts(who, value, 0, 0);
}

cJSON *cJSON_ParseWithRefOpts(const char * who, const char* value, const char** return_parse_end, cJSON_bool require_null_terminated)
{
    size_t buffer_length;

    if (NULL == value)
    {
        return NULL;
    }

    /* Adding null character size due to require_null_terminated. */
    buffer_length = strlen(value) + 1;

    return cJSON_ParseWithRefWithLengthOpts(who, value, buffer_length, return_parse_end, require_null_terminated);
}

static cJSON_bool parse_string_with_ref(cJSON* const item, parse_buffer* const input_buffer)
{
    const unsigned char* input_pointer = buffer_at_offset(input_buffer) + 1;
    const unsigned char* input_end = buffer_at_offset(input_buffer) + 1;
    /* unsigned char *output_pointer = NULL; */
    /* unsigned char *output = NULL; */

    /* not a string */
    if (buffer_at_offset(input_buffer)[0] != '\"')
    {
        goto fail;
    }

    {
        /* calculate approximate size of the output (overestimate) */
        size_t skipped_bytes = 0;
        while (((size_t)(input_end - input_buffer->content) < input_buffer->length) && (*input_end != '\"'))
        {
            /* is escape sequence */
            if (input_end[0] == '\\')
            {
                if ((size_t)(input_end + 1 - input_buffer->content) >= input_buffer->length)
                {
                    /* prevent buffer overflow when last input character is a backslash */
                    goto fail;
                }
                skipped_bytes++;
                input_end++;
            }
            input_end++;
        }

        if (((size_t)(input_end - input_buffer->content) >= input_buffer->length) || (*input_end != '\"'))
        {
            goto fail; /* string ended unexpectedly */
        }

        item->is_value_ref = cJSON_True;
        item->type = cJSON_String | cJSON_IsReference;
        item->str_value_len = (size_t)(input_end - buffer_at_offset(input_buffer)) - skipped_bytes - 1;
        item->valuestring = (char*)buffer_at_offset(input_buffer) + 1;

        input_buffer->offset = (size_t)(input_end - input_buffer->content);
        input_buffer->offset++;
    }

    return true;

fail:
    if (input_pointer != NULL)
    {
        input_buffer->offset = (size_t)(input_pointer - input_buffer->content);
    }

    return false;
}

static cJSON_bool parse_value_with_ref(const char * who, cJSON* const item, parse_buffer* const input_buffer)
{
    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return false; /* no input */
    }

    /* parse the different types of values */
    /* null */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "null", 4) == 0))
    {
        item->type = cJSON_NULL;
        input_buffer->offset += 4;
        return true;
    }
    /* false */
    if (can_read(input_buffer, 5) && (strncmp((const char*)buffer_at_offset(input_buffer), "false", 5) == 0))
    {
        item->type = cJSON_False;
        input_buffer->offset += 5;
        return true;
    }
    /* true */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "true", 4) == 0))
    {
        item->type = cJSON_True;
        item->valueint = 1;
        input_buffer->offset += 4;
        return true;
    }
    /* string */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"'))
    {
        return parse_string_with_ref(item, input_buffer);
    }
    /* number */
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9'))))
    {
        return parse_number(item, input_buffer);
    }
    /* array */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '['))
    {
        return parse_array_with_ref(who, item, input_buffer);
    }
    /* object */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{'))
    {
        return parse_object_with_ref(who, item, input_buffer);
    }

    return false;
}

cJSON *cJSON_ParseWithRefWithLengthOpts(const char * who, const char* value, size_t buffer_length, const char** return_parse_end, cJSON_bool require_null_terminated)
{
    parse_buffer buffer = { 0, 0, 0, 0 };
    cJSON* item = NULL;

    /* reset error position */
    global_error.json = NULL;
    global_error.position = 0;

    if (value == NULL || 0 == buffer_length)
    {
        goto fail;
    }

    buffer.content = (const unsigned char*)value;
    buffer.length = buffer_length;
    buffer.offset = 0;

    item = cJSON_New_Item(who);
    if (item == NULL) /* memory fail */
    {
        goto fail;
    }

    if (!parse_value_with_ref(who, item, buffer_skip_whitespace(skip_utf8_bom(&buffer))))
    {
        /* parse failure. ep is set. */
        goto fail;
    }

    /* if we require null-terminated JSON without appended garbage, skip and then check for a null terminator */
    if (require_null_terminated)
    {
        buffer_skip_whitespace(&buffer);
        if ((buffer.offset >= buffer.length) || buffer_at_offset(&buffer)[0] != '\0')
        {
            goto fail;
        }
    }

    if (return_parse_end)
    {
        *return_parse_end = (const char*)buffer_at_offset(&buffer);
    }

    return item;

fail:
    if (item != NULL)
    {
        cJSON_Delete(who, item);
    }

    if (value != NULL)
    {
        error local_error;
        local_error.json = (const unsigned char*)value;
        local_error.position = 0;

        if (buffer.offset < buffer.length)
        {
            local_error.position = buffer.offset;
        }
        else if (buffer.length > 0)
        {
            local_error.position = buffer.length - 1;
        }

        if (return_parse_end != NULL)
        {
            *return_parse_end = (const char*)local_error.json + local_error.position;
        }

        global_error = local_error;
    }

    return NULL;
}

cJSON *cJSON_ParseWithRef(const char * who, const char* value)
{
    return cJSON_ParseWithRefOpts(who, value, 0, 0);
}

cJSON *cJSON_ParseWithRefWithLength(const char * who, const char* value, size_t buffer_length)
{
    return cJSON_ParseWithRefWithLengthOpts(who, value, buffer_length, 0, 0);
}

cJSON *cJSON_ParseWithLength(const char * who, const char* value, size_t buffer_length)
{
    return cJSON_ParseWithLengthOpts(who, value, buffer_length, 0, 0);
}

#define cjson_min(a, b) (((a) < (b)) ? (a) : (b))

static unsigned char* print(const char * who, const cJSON* const item, cJSON_bool format)
{
    static const size_t default_buffer_size = 256;
    printbuffer buffer[1];
    unsigned char* printed = NULL;

    memset(buffer, 0, sizeof(buffer));

    /* create buffer */
    buffer->buffer = (unsigned char*)ezlopi_malloc(who, default_buffer_size);
    buffer->length = default_buffer_size;
    buffer->format = format;

    if (buffer->buffer == NULL)
    {
        goto fail;
    }

    memset(buffer->buffer, 0, default_buffer_size);

    /* print the value */
    if (!print_value(who, item, buffer))
    {
        goto fail;
    }
    update_offset(buffer);


    printed = (unsigned char*)ezlopi_realloc(who, buffer->buffer, buffer->offset + 1);
    if (printed == NULL)
    {
        goto fail;
    }
    buffer->buffer = NULL;

    return printed;

fail:
    if (buffer->buffer != NULL)
    {
        ezlopi_free(who, buffer->buffer);
    }

    if (printed != NULL)
    {
        ezlopi_free(who, printed);
    }

    return NULL;
}

/* Render a cJSON item/entity/structure to text. */
char *cJSON_Print(const char * who, const cJSON* item)
{
    return (char*)print(who, item, true);
}

char *cJSON_PrintUnformatted(const char *who, const cJSON* item)
{
    return (char*)print(who, item, false);
}

char *cJSON_PrintBuffered(const char * who, const cJSON* item, int prebuffer, cJSON_bool fmt)
{
    printbuffer p = { 0, 0, 0, 0, 0, 0 };

    if (prebuffer < 0)
    {
        return NULL;
    }

    p.buffer = (unsigned char*)ezlopi_malloc(who, (size_t)prebuffer);
    if (!p.buffer)
    {
        return NULL;
    }

    p.length = (size_t)prebuffer;
    p.offset = 0;
    p.noalloc = false;
    p.format = fmt;

    if (!print_value(who, item, &p))
    {
        ezlopi_free(who, p.buffer);
        return NULL;
    }

    return (char*)p.buffer;
}

cJSON_bool cJSON_PrintPreallocated(const char * who, cJSON* item, char* buffer, const int length, const cJSON_bool format)
{
    printbuffer p = { 0, 0, 0, 0, 0, 0 };

    if ((length < 0) || (buffer == NULL))
    {
        return false;
    }

    p.buffer = (unsigned char*)buffer;
    p.length = (size_t)length;
    p.offset = 0;
    p.noalloc = true;
    p.format = format;

    return print_value(who, item, &p);
}

/* Parser core - when encountering text, process appropriately. */
static cJSON_bool parse_value(const char * who, cJSON* const item, parse_buffer* const input_buffer)
{
    if ((input_buffer == NULL) || (input_buffer->content == NULL))
    {
        return false; /* no input */
    }

    /* parse the different types of values */
    /* null */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "null", 4) == 0))
    {
        item->type = cJSON_NULL;
        input_buffer->offset += 4;
        return true;
    }
    /* false */
    if (can_read(input_buffer, 5) && (strncmp((const char*)buffer_at_offset(input_buffer), "false", 5) == 0))
    {
        item->type = cJSON_False;
        input_buffer->offset += 5;
        return true;
    }
    /* true */
    if (can_read(input_buffer, 4) && (strncmp((const char*)buffer_at_offset(input_buffer), "true", 4) == 0))
    {
        item->type = cJSON_True;
        item->valueint = 1;
        input_buffer->offset += 4;
        return true;
    }
    /* string */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '\"'))
    {
        return parse_string(who, item, input_buffer);
    }
    /* number */
    if (can_access_at_index(input_buffer, 0) && ((buffer_at_offset(input_buffer)[0] == '-') || ((buffer_at_offset(input_buffer)[0] >= '0') && (buffer_at_offset(input_buffer)[0] <= '9'))))
    {
        return parse_number(item, input_buffer);
    }
    /* array */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '['))
    {
        return parse_array(who, item, input_buffer);
    }
    /* object */
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '{'))
    {
        return parse_object(who, item, input_buffer);
    }

    return false;
}

/* Render a value to text. */
static cJSON_bool print_value(const char * who, const cJSON* const item, printbuffer* const output_buffer)
{
    unsigned char* output = NULL;

    if ((item == NULL) || (output_buffer == NULL))
    {
        return false;
    }

    switch ((item->type) & 0xFF)
    {
    case cJSON_NULL:
        output = ensure(who, output_buffer, 5);
        if (output == NULL)
        {
            return false;
        }
        strcpy((char*)output, "null");
        return true;

    case cJSON_False:
        output = ensure(who, output_buffer, 6);
        if (output == NULL)
        {
            return false;
        }
        strcpy((char*)output, "false");
        return true;

    case cJSON_True:
        output = ensure(who, output_buffer, 5);
        if (output == NULL)
        {
            return false;
        }
        strcpy((char*)output, "true");
        return true;

    case cJSON_Number:
        return print_number(who, item, output_buffer);

    case cJSON_Raw:
    {
        size_t raw_length = 0;
        if (item->valuestring == NULL)
        {
            return false;
        }

        raw_length = item->str_value_len + 1;
        output = ensure(who, output_buffer, raw_length);
        if (output == NULL)
        {
            return false;
        }
        memcpy(output, item->valuestring, raw_length);
        return true;
    }

    case cJSON_String:
    {
        return print_string(who, item, output_buffer);
    }

    case cJSON_Array:
    {
        return print_array(who, item, output_buffer);
    }

    case cJSON_Object:
    {
        return print_object(who, item, output_buffer);
    }

    default:
    {
        return false;
    }
    }
}

/* Build an array from input text. */
static cJSON_bool parse_array(const char * who, cJSON* const item, parse_buffer* const input_buffer)
{
    cJSON* head = NULL; /* head of the linked list */
    cJSON* current_item = NULL;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT)
    {
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[')
    {
        /* not an array */
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']'))
    {
        /* empty array */
        goto success;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        cJSON* new_item = cJSON_New_Item(who);
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse next value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(who, current_item, input_buffer))
        {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']')
    {
        goto fail; /* expected end of array */
    }

success:
    input_buffer->depth--;

    if (head != NULL)
    {
        head->prev = current_item;
    }

    item->type = cJSON_Array;
    item->child = head;

    input_buffer->offset++;

    return true;

fail:
    if (head != NULL)
    {
        cJSON_Delete(who, head);
    }

    return false;
}

/* Build an array from input text. */
static cJSON_bool parse_array_with_ref(const char * who, cJSON* const item, parse_buffer* const input_buffer)
{
    cJSON* head = NULL; /* head of the linked list */
    cJSON* current_item = NULL;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT)
    {
        return false; /* to deeply nested */
    }
    input_buffer->depth++;

    if (buffer_at_offset(input_buffer)[0] != '[')
    {
        /* not an array */
        goto fail;
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);
    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ']'))
    {
        /* empty array */
        goto success;
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        cJSON* new_item = cJSON_New_Item(who);
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse next value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value_with_ref(who, current_item, input_buffer))
        {
            goto fail; /* failed to parse value */
        }
        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || buffer_at_offset(input_buffer)[0] != ']')
    {
        goto fail; /* expected end of array */
    }

success:
    input_buffer->depth--;

    if (head != NULL)
    {
        head->prev = current_item;
    }

    item->type = cJSON_Array;
    item->child = head;

    input_buffer->offset++;

    return true;

fail:
    if (head != NULL)
    {
        cJSON_Delete(who, head);
    }

    return false;
}

/* Render an array to text */
static cJSON_bool print_array(const char * who, const cJSON* const item, printbuffer* const output_buffer)
{
    unsigned char* output_pointer = NULL;
    size_t length = 0;
    cJSON* current_element = item->child;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* Compose the output array. */
    /* opening square bracket */
    output_pointer = ensure(who, output_buffer, 1);
    if (output_pointer == NULL)
    {
        return false;
    }

    *output_pointer = '[';
    output_buffer->offset++;
    output_buffer->depth++;

    while (current_element != NULL)
    {
        if (!print_value(who, current_element, output_buffer))
        {
            return false;
        }
        update_offset(output_buffer);
        if (current_element->next)
        {
            length = (size_t)(output_buffer->format ? 2 : 1);
            output_pointer = ensure(who, output_buffer, length + 1);
            if (output_pointer == NULL)
            {
                return false;
            }
            *output_pointer++ = ',';
            if (output_buffer->format)
            {
                *output_pointer++ = ' ';
            }
            *output_pointer = '\0';
            output_buffer->offset += length;
        }
        current_element = current_element->next;
    }

    output_pointer = ensure(who, output_buffer, 2);
    if (output_pointer == NULL)
    {
        return false;
    }
    *output_pointer++ = ']';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Build an object from the text. */
static cJSON_bool parse_object(const char * who, cJSON* const item, parse_buffer* const input_buffer)
{
    cJSON* head = NULL; /* linked list head */
    cJSON* current_item = NULL;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT)
    {
        return false; /* to deeply nested */
    }

    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{'))
    {
        goto fail; /* not an object */
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);

    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}'))
    {
        goto success; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        cJSON* new_item = cJSON_New_Item(who);
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse the name of the child */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_string(who, current_item, input_buffer))
        {
            goto fail; /* failed to parse name */
        }

        buffer_skip_whitespace(input_buffer);

        /* swap valuestring and string, because we parsed the name */
        current_item->string = current_item->valuestring;
        current_item->is_key_ref = current_item->is_value_ref;
        current_item->str_key_len = current_item->str_value_len;
        // TRACE_E("current_item->string: %s", current_item->string);

        current_item->is_value_ref = 0;
        current_item->str_value_len = 0;
        current_item->valuestring = NULL;

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':'))
        {
            goto fail; /* invalid object */
        }

        /* parse the value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_value(who, current_item, input_buffer))
        {
            goto fail; /* failed to parse value */
        }

        if (cJSON_True == current_item->is_key_ref)
        {
            current_item->type |= cJSON_StringIsConst;
        }

        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}'))
    {
        goto fail; /* expected end of object */
    }

success:
    input_buffer->depth--;

    if (head != NULL)
    {
        head->prev = current_item;
    }

    item->type = cJSON_Object;
    item->child = head;

    input_buffer->offset++;
    return true;

fail:
    if (head != NULL)
    {
        cJSON_Delete(who, head);
    }

    return false;
}

/* Build an object from the text. */
static cJSON_bool parse_object_with_ref(const char * who, cJSON* const item, parse_buffer* const input_buffer)
{
    cJSON* head = NULL; /* linked list head */
    cJSON* current_item = NULL;

    if (input_buffer->depth >= CJSON_NESTING_LIMIT)
    {
        return false; /* to deeply nested */
    }

    input_buffer->depth++;

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '{'))
    {
        goto fail; /* not an object */
    }

    input_buffer->offset++;
    buffer_skip_whitespace(input_buffer);

    if (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == '}'))
    {
        goto success; /* empty object */
    }

    /* check if we skipped to the end of the buffer */
    if (cannot_access_at_index(input_buffer, 0))
    {
        input_buffer->offset--;
        goto fail;
    }

    /* step back to character in front of the first element */
    input_buffer->offset--;
    /* loop through the comma separated array elements */
    do
    {
        /* allocate next item */
        cJSON* new_item = cJSON_New_Item(who);
        if (new_item == NULL)
        {
            goto fail; /* allocation failure */
        }

        /* attach next item to list */
        if (head == NULL)
        {
            /* start the linked list */
            current_item = head = new_item;
        }
        else
        {
            /* add to the end and advance */
            current_item->next = new_item;
            new_item->prev = current_item;
            current_item = new_item;
        }

        /* parse the name of the child */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);
        if (!parse_string_with_ref(current_item, input_buffer))
        {
            goto fail; /* failed to parse name */
        }

        buffer_skip_whitespace(input_buffer);

        /* swap valuestring and string, because we parsed the name */
        current_item->string = current_item->valuestring;
        current_item->is_key_ref = current_item->is_value_ref;
        current_item->str_key_len = current_item->str_value_len;

        current_item->is_value_ref = 0;
        current_item->str_value_len = 0;
        current_item->valuestring = NULL;

        if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != ':'))
        {
            goto fail; /* invalid object */
        }

        /* parse the value */
        input_buffer->offset++;
        buffer_skip_whitespace(input_buffer);

        if (!parse_value_with_ref(who, current_item, input_buffer))
        {
            goto fail; /* failed to parse value */
        }

        if (cJSON_True == current_item->is_key_ref)
        {
            current_item->type |= cJSON_StringIsConst;
        }

        buffer_skip_whitespace(input_buffer);
    } while (can_access_at_index(input_buffer, 0) && (buffer_at_offset(input_buffer)[0] == ','));

    if (cannot_access_at_index(input_buffer, 0) || (buffer_at_offset(input_buffer)[0] != '}'))
    {
        goto fail; /* expected end of object */
    }

success:
    input_buffer->depth--;

    if (head != NULL)
    {
        head->prev = current_item;
    }

    item->type = cJSON_Object;
    item->child = head;

    input_buffer->offset++;
    return true;

fail:
    if (head != NULL)
    {
        cJSON_Delete(who, head);
    }

    return false;
}

/* Render an object to text. */
static cJSON_bool print_object(const char * who, const cJSON* const item, printbuffer* const output_buffer)
{
    unsigned char* output_pointer = NULL;
    size_t length = 0;
    cJSON* current_item = item->child;

    if (output_buffer == NULL)
    {
        return false;
    }

    /* Compose the output: */
    length = (size_t)(output_buffer->format ? 2 : 1); /* fmt: {\n */
    output_pointer = ensure(who, output_buffer, length + 1);
    if (output_pointer == NULL)
    {
        return false;
    }

    *output_pointer++ = '{';
    output_buffer->depth++;
    if (output_buffer->format)
    {
        *output_pointer++ = '\n';
    }
    output_buffer->offset += length;

    while (current_item)
    {
        if (output_buffer->format)
        {
            size_t i;
            output_pointer = ensure(who, output_buffer, output_buffer->depth);
            if (output_pointer == NULL)
            {
                return false;
            }
            for (i = 0; i < output_buffer->depth; i++)
            {
                *output_pointer++ = '\t';
            }
            output_buffer->offset += output_buffer->depth;
        }

        /* print key */
        if (!print_string_ptr(who, (unsigned char*)current_item->string, current_item->str_key_len, output_buffer))
        {
            return false;
        }
        update_offset(output_buffer);

        length = (size_t)(output_buffer->format ? 2 : 1);
        output_pointer = ensure(who, output_buffer, length);
        if (output_pointer == NULL)
        {
            return false;
        }
        *output_pointer++ = ':';
        if (output_buffer->format)
        {
            *output_pointer++ = '\t';
        }
        output_buffer->offset += length;

        /* print value */
        if (!print_value(who, current_item, output_buffer))
        {
            return false;
        }
        update_offset(output_buffer);

        /* print comma if not last */
        length = ((size_t)(output_buffer->format ? 1 : 0) + (size_t)(current_item->next ? 1 : 0));
        output_pointer = ensure(who, output_buffer, length + 1);

        if (output_pointer == NULL)
        {
            return false;
        }
        if (current_item->next)
        {
            *output_pointer++ = ',';
        }

        if (output_buffer->format)
        {
            *output_pointer++ = '\n';
        }
        *output_pointer = '\0';
        output_buffer->offset += length;

        current_item = current_item->next;
    }

    output_pointer = ensure(who, output_buffer, output_buffer->format ? (output_buffer->depth + 1) : 2);
    if (output_pointer == NULL)
    {
        return false;
    }
    if (output_buffer->format)
    {
        size_t i;
        for (i = 0; i < (output_buffer->depth - 1); i++)
        {
            *output_pointer++ = '\t';
        }
    }
    *output_pointer++ = '}';
    *output_pointer = '\0';
    output_buffer->depth--;

    return true;
}

/* Get Array size/item / object item. */
int cJSON_GetArraySize(const cJSON* array)
{
    cJSON* child = NULL;
    size_t size = 0;

    if (array == NULL)
    {
        return 0;
    }

    child = array->child;

    while (child != NULL)
    {
        size++;
        child = child->next;
    }

    /* FIXME: Can overflow here. Cannot be fixed without breaking the API */

    return (int)size;
}

static cJSON* get_array_item(const cJSON* array, size_t index)
{
    cJSON* current_child = NULL;

    if (array == NULL)
    {
        return NULL;
    }

    current_child = array->child;
    while ((current_child != NULL) && (index > 0))
    {
        index--;
        current_child = current_child->next;
    }

    return current_child;
}

cJSON *cJSON_GetArrayItem(const cJSON* array, int index)
{
    if (index < 0)
    {
        return NULL;
    }

    return get_array_item(array, (size_t)index);
}

static cJSON* get_object_item(const cJSON* const object, const char* const name, const cJSON_bool case_sensitive)
{
    cJSON* current_element = NULL;

    if ((object == NULL) || (name == NULL))
    {
        return NULL;
    }

    current_element = object->child;
    if (case_sensitive)
    {
        size_t const_str_len = strlen(name);
        // while ((current_element != NULL) && (current_element->string != NULL) && (strcmp(name, current_element->string) != 0))
        while ((current_element != NULL) && (current_element->string != NULL) &&
            (strncmp(name, current_element->string, ((const_str_len > current_element->str_key_len) ? const_str_len : current_element->str_key_len)) != 0))
        {
            current_element = current_element->next;
        }
    }
    else
    {
        while ((current_element != NULL) &&
            (case_insensitive_strcmp((const unsigned char*)name, (const unsigned char*)(current_element->string), current_element->str_key_len) != 0))
        {
            current_element = current_element->next;
        }
    }

    if ((current_element == NULL) || (current_element->string == NULL))
    {
        return NULL;
    }

    return current_element;
}

cJSON * cJSON_GetObjectItem(const char * who, const cJSON* const object, const char* const string)
{
    return get_object_item(object, string, false);
}

cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON* const object, const char* const string)
{
    return get_object_item(object, string, true);
}

cJSON_bool cJSON_HasObjectItem(const char * who, const cJSON* object, const char* string)
{
    return cJSON_GetObjectItem(who, object, string) ? 1 : 0;
}

/* Utility for array list handling. */
static void suffix_object(cJSON* prev, cJSON* item)
{
    prev->next = item;
    item->prev = prev;
}

/* Utility for handling references. */
static cJSON* create_reference(const char * who, const cJSON* item)
{
    cJSON* reference = NULL;
    if (item == NULL)
    {
        return NULL;
    }

    reference = cJSON_New_Item(who);
    if (reference == NULL)
    {
        return NULL;
    }

    memcpy(reference, item, sizeof(cJSON));

    reference->string = NULL;
    reference->is_value_ref = cJSON_True;
    reference->type |= (cJSON_IsReference);
    reference->next = reference->prev = NULL;

    return reference;
}

static cJSON_bool add_item_to_array(cJSON* array, cJSON* item)
{
    cJSON* child = NULL;

    if ((item == NULL) || (array == NULL) || (array == item))
    {
        return false;
    }

    child = array->child;
    /*
     * To find the last item in array quickly, we use prev in array
     */
    if (child == NULL)
    {
        /* list is empty, start new one */
        array->child = item;
        item->prev = item;
        item->next = NULL;
    }
    else
    {
        /* append to the end */
        if (child->prev)
        {
            suffix_object(child->prev, item);
            array->child->prev = item;
        }
    }

    return true;
}

/* Add item to array/object. */
cJSON_bool cJSON_AddItemToArray(cJSON* array, cJSON* item)
{
    return add_item_to_array(array, item);
}

#if defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
#pragma GCC diagnostic push
#endif
#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wcast-qual"
#endif
/* helper function to cast away const */
static void* cast_away_const(const void* string)
{
    return (void*)string;
}
#if defined(__clang__) || (defined(__GNUC__) && ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ > 5))))
#pragma GCC diagnostic pop
#endif

static cJSON_bool add_item_to_object(const char * who, cJSON* const object, const char* const string, cJSON* const item, const cJSON_bool constant_key)
{
    char* new_key = NULL;
    int is_key_ref = 0;
    int new_type = cJSON_Invalid;

    if ((object == NULL) || (string == NULL) || (item == NULL) || (object == item))
    {
        return false;
    }

    if (constant_key)
    {
        is_key_ref = cJSON_True;
        new_key = (char*)cast_away_const(string);
        new_type = item->type | cJSON_StringIsConst;
    }
    else
    {
        new_key = (char*)cJSON_strdup(who, (const unsigned char*)string, strlen(string));
        if (new_key == NULL)
        {
            return false;
        }

        item->is_key_ref = cJSON_False;
        new_type = item->type & ~cJSON_StringIsConst;
    }

    if (!(item->type & cJSON_StringIsConst) && (item->string != NULL))
    {
        ezlopi_free(who, item->string);
    }

    item->type = new_type | ((is_key_ref == cJSON_True) ? cJSON_StringIsConst : 0);
    item->string = new_key;
    item->is_key_ref = is_key_ref;
    item->str_key_len = strlen(new_key);
    item->type |= (is_key_ref == cJSON_True) ? cJSON_StringIsConst : 0;

    return add_item_to_array(object, item);
}


cJSON_bool cJSON_AddItemToObject(const char * who, cJSON* object, const char* string, cJSON* item)
{
    return add_item_to_object(who, object, string, item, false);
}

/* Add an item to an object with constant string as key */
cJSON_bool cJSON_AddItemToObjectCS(const char * who, cJSON* object, const char* string, cJSON* item)
{
    return add_item_to_object(who, object, string, item, true);
}

cJSON_bool cJSON_AddItemReferenceToArray(const char * who, cJSON* array, cJSON* item)
{
    if (array == NULL)
    {
        return false;
    }

    return add_item_to_array(array, create_reference(who, item));
}

cJSON_bool cJSON_AddItemReferenceToObject(const char * who, cJSON* object, const char* string, cJSON* item)
{
    if ((object == NULL) || (string == NULL))
    {
        return false;
    }

    return add_item_to_object(who, object, string, create_reference(who, item), false);
}

cJSON *cJSON_AddNullToObject(const char * who, cJSON* const object, const char* const name)
{
    cJSON* null = cJSON_CreateNull(who);
    if (add_item_to_object(who, object, name, null, false))
    {
        return null;
    }

    cJSON_Delete(who, null);
    return NULL;
}

cJSON *cJSON_AddTrueToObject(const char * who, cJSON* const object, const char* const name)
{
    cJSON* true_item = cJSON_CreateTrue(who);
    if (add_item_to_object(who, object, name, true_item, false))
    {
        return true_item;
    }

    cJSON_Delete(who, true_item);
    return NULL;
}

cJSON *cJSON_AddFalseToObject(const char * who, cJSON* const object, const char* const name)
{
    cJSON* false_item = cJSON_CreateFalse(who);
    if (add_item_to_object(who, object, name, false_item, false))
    {
        return false_item;
    }

    cJSON_Delete(who, false_item);
    return NULL;
}

cJSON *cJSON_AddBoolToObject(const char * who, cJSON* const object, const char* const name, const cJSON_bool boolean)
{
    cJSON* bool_item = cJSON_CreateBool(who, boolean);
    if (add_item_to_object(who, object, name, bool_item, false))
    {
        return bool_item;
    }

    cJSON_Delete(who, bool_item);
    return NULL;
}

cJSON *cJSON_AddNumberToObject(const char * who, cJSON* const object, const char* const name, const double number)
{
    cJSON* number_item = cJSON_CreateNumber(who, number);
    if (add_item_to_object(who, object, name, number_item, false))
    {
        return number_item;
    }

    cJSON_Delete(who, number_item);
    return NULL;
}

cJSON *cJSON_AddNumberToObjectWithRef(const char * who, cJSON* const object, const char* const name, const double number)
{
    cJSON* number_item = cJSON_CreateNumber(who, number);
    if (add_item_to_object(who, object, name, number_item, true))
    {
        return number_item;
    }

    cJSON_Delete(who, number_item);
    return NULL;
}

cJSON *cJSON_AddStringToObject(const char * who, cJSON* const object, const char* const name, const char* const string)
{
    cJSON* string_item = cJSON_CreateString(who, string);
    if (add_item_to_object(who, object, name, string_item, false))
    {
        return string_item;
    }

    cJSON_Delete(who, string_item);
    return NULL;
}

cJSON *cJSON_AddStringToObjectWithRef(const char * who, cJSON* const object, const char* const name, const char* const string)
{
    cJSON* string_item = cJSON_CreateStringReference(who, string);
    if (add_item_to_object(who, object, name, string_item, true))
    {
        return string_item;
    }

    cJSON_Delete(who, string_item);
    return NULL;
}

cJSON *cJSON_AddRawToObject(const char * who, cJSON* const object, const char* const name, const char* const raw)
{
    cJSON* raw_item = cJSON_CreateRaw(who, raw);
    if (add_item_to_object(who, object, name, raw_item, false))
    {
        return raw_item;
    }

    cJSON_Delete(who, raw_item);
    return NULL;
}

cJSON *cJSON_AddObjectToObject(const char * who, cJSON* const object, const char* const name)
{
    cJSON* object_item = cJSON_CreateObject(who);
    if (add_item_to_object(who, object, name, object_item, false))
    {
        return object_item;
    }

    cJSON_Delete(who, object_item);
    return NULL;
}

cJSON *cJSON_AddObjectToObjectWithRef(const char * who, cJSON* const object, const char* const name)
{
    cJSON* object_item = cJSON_CreateObject(who);
    if (add_item_to_object(who, object, name, object_item, true))
    {
        return object_item;
    }

    cJSON_Delete(who, object_item);
    return NULL;
}

cJSON *cJSON_AddArrayToObject(const char * who, cJSON* const object, const char* const name)
{
    cJSON* array = cJSON_CreateArray(who);
    if (add_item_to_object(who, object, name, array, false))
    {
        return array;
    }

    cJSON_Delete(who, array);
    return NULL;
}

cJSON *cJSON_DetachItemViaPointer(const char * who, cJSON* parent, cJSON* const item)
{
    if ((parent == NULL) || (item == NULL))
    {
        return NULL;
    }

    if (item != parent->child)
    {
        /* not the first element */
        item->prev->next = item->next;
    }
    if (item->next != NULL)
    {
        /* not the last element */
        item->next->prev = item->prev;
    }

    if (item == parent->child)
    {
        /* first element */
        parent->child = item->next;
    }
    else if (item->next == NULL)
    {
        /* last element */
        parent->child->prev = item->prev;
    }

    /* make sure the detached item doesn't point anywhere anymore */
    item->prev = NULL;
    item->next = NULL;

    return item;
}

cJSON *cJSON_DetachItemFromArray(const char * who, cJSON* array, int which)
{
    if (which < 0)
    {
        return NULL;
    }

    return cJSON_DetachItemViaPointer(who, array, get_array_item(array, (size_t)which));
}

void cJSON_DeleteItemFromArray(const char * who, cJSON* array, int which)
{
    cJSON_Delete(who, cJSON_DetachItemFromArray(who, array, which));
}

cJSON *cJSON_DetachItemFromObject(const char * who, cJSON* object, const char* string)
{
    cJSON* to_detach = cJSON_GetObjectItem(who, object, string);
    return cJSON_DetachItemViaPointer(who, object, to_detach);
}

cJSON *cJSON_DetachItemFromObjectCaseSensitive(const char * who, cJSON* object, const char* string)
{
    cJSON* to_detach = cJSON_GetObjectItemCaseSensitive(object, string);
    return cJSON_DetachItemViaPointer(who, object, to_detach);
}

void cJSON_DeleteItemFromObject(const char * who, cJSON* object, const char* string)
{
    cJSON_Delete(who, cJSON_DetachItemFromObject(who, object, string));
}

void cJSON_DeleteItemFromObjectCaseSensitive(const char * who, cJSON* object, const char* string)
{
    cJSON_Delete(who, cJSON_DetachItemFromObjectCaseSensitive(who, object, string));
}

/* Replace array/object items with new ones. */
cJSON_bool cJSON_InsertItemInArray(cJSON* array, int which, cJSON* newitem)
{
    cJSON* after_inserted = NULL;

    if (which < 0 || newitem == NULL)
    {
        return false;
    }

    after_inserted = get_array_item(array, (size_t)which);
    if (after_inserted == NULL)
    {
        return add_item_to_array(array, newitem);
    }

    if (after_inserted != array->child && after_inserted->prev == NULL)
    {
        /* return false if after_inserted is a corrupted array item */
        return false;
    }

    newitem->next = after_inserted;
    newitem->prev = after_inserted->prev;
    after_inserted->prev = newitem;
    if (after_inserted == array->child)
    {
        array->child = newitem;
    }
    else
    {
        newitem->prev->next = newitem;
    }
    return true;
}

cJSON_bool cJSON_ReplaceItemViaPointer(const char * who, cJSON* const parent, cJSON* const item, cJSON* replacement)
{
    if ((parent == NULL) || (parent->child == NULL) || (replacement == NULL) || (item == NULL))
    {
        return false;
    }

    if (replacement == item)
    {
        return true;
    }

    replacement->next = item->next;
    replacement->prev = item->prev;

    if (replacement->next != NULL)
    {
        replacement->next->prev = replacement;
    }
    if (parent->child == item)
    {
        if (parent->child->prev == parent->child)
        {
            replacement->prev = replacement;
        }
        parent->child = replacement;
    }
    else
    { /*
       * To find the last item in array quickly, we use prev in array.
       * We can't modify the last item's next pointer where this item was the parent's child
       */
        if (replacement->prev != NULL)
        {
            replacement->prev->next = replacement;
        }
        if (replacement->next == NULL)
        {
            parent->child->prev = replacement;
        }
    }

    item->next = NULL;
    item->prev = NULL;
    cJSON_Delete(who, item);

    return true;
}

cJSON_bool cJSON_ReplaceItemInArray(const char * who, cJSON* array, int which, cJSON* newitem)
{
    if (which < 0)
    {
        return false;
    }

    return cJSON_ReplaceItemViaPointer(who, array, get_array_item(array, (size_t)which), newitem);
}

static cJSON_bool replace_item_in_object(const char * who, cJSON* object, const char* string, cJSON* replacement, cJSON_bool case_sensitive)
{
    if ((replacement == NULL) || (string == NULL))
    {
        return false;
    }

    /* replace the name in the replacement */
    if (!(replacement->type & cJSON_StringIsConst) && (replacement->string != NULL))
    {
        ezlopi_free(who, replacement->string);
    }

    replacement->str_key_len = strlen(string);
    replacement->string = (char*)cJSON_strdup(who, (const unsigned char*)string, replacement->str_key_len);

    if (replacement->string == NULL)
    {
        return false;
    }

    replacement->is_key_ref = cJSON_False;
    replacement->type &= ~cJSON_StringIsConst;

    return cJSON_ReplaceItemViaPointer(who, object, get_object_item(object, string, case_sensitive), replacement);
}

cJSON_bool cJSON_ReplaceItemInObject(const char * who, cJSON* object, const char* string, cJSON* newitem)
{
    return replace_item_in_object(who, object, string, newitem, false);
}

cJSON_bool cJSON_ReplaceItemInObjectCaseSensitive(const char * who, cJSON* object, const char* string, cJSON* newitem)
{
    return replace_item_in_object(who, object, string, newitem, true);
}

/* Create basic types: */
cJSON *cJSON_CreateNull(const char * who)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->type = cJSON_NULL;
    }

    return item;
}

cJSON *cJSON_CreateTrue(const char * who)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->type = cJSON_True;
    }

    return item;
}

cJSON *cJSON_CreateFalse(const char * who)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->type = cJSON_False;
    }

    return item;
}

cJSON *cJSON_CreateBool(const char * who, cJSON_bool boolean)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->type = boolean ? cJSON_True : cJSON_False;
    }

    return item;
}

cJSON *cJSON_CreateNumber(const char * who, double num)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->type = cJSON_Number;
        item->valuedouble = num;

        /* use saturation in case of overflow */
        if (num >= INT_MAX)
        {
            item->valueint = INT_MAX;
        }
        else if (num <= (double)INT_MIN)
        {
            item->valueint = INT_MIN;
        }
        else
        {
            item->valueint = (int)num;
        }
    }

    return item;
}

cJSON *cJSON_CreateString(const char * who, const char* string)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->str_value_len = strlen(string);
        item->valuestring = (char*)cJSON_strdup(who, (const unsigned char*)string, item->str_value_len);

        if (!item->valuestring)
        {
            cJSON_Delete(who, item);
            return NULL;
        }

        item->type = cJSON_String;
        item->is_value_ref = cJSON_False;
    }

    return item;
}

cJSON *cJSON_CreateStringReference(const char * who, const char* string)
{
    cJSON* item = cJSON_New_Item(who);
    if (item != NULL)
    {
        item->type = cJSON_String | cJSON_IsReference;
        item->valuestring = (char *)string;
        item->str_value_len = strlen(string);
        item->is_value_ref = cJSON_True;
    }

    return item;
}

cJSON *cJSON_CreateObjectReference(const char * who, const cJSON* child)
{
    cJSON* item = cJSON_New_Item(who);
    if (item != NULL)
    {
        item->type = cJSON_Object | cJSON_IsReference;
        item->child = (cJSON*)cast_away_const(child);
    }

    return item;
}

cJSON *cJSON_CreateArrayReference(const char * who, const cJSON* child)
{
    cJSON* item = cJSON_New_Item(who);
    if (item != NULL)
    {
        item->type = cJSON_Array | cJSON_IsReference;
        item->child = (cJSON*)cast_away_const(child);
    }

    return item;
}

cJSON *cJSON_CreateRaw(const char * who, const char* raw)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->str_value_len = strlen(raw);
        item->valuestring = (char*)cJSON_strdup(who, (const unsigned char*)raw, item->str_value_len);

        if (!item->valuestring)
        {
            cJSON_Delete(who, item);
            return NULL;
        }

        item->type = cJSON_Raw;
        item->is_value_ref = cJSON_False;
    }

    return item;
}

cJSON *cJSON_CreateArray(const char * who)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->type = cJSON_Array;
    }

    return item;
}

cJSON *cJSON_CreateObject(const char * who)
{
    cJSON* item = cJSON_New_Item(who);
    if (item)
    {
        item->type = cJSON_Object;
    }

    return item;
}

/* Create Arrays: */
cJSON *cJSON_CreateIntArray(const char * who, const int* numbers, int count)
{
    size_t i = 0;
    cJSON* n = NULL;
    cJSON* p = NULL;
    cJSON* a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray(who);

    for (i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateNumber(who, numbers[i]);
        if (!n)
        {
            cJSON_Delete(who, a);
            return NULL;
        }
        if (!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child)
    {
        a->child->prev = n;
    }

    return a;
}

cJSON *cJSON_CreateFloatArray(const char *who, const float* numbers, int count)
{
    size_t i = 0;
    cJSON* n = NULL;
    cJSON* p = NULL;
    cJSON* a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray(who);

    for (i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateNumber(who, (double)numbers[i]);
        if (!n)
        {
            cJSON_Delete(who, a);
            return NULL;
        }
        if (!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child)
    {
        a->child->prev = n;
    }

    return a;
}

cJSON *cJSON_CreateDoubleArray(const char *who, const double* numbers, int count)
{
    size_t i = 0;
    cJSON* n = NULL;
    cJSON* p = NULL;
    cJSON* a = NULL;

    if ((count < 0) || (numbers == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray(who);

    for (i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateNumber(who, numbers[i]);
        if (!n)
        {
            cJSON_Delete(who, a);
            return NULL;
        }
        if (!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child)
    {
        a->child->prev = n;
    }

    return a;
}

cJSON *cJSON_CreateStringArray(const char * who, const char* const* strings, int count)
{
    size_t i = 0;
    cJSON* n = NULL;
    cJSON* p = NULL;
    cJSON* a = NULL;

    if ((count < 0) || (strings == NULL))
    {
        return NULL;
    }

    a = cJSON_CreateArray(who);

    for (i = 0; a && (i < (size_t)count); i++)
    {
        n = cJSON_CreateString(who, strings[i]);
        if (!n)
        {
            cJSON_Delete(who, a);
            return NULL;
        }
        if (!i)
        {
            a->child = n;
        }
        else
        {
            suffix_object(p, n);
        }
        p = n;
    }

    if (a && a->child)
    {
        a->child->prev = n;
    }

    return a;
}

/* Duplication */
cJSON *cJSON_Duplicate(const char * who, const cJSON* item, cJSON_bool recurse)
{
    cJSON* newitem = NULL;
    cJSON* child = NULL;
    cJSON* next = NULL;
    cJSON* newchild = NULL;

    /* Bail on bad ptr */
    if (!item)
    {
        goto fail;
    }
    /* Create new item */
    newitem = cJSON_New_Item(who);
    if (!newitem)
    {
        goto fail;
    }

    /* Copy over all vars */
    newitem->type = item->type & (~cJSON_IsReference) & (~cJSON_IsReference);
    newitem->valueint = item->valueint;
    newitem->valuedouble = item->valuedouble;

    if (item->valuestring)
    {
        newitem->is_value_ref = cJSON_False;
        newitem->str_value_len = item->str_value_len;
        newitem->valuestring = (char*)cJSON_strdup(who, (unsigned char*)item->valuestring, item->str_value_len);

        if (!newitem->valuestring)
        {
            goto fail;
        }
    }

    if (item->string)
    {
        newitem->is_key_ref = cJSON_False;
        newitem->str_key_len = item->str_key_len;
        newitem->string = (char*)cJSON_strdup(who, (unsigned char*)item->string, item->str_key_len);

        if (!newitem->string)
        {
            goto fail;
        }
    }

    /* If non-recursive, then we're done! */
    if (!recurse)
    {
        return newitem;
    }
    /* Walk the ->next chain for the child. */
    child = item->child;
    while (child != NULL)
    {
        newchild = cJSON_Duplicate(who, child, true); /* Duplicate (with recurse) each item in the ->next chain */

        if (!newchild)
        {
            goto fail;
        }

        if (next != NULL)
        {
            /* If newitem->child already set, then crosswire ->prev and ->next and move on */
            next->next = newchild;
            newchild->prev = next;
            next = newchild;
        }
        else
        {
            /* Set newitem->child and move to it */
            newitem->child = newchild;
            next = newchild;
        }

        child = child->next;
    }

    if (newitem && newitem->child)
    {
        newitem->child->prev = newchild;
    }

    return newitem;

fail:
    if (newitem != NULL)
    {
        cJSON_Delete(who, newitem);
    }

    return NULL;
}

static void skip_oneline_comment(char** input)
{
    *input += static_strlen("//");

    for (; (*input)[0] != '\0'; ++(*input))
    {
        if ((*input)[0] == '\n')
        {
            *input += static_strlen("\n");
            return;
        }
    }
}

static void skip_multiline_comment(char** input)
{
    *input += static_strlen("/*");

    for (; (*input)[0] != '\0'; ++(*input))
    {
        if (((*input)[0] == '*') && ((*input)[1] == '/'))
        {
            *input += static_strlen("*/");
            return;
        }
    }
}

static void minify_string(char** input, char** output)
{
    (*output)[0] = (*input)[0];
    *input += static_strlen("\"");
    *output += static_strlen("\"");

    for (; (*input)[0] != '\0'; (void)++(*input), ++(*output))
    {
        (*output)[0] = (*input)[0];

        if ((*input)[0] == '\"')
        {
            (*output)[0] = '\"';
            *input += static_strlen("\"");
            *output += static_strlen("\"");
            return;
        }
        else if (((*input)[0] == '\\') && ((*input)[1] == '\"'))
        {
            (*output)[1] = (*input)[1];
            *input += static_strlen("\"");
            *output += static_strlen("\"");
        }
    }
}

void cJSON_Minify(char* json)
{
    char* into = json;

    if (json == NULL)
    {
        return;
    }

    while (json[0] != '\0')
    {
        switch (json[0])
        {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            json++;
            break;

        case '/':
            if (json[1] == '/')
            {
                skip_oneline_comment(&json);
            }
            else if (json[1] == '*')
            {
                skip_multiline_comment(&json);
            }
            else
            {
                json++;
            }
            break;

        case '\"':
            minify_string(&json, (char**)&into);
            break;

        default:
            into[0] = json[0];
            json++;
            into++;
        }
    }

    /* and null-terminate. */
    *into = '\0';
}

cJSON_bool cJSON_IsInvalid(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Invalid;
}

cJSON_bool cJSON_IsFalse(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_False;
}

cJSON_bool cJSON_IsTrue(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xff) == cJSON_True;
}

cJSON_bool cJSON_IsBool(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & (cJSON_True | cJSON_False)) != 0;
}
cJSON_bool cJSON_IsNull(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_NULL;
}

cJSON_bool cJSON_IsNumber(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Number;
}

cJSON_bool cJSON_IsString(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_String;
}

cJSON_bool cJSON_IsArray(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Array;
}

cJSON_bool cJSON_IsObject(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Object;
}

cJSON_bool cJSON_IsRaw(const cJSON* const item)
{
    if (item == NULL)
    {
        return false;
    }

    return (item->type & 0xFF) == cJSON_Raw;
}

cJSON_bool cJSON_Compare(const cJSON* const a, const cJSON* const b, const cJSON_bool case_sensitive)
{
    if ((a == NULL) || (b == NULL) || ((a->type & 0xFF) != (b->type & 0xFF)))
    {
        return false;
    }

    /* check if type is valid */
    switch (a->type & 0xFF)
    {
    case cJSON_False:
    case cJSON_True:
    case cJSON_NULL:
    case cJSON_Number:
    case cJSON_String:
    case cJSON_Raw:
    case cJSON_Array:
    case cJSON_Object:
        break;

    default:
        return false;
    }

    /* identical objects are equal */
    if (a == b)
    {
        return true;
    }

    switch (a->type & 0xFF)
    {
        /* in these cases and equal type is enough */
    case cJSON_False:
    case cJSON_True:
    case cJSON_NULL:
        return true;

    case cJSON_Number:
        if (compare_double(a->valuedouble, b->valuedouble))
        {
            return true;
        }
        return false;

    case cJSON_String:
    case cJSON_Raw:
        if ((a->valuestring == NULL) || (b->valuestring == NULL))
        {
            return false;
        }
        uint32_t comp_len = (a->str_value_len > b->str_value_len) ? a->str_value_len : b->str_value_len;
        if (strncmp(a->valuestring, b->valuestring, comp_len) == 0)
        {
            return true;
        }

        return false;

    case cJSON_Array:
    {
        cJSON* a_element = a->child;
        cJSON* b_element = b->child;

        for (; (a_element != NULL) && (b_element != NULL);)
        {
            if (!cJSON_Compare(a_element, b_element, case_sensitive))
            {
                return false;
            }

            a_element = a_element->next;
            b_element = b_element->next;
        }

        /* one of the arrays is longer than the other */
        if (a_element != b_element)
        {
            return false;
        }

        return true;
    }

    case cJSON_Object:
    {
        cJSON* a_element = NULL;
        cJSON* b_element = NULL;
        cJSON_ArrayForEach(a_element, a)
        {
            /* TODO This has O(n^2) runtime, which is horrible! */
            b_element = get_object_item(b, a_element->string, case_sensitive);
            if (b_element == NULL)
            {
                return false;
            }

            if (!cJSON_Compare(a_element, b_element, case_sensitive))
            {
                return false;
            }
        }

        /* doing this twice, once on a and b to prevent true comparison if a subset of b
         * TODO: Do this the proper way, this is just a fix for now */
        cJSON_ArrayForEach(b_element, b)
        {
            a_element = get_object_item(a, b_element->string, case_sensitive);
            if (a_element == NULL)
            {
                return false;
            }

            if (!cJSON_Compare(b_element, a_element, case_sensitive))
            {
                return false;
            }
        }

        return true;
    }

    default:
        return false;
    }
}

static int __estimateFromatedPrintLength(const char * who, cJSON* item, uint32_t child_offset)
{
    int ret = 0;

    while (item)
    {
        ret += child_offset;

        if (item->string)
        {
            ret += 5 + item->str_key_len;
        }

        switch (item->type)
        {
        case cJSON_False:
        {
            ret += 5;
            break;
        }
        case cJSON_True:
        case cJSON_NULL:
        {
            ret += 4;
            break;
        }
        case cJSON_Number:
        {
            char tmp_buffer[32];
            memset(tmp_buffer, 0, 32);

            printbuffer prt_buffer = {
                .buffer = (unsigned char*)tmp_buffer,
                .length = 32,
                .offset = 0,
                .depth = 0,
                .noalloc = cJSON_True,
                .format = cJSON_False,
            };

            print_number(who, item, &prt_buffer);
            ret += strlen(tmp_buffer);

            // if (item->next)
            // {
            //     ret += 1;
            // }
            break;
        }
        case cJSON_String:
        {
            ret += item->str_value_len + 2;
            break;
        }
        case cJSON_Raw:
        {
            ret += item->str_value_len;
            break;
        }
        case cJSON_Array:
        {
            ret += 2;
            if (item->child)
            {
                if (item->child->type == cJSON_Object)
                {
                    ret += __estimateFromatedPrintLength(who, item->child, child_offset + 1);
                    ret--;
                    cJSON* child = item->child;
                    while (child)
                    {
                        ret--;
                        child = child->next;
                    }
                }
                else
                {
                    ret += __estimateFromatedPrintLength(who, item->child, 1);
                    ret--;
                }
            }
            break;
        }
        case cJSON_Object:
        {
            if (item->string)
            {
                ret += 3 + child_offset;
            }
            else
            {
                ret += 3 + child_offset;
            }

            ret += __estimateFromatedPrintLength(who, item->child, child_offset + 1);
            break;
        }
        default:
        {
            TRACE_E("invalid type!");
            break;
        }
        }

        if (item->next)
        {
            ret += 1;
        }

        item = item->next;
    }

    return ret;
}

int cJSON_EstimatePrintLength(const char * who, cJSON* item)
{
    return (__estimateFromatedPrintLength(who, item, 0));
}
