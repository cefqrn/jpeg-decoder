#ifndef MACROS_H
#define MACROS_H

#include <err.h>

#define CHECK_ALLOC(varName, name) if (varName == NULL) errx(EXIT_FAILURE, "Could not allocate memory for %s.", name);

#endif