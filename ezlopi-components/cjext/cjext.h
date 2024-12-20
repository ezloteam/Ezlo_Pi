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

#ifndef cJSON__h
#define cJSON__h

#ifdef __cplusplus
extern "C"
{
#endif

  /* project version */
#define CJSON_VERSION_MAJOR 1
#define CJSON_VERSION_MINOR 7
#define CJSON_VERSION_PATCH 17

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"

#ifndef __FILENAME__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/* malloc/free objects using the malloc/free functions that have been set with cJSON_InitHooks */
// #define cJSON_free free
// #define cJSON_malloc malloc
// #define cJSON_realloc realloc

/* cJSON Types: */
#define cJSON_Invalid (0)     // 0
#define cJSON_False (1 << 0)  // 1
#define cJSON_True (1 << 1)   // 2
#define cJSON_NULL (1 << 2)   // 4
#define cJSON_Number (1 << 3) // 8
#define cJSON_String (1 << 4) // 16
#define cJSON_Array (1 << 5)  // 32
#define cJSON_Object (1 << 6) // 64
#define cJSON_Raw (1 << 7)    // 128

#define cJSON_IsReference 256   // (1 << 8)
#define cJSON_StringIsConst 512 // (1 << 9)

  typedef int cJSON_bool;

  /* Limits how deeply nested arrays/objects can be before cJSON rejects to parse them.
   * This is to prevent stack overflows. */
#define CJSON_NESTING_LIMIT 1000

  /* The cJSON structure: */
  typedef struct cJSON
  {
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct cJSON *child;

    /* The type of the item, as above. */
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char *valuestring;
    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */
    int valueint;
    /* The item's number, if type==cJSON_Number */
    double valuedouble;

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char *string;

    /* Krishna */
    int is_key_ref;
    int is_value_ref;
    size_t str_key_len;
    size_t str_value_len;
  } cJSON;

  /* returns the version of cJSON as a string */
  const char *cJSON_Version(void);

  /* Memory Management: the caller is always responsible to free the results from all variants of cJSON_Parse (with cJSON_Delete) and cJSON_Print (with stdlib free, cJSON_Hooks.free_fn, or cJSON_free as appropriate). The exception is cJSON_PrintPreallocated, where the caller has full responsibility of the buffer. */
  /* Supply a block of JSON, and this returns a cJSON object you can interrogate. */
  cJSON *cJSON_Parse(const char *who, const char *value);
  cJSON *cJSON_ParseWithLength(const char *who, const char *value, size_t buffer_length);
  /* ParseWithOpts allows you to require (and check) that the JSON is null terminated, and to retrieve the pointer to the final byte parsed. */
  /* If you supply a ptr in return_parse_end and parsing fails, then return_parse_end will contain a pointer to the error so will match cJSON_GetErrorPtr(). */
  cJSON *cJSON_ParseWithOpts(const char *who, const char *value, const char **return_parse_end, bool require_null_terminated);
  cJSON *cJSON_ParseWithLengthOpts(const char *who, const char *value, size_t buffer_length, const char **return_parse_end, bool require_null_terminated);

  /* Render a cJSON entity to text for transfer/storage. */
  char *cJSON_Print(const char *who, const cJSON *item);
  /* Render a cJSON entity to text for transfer/storage without any formatting. */
  char *cJSON_PrintUnformatted(const char *who, const cJSON *item);
  /* Render a cJSON entity to text using a buffered strategy. prebuffer is a guess at the final size. guessing well reduces reallocation. fmt=0 gives unformatted, =1 gives formatted */
  char *cJSON_PrintBuffered(const char *who, const cJSON *item, int prebuffer, bool fmt);
  /* Render a cJSON entity to text using a buffer already allocated in memory with given length. Returns 1 on success and 0 on failure. */
  /* NOTE: cJSON is not always 100% accurate in estimating how much memory it will use, so to be safe allocate 5 bytes more than you actually need */
  bool cJSON_PrintPreallocated(const char *who, cJSON *item, char *buffer, const int length, const bool format);
  /* Delete a cJSON entity and all subentities. */
  void cJSON_Delete(const char *who, cJSON *item);

  /* Returns the number of items in an array (or object). */
  int cJSON_GetArraySize(const cJSON *array);
  /* Retrieve item number "index" from array "array". Returns NULL if unsuccessful. */
  cJSON *cJSON_GetArrayItem(const cJSON *array, int index);
  /* Get item "string" from object. Case insensitive. */
  cJSON *cJSON_GetObjectItem(const char *who, const cJSON *const object, const char *const string);
  cJSON *cJSON_GetObjectItemCaseSensitive(const cJSON *const object, const char *const string);
  bool cJSON_HasObjectItem(const char *who, const cJSON *object, const char *string);
  /* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
  const char *cJSON_GetErrorPtr(void);

  /* Check item type and return its value */
  char *cJSON_GetStringValue(const cJSON *const item);
  double cJSON_GetNumberValue(const cJSON *const item);

  /* These functions check the type of an item */
  bool cJSON_IsInvalid(const cJSON *const item);
  bool cJSON_IsFalse(const cJSON *const item);
  bool cJSON_IsTrue(const cJSON *const item);
  bool cJSON_IsBool(const cJSON *const item);
  bool cJSON_IsNull(const cJSON *const item);
  bool cJSON_IsNumber(const cJSON *const item);
  bool cJSON_IsString(const cJSON *const item);
  bool cJSON_IsArray(const cJSON *const item);
  bool cJSON_IsObject(const cJSON *const item);
  bool cJSON_IsRaw(const cJSON *const item);

  /* These calls create a cJSON item of the appropriate type. */
  cJSON *cJSON_CreateNull(const char *who);
  cJSON *cJSON_CreateTrue(const char *who);
  cJSON *cJSON_CreateFalse(const char *who);
  cJSON *cJSON_CreateBool(const char *who, bool boolean);
  cJSON *cJSON_CreateNumber(const char *who, double num);
  cJSON *cJSON_CreateString(const char *who, const char *string);
  /* raw json */
  cJSON *cJSON_CreateRaw(const char *who, const char *raw);
  cJSON *cJSON_CreateArray(const char *who);
  cJSON *cJSON_CreateObject(const char *who);

  /* Create a string where valuestring references a string so
   * it will not be freed by cJSON_Delete */
  cJSON *cJSON_CreateStringReference(const char *who, const char *string);
  /* Create an object/array that only references it's elements so
   * they will not be freed by cJSON_Delete */
  cJSON *cJSON_CreateObjectReference(const char *who, const cJSON *child);
  cJSON *cJSON_CreateArrayReference(const char *who, const cJSON *child);

  /* These utilities create an Array of count items.
   * The parameter count cannot be greater than the number of elements in the number array, otherwise array access will be out of bounds.*/
  cJSON *cJSON_CreateIntArray(const char *who, const int *numbers, int count);
  cJSON *cJSON_CreateFloatArray(const char *who, const float *numbers, int count);
  cJSON *cJSON_CreateDoubleArray(const char *who, const double *numbers, int count);
  cJSON *cJSON_CreateStringArray(const char *who, const char *const *strings, int count);

  /* Append item to the specified array/object. */
  bool cJSON_AddItemToArray(cJSON *array, cJSON *item);
  bool cJSON_AddItemToObject(const char *who, cJSON *object, const char *string, cJSON *item);
  /* Use this when string is definitely const (i.e. a literal, or as good as), and will definitely survive the cJSON object.
   * WARNING: When this function was used, make sure to always check that (item->type & cJSON_StringIsConst) is zero before
   * writing to `item->string` */
  bool cJSON_AddItemToObjectCS(const char *who, cJSON *object, const char *string, cJSON *item);
  /* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
  bool cJSON_AddItemReferenceToArray(const char *who, cJSON *array, cJSON *item);
  bool cJSON_AddItemReferenceToObject(const char *who, cJSON *object, const char *string, cJSON *item);

  /* Remove/Detach items from Arrays/Objects. */
  cJSON *cJSON_DetachItemViaPointer(const char *who, cJSON *parent, cJSON *const item);
  cJSON *cJSON_DetachItemFromArray(const char *who, cJSON *array, int which);
  void cJSON_DeleteItemFromArray(const char *who, cJSON *array, int which);
  cJSON *cJSON_DetachItemFromObject(const char *who, cJSON *object, const char *string);
  cJSON *cJSON_DetachItemFromObjectCaseSensitive(const char *who, cJSON *object, const char *string);
  void cJSON_DeleteItemFromObject(const char *who, cJSON *object, const char *string);
  void cJSON_DeleteItemFromObjectCaseSensitive(const char *who, cJSON *object, const char *string);

  /* Update array items. */
  bool cJSON_InsertItemInArray(cJSON *array, int which, cJSON *newitem); /* Shifts pre-existing items to the right. */
  bool cJSON_ReplaceItemViaPointer(const char *who, cJSON *const parent, cJSON *const item, cJSON *replacement);
  bool cJSON_ReplaceItemInArray(const char *who, cJSON *array, int which, cJSON *newitem);
  bool cJSON_ReplaceItemInObject(const char *who, cJSON *object, const char *string, cJSON *newitem);
  bool cJSON_ReplaceItemInObjectCaseSensitive(const char *who, cJSON *object, const char *string, cJSON *newitem);

  /* Duplicate a cJSON item */
  cJSON *cJSON_Duplicate(const char *who, const cJSON *item, bool recurse);
  /* Duplicate will create a new, identical cJSON item to the one you pass, in new memory that will
   * need to be released. With recurse!=0, it will duplicate any children connected to the item.
   * The item->next and ->prev pointers are always zero on return from Duplicate. */
  /* Recursively compare two cJSON items for equality. If either a or b is NULL or invalid, they will be considered unequal.
   * case_sensitive determines if object keys are treated case sensitive (1) or case insensitive (0) */
  bool cJSON_Compare(const cJSON *const a, const cJSON *const b, const bool case_sensitive);

  /* Minify a strings, remove blank characters(such as ' ', '\t', '\r', '\n') from strings.
   * The input pointer json cannot point to a read-only address area, such as a string constant,
   * but should point to a readable and writable address area. */
  void cJSON_Minify(char *json);

  /* Helper functions for creating and adding items to an object at the same time.
   * They return the added item or NULL on failure. */
  cJSON *cJSON_AddNullToObject(const char *who, cJSON *const object, const char *const name);
  cJSON *cJSON_AddTrueToObject(const char *who, cJSON *const object, const char *const name);
  cJSON *cJSON_AddFalseToObject(const char *who, cJSON *const object, const char *const name);
  cJSON *cJSON_AddBoolToObject(const char *who, cJSON *const object, const char *const name, const bool boolean);
  cJSON *cJSON_AddNumberToObject(const char *who, cJSON *const object, const char *const name, const double number);
  cJSON *cJSON_AddStringToObject(const char *who, cJSON *const object, const char *const name, const char *const string);
  cJSON *cJSON_AddRawToObject(const char *who, cJSON *const object, const char *const name, const char *const raw);
  cJSON *cJSON_AddObjectToObject(const char *who, cJSON *const object, const char *const name);
  cJSON *cJSON_AddArrayToObject(const char *who, cJSON *const object, const char *const name);

  /* When assigning an integer value, it needs to be propagated to valuedouble too. */
#define cJSON_SetIntValue(object, number) ((object) ? (object)->valueint = (object)->valuedouble = (number) : (number))
  /* helper for the cJSON_SetNumberValue macro */
  double cJSON_SetNumberHelper(cJSON *object, double number);
#define cJSON_SetNumberValue(object, number) ((object != NULL) ? cJSON_SetNumberHelper(object, (double)number) : (number))
  /* Change the valuestring of a cJSON_String object, only takes effect when type of object is cJSON_String */
  char *cJSON_SetValuestring(const char *who, cJSON *object, const char *valuestring);

  /* If the object is not a boolean type this does nothing and returns cJSON_Invalid else it returns the new type*/
#define cJSON_SetBoolValue(object, boolValue) ( \
    (object != NULL && ((object)->type & (cJSON_False | cJSON_True))) ? (object)->type = ((object)->type & (~(cJSON_False | cJSON_True))) | ((boolValue) ? cJSON_True : cJSON_False) : cJSON_Invalid)

/* Macro for iterating over an array or object */
#define cJSON_ArrayForEach(element, array) for (element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)

  /* Krishna */
  cJSON *cJSON_ParseWithRef(const char *who, const char *value);
  cJSON *cJSON_ParseWithRefWithLength(const char *who, const char *value, size_t buffer_length);
  cJSON *cJSON_ParseWithRefOpts(const char *who, const char *value, const char **return_parse_end, bool require_null_terminated);
  cJSON *cJSON_ParseWithRefWithLengthOpts(const char *who, const char *value, size_t buffer_length, const char **return_parse_end, bool require_null_terminated);

  cJSON *cJSON_AddStringToObjectWithRef(const char *who, cJSON *const object, const char *const name, const char *const string);
  cJSON *cJSON_AddNumberToObjectWithRef(const char *who, cJSON *const object, const char *const name, const double number);
  cJSON *cJSON_AddObjectToObjectWithRef(const char *who, cJSON *const object, const char *const name);

  int cJSON_EstimatePrintLength(const char *who, cJSON *item);
#ifdef __cplusplus
}
#endif

#endif
