#ifndef MACROS_H
#define MACROS_H

#include <stdlib.h>
#include <err.h>

#define CHECK_FAIL(cond, ...) if (cond) errx(EXIT_FAILURE, __VA_ARGS__)
#define CHECK_ALLOC(varName, name) CHECK_FAIL(varName == NULL, "Could not allocate memory for %s.", name)

#define FREE_2D_ARRAY(arrayName, length) for (size_t i=0; i < length; ++i) {free(arrayName[i]);} free(arrayName)

#endif