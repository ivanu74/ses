#ifndef JSON_H
#define JSON_H

#include <stdlib.h>
#include <stdio.h>

/*
 * Don't forget to call free().
 */
char* str_escape(const char* src);

#define IF_PRINT_JSON if (print_json)

#define IF_PRINT_NONE_JSON if (!print_json)

#define JSON_HEADER IF_PRINT_JSON printf("{\n")

#define JSON_ENDING IF_PRINT_JSON printf("\n}\n")

#define CASE_JSON case 'j': print_json = 1; break

#define PRINT_JSON_ITEM(k, f, v) IF_PRINT_JSON \
  printf("\"%s\": \"" f "\", ", (k), (v))
#define PRINT_JSON_LAST_ITEM(k, f, v) IF_PRINT_JSON \
  printf("\"%s\": \"" f "\"", (k), (v))

#define PRINT_JSON_ITEM_UNIT(k, f, v, u) IF_PRINT_JSON \
  printf("\"%s\": \"" f "%s\", ", (k), (v), (u))
#define PRINT_JSON_LAST_ITEM_UNIT(k, f, v, u) IF_PRINT_JSON \
  printf("\"%s\": \"" f "%s\"", (k), (v), (u))

#define PRINT_JSON_MORE_ITEM IF_PRINT_JSON printf(", ")

#define PRINT_JSON_GROUP_HEADER(k) IF_PRINT_JSON printf("\"%s\": {", (k))
#define PRINT_JSON_GROUP_ENDING IF_PRINT_JSON printf("}")
#define PRINT_JSON_MORE_GROUP IF_PRINT_JSON printf(",\n")

extern int do_new_item;
extern int print_json;

#define PRINT_JSON_GROUP_SEPARATE if (do_new_item) PRINT_JSON_MORE_GROUP ; \
  do_new_item = 1

#define PRINT_JSON_RESET_GROUP do_new_item = 0

#endif
