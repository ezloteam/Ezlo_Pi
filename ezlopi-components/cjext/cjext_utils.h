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

#ifndef cJSON_Utils__h
#define cJSON_Utils__h

#ifdef __cplusplus
extern "C"
{
#endif

#include "cjext.h"
#include "EZLOPI_USER_CONFIG.h"

  /* Implement RFC6901 (https://tools.ietf.org/html/rfc6901) JSON Pointer spec. */
  cJSON * cJSONUtils_GetPointer(cJSON* const object, const char* pointer);
  cJSON * cJSONUtils_GetPointerCaseSensitive(cJSON* const object, const char* pointer);

  /* Implement RFC6902 (https://tools.ietf.org/html/rfc6902) JSON Patch spec. */
  /* NOTE: This modifies objects in 'from' and 'to' by sorting the elements by their key */
  cJSON * cJSONUtils_GeneratePatches(const char * who, cJSON* const from, cJSON* const to);
  cJSON * cJSONUtils_GeneratePatchesCaseSensitive(const char * who, cJSON* const from, cJSON* const to);
  /* Utility for generating patch array entries. */
  void cJSONUtils_AddPatchToArray(const char * who, cJSON* const array, const char* const operation, const char* const path, const cJSON* const value);
  /* Returns 0 for success. */
  int cJSONUtils_ApplyPatches(const char * who, cJSON* const object, const cJSON* const patches);
  int cJSONUtils_ApplyPatchesCaseSensitive(const char * who, cJSON* const object, const cJSON* const patches);

  /*
  // Note that ApplyPatches is NOT atomic on failure. To implement an atomic ApplyPatches, use:
  //int cJSONUtils_AtomicApplyPatches(cJSON **object, cJSON *patches)
  //{
  //    cJSON *modme = cJSON_Duplicate(*object, 1);
  //    int error = cJSONUtils_ApplyPatches(modme, patches);
  //    if (!error)
  //    {
  //        cJSON_Delete(*object);
  //        *object = modme;
  //    }
  //    else
  //    {
  //        cJSON_Delete(modme);
  //    }
  //
  //    return error;
  //}
  // Code not added to library since this strategy is a LOT slower.
  */

  /* Implement RFC7386 (https://tools.ietf.org/html/rfc7396) JSON Merge Patch spec. */
  /* target will be modified by patch. return value is new ptr for target. */
  cJSON * cJSONUtils_MergePatch(const char * who, cJSON* target, const cJSON* const patch);
  cJSON * cJSONUtils_MergePatchCaseSensitive(const char * who, cJSON* target, const cJSON* const patch);
  /* generates a patch to move from -> to */
  /* NOTE: This modifies objects in 'from' and 'to' by sorting the elements by their key */
  cJSON * cJSONUtils_GenerateMergePatch(const char * who, cJSON* const from, cJSON* const to);
  cJSON * cJSONUtils_GenerateMergePatchCaseSensitive(const char * who, cJSON* const from, cJSON* const to);

  /* Given a root object and a target object, construct a pointer from one to the other. */
  char *cJSONUtils_FindPointerFromObjectTo(const char * who, const cJSON* const object, const cJSON* const target);

  /* Sorts the members of the object into alphabetical order. */
  void cJSONUtils_SortObject(cJSON* const object);
  void cJSONUtils_SortObjectCaseSensitive(cJSON* const object);

#ifdef __cplusplus
}
#endif

#endif
