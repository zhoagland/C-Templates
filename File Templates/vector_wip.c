/**
* @file template.c
* @author your name (you@domain.com)
* @brief
* @version 0.1
* @date 2025-04-03
*
* @copyright Copyright (c) 2025
*
*/
/* -------------------- Private Includes ------------------------------------------- */
#include "vector.h" /* Used to expose the vector API. */
#include <stdio.h> /* Used for io */
#include <stdlib.h> /* Used for memory allocation */

/* -------------------- Private Macros/Defines ------------------------------------- */

/** Vector virtual table definition macro. */
#define VECTOR_VTABLE_INIT(data_type) \
static vector_##data_type##_vtbl_t vector_##data_type##_vtable = { \
        .push = vector_##data_type##_push, \
        .pop = vector_##data_type##_pop, \
        .get = vector_##data_type##_get, \
};

/** Vector function declarations macro. */
#define VECTOR_STATIC_FUNCTIONS_DECLARE(data_type) \
    static int vector_##data_type##_push(vector_##data_type##_t *vector, data_type value); \
    static int vector_##data_type##_pop(vector_##data_type##_t *vector); \
    static int vector_##data_type##_get(vector_##data_type##_t *vector, int index, data_type *out);



/** Macro to generate vector create function */
#define GENERIC_VECTOR_CREATE_FUNC(data_type) \
    vector_##data_type##_t *vector_##data_type##_create(int initial_capacity) { \
        vector_##data_type##_t *vector = (vector_##data_type##_t *)malloc(sizeof(vector_##data_type##_t)); \
        if (vector == NULL) { \
            return NULL; \
        } \
        vector->size = 0; \
        vector->capacity = initial_capacity; \
        vector->vptr = &vector_##data_type##_vtable; \
        vector->data = (data_type *)malloc(initial_capacity * sizeof(data_type)); \
        if (vector->data == NULL) { \
            free(vector); \
            return NULL; \
        } \
        VECTOR_MUTEX_INIT(&vector->lock); /* Initialize lock */ \
        return vector; \
    }

/** Macro to generate vector destroy function */
#define GENERIC_VECTOR_DESTROY_FUNC(data_type) \
    int vector_##data_type##_destroy(vector_##data_type##_t *vector) { \
        if (vector == NULL) { \
            return -1; \
        } \
        VECTOR_MUTEX_DESTROY(&vector->lock); /* Delete lock */ \
        free(vector->data); \
        free(vector); \
        return 0; \
    }

/** Macro to generate vector push function */
#define GENERIC_VECTOR_PUSH_FUNC(data_type) \
    int vector_##data_type##_push(vector_##data_type##_t *vector, data_type value) { \
        if (vector == NULL) { \
            return -1; \
        } \
        VECTOR_MUTEX_LOCK(&vector->lock); \
        if (vector->size >= vector->capacity) { \
            int new_capacity = vector->capacity * 2; \
            data_type *new_data = (data_type *)realloc(vector->data, new_capacity * sizeof(data_type)); \
            if (new_data == NULL) { \
                VECTOR_MUTEX_UNLOCK(&vector->lock); \
                return -1; \
            } \
            vector->data = new_data; \
            vector->capacity = new_capacity; \
        } \
        vector->data[vector->size] = value; \
        vector->size++; \
        VECTOR_MUTEX_UNLOCK(&vector->lock); \
        return 0; \
    }

/** Macro to generate vector pop function */
#define GENERIC_VECTOR_POP_FUNC(data_type) \
    int vector_##data_type##_pop(vector_##data_type##_t *vector) { \
        if (vector == NULL) { \
            return -1; \
        } \
        VECTOR_MUTEX_LOCK(&vector->lock); \
        if (vector->size == 0) { \
            VECTOR_MUTEX_UNLOCK(&vector->lock); \
            return -1; \
        } \
        vector->size--; \
        VECTOR_MUTEX_UNLOCK(&vector->lock); \
        return 0; \
    }

/** Macro to generate vector get function */
#define GENERIC_VECTOR_GET_FUNC(data_type) \
    int vector_##data_type##_get(vector_##data_type##_t *vector, int index, data_type *out) { \
        if (!vector || !out) \
            return -1; \
        VECTOR_MUTEX_LOCK(&vector->lock); \
        if (index < 0 || index >= vector->size) { \
            VECTOR_MUTEX_UNLOCK(&vector->lock); \
            return -1; \
        } \
        *out = vector->data[index]; \
        VECTOR_MUTEX_UNLOCK(&vector->lock); \
        return 0; \
    }

/** Macro to generate vector function declarations */
#define GENERIC_VECTOR_FUNCTIONS(data_type) \
    VECTOR_STATIC_FUNCTIONS_DECLARE(data_type) \
    VECTOR_VTABLE_INIT(data_type) \
    GENERIC_VECTOR_CREATE_FUNC(data_type) \
    GENERIC_VECTOR_DESTROY_FUNC(data_type) \
    GENERIC_VECTOR_PUSH_FUNC(data_type) \
    GENERIC_VECTOR_POP_FUNC(data_type) \
    GENERIC_VECTOR_GET_FUNC(data_type)


GENERIC_VECTOR_FUNCTIONS(int)
GENERIC_VECTOR_FUNCTIONS(double)
GENERIC_VECTOR_FUNCTIONS(char)
GENERIC_VECTOR_FUNCTIONS(uint8_t)
GENERIC_VECTOR_FUNCTIONS(uint16_t)
GENERIC_VECTOR_FUNCTIONS(uint32_t)
GENERIC_VECTOR_FUNCTIONS(uint64_t)

