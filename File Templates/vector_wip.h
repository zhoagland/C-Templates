/**
 * @file vector.h
 * @author Zachary Hoagland - C63928 (zachary.hoagland@microchip.com)
 * @brief
 * @version 0.1
 * @date 2025-04-03
 *
 * @copyright Microchip Technology Inc. Copyright (c) 2025
 *
 */
#ifndef _VECTOR_H_
#define _VECTOR_H_


#ifdef __cplusplus
    extern "C" {
#endif


/* -------------------- Public Includes --------------------------------- */

#include <stdint.h> /* For int types */

/* -------------------- Public Macros/Defines --------------------------- */

/*
TODO Functions to add
insert
remove_at
clear
find
copy
shrink_to_fit

TODO Make error system a lot more robust.
*/

/* If Windows */
#ifdef _WIN32
    #include <windows.h>
    #define VECTOR_MUTEX_TYPE CRITICAL_SECTION
    #define VECTOR_MUTEX_INIT(p_mutex_address) InitializeCriticalSection((p_mutex_address))
    #define VECTOR_MUTEX_DESTROY(p_mutex_address) DeleteCriticalSection((p_mutex_address))
    #define VECTOR_MUTEX_LOCK(p_mutex_address) EnterCriticalSection((p_mutex_address))
    #define VECTOR_MUTEX_UNLOCK(p_mutex_address) LeaveCriticalSection((p_mutex_address))
#else /* If POSIX-like system */
    #include <pthread.h>
    #define VECTOR_MUTEX_TYPE pthread_mutex_t
    #define VECTOR_MUTEX_INIT(p_mutex_address) pthread_mutex_init((p_mutex_address), NULL)
    #define VECTOR_MUTEX_DESTROY(p_mutex_address) pthread_mutex_destroy((p_mutex_address))
    #define VECTOR_MUTEX_LOCK(p_mutex_address) pthread_mutex_lock((p_mutex_address))
    #define VECTOR_MUTEX_UNLOCK(p_mutex_address) pthread_mutex_unlock((p_mutex_address))
#endif

/** Vector virtual table definition macro. */
#define VECTOR_VTBL_T(data_type) \
	typedef struct _vector_##data_type##_vtbl { \
	    int (*const push)(vector_##data_type##_t *vector, data_type value); 		        /**< Push a value to the back of the vector */ \
	    int (*const pop)(vector_##data_type##_t *vector);                    		        /**< Pop a value from the back of the vector */ \
	    int (*const get)(vector_##data_type##_t *vector, int index, data_type *out);        /**< Get a value from the vector at index */ \
	} vector_##data_type##_vtbl_t;

/** Vector structure definition macro. */
#define VECTOR_T(data_type) \
	struct _vector_##data_type { \
	    vector_##data_type##_vtbl_t *vptr;  /**< Pointer to the virtual table */ \
	    int size;                           /**< Size of the vector */ \
	    int capacity;                       /**< Capacity of the vector */ \
	    data_type *data;                    /**< Pointer to the data array */ \
	    VECTOR_MUTEX_TYPE lock; 			/** Mutex for thread safety */ \
	};

/** Vector create function */
#define VECTOR_CREATE_FUNC(data_type) \
    vector_##data_type##_t *vector_##data_type##_create(int initial_capacity);

/** Vector destroy function */
#define VECTOR_DESTROY_FUNC(data_type) \
    int vector_##data_type##_destroy(vector_##data_type##_t *vector);

/** Vector public function declarations macro. */
#define VECTOR_PUBLIC_FUNCTIONS_DECLARE(data_type) \
    VECTOR_CREATE_FUNC(data_type) \
    VECTOR_DESTROY_FUNC(data_type)

/** Vector structure and virtual table definition macro. */
#define VECTOR_DATA_STRUCTURE(data_type) \
	typedef struct _vector_##data_type vector_##data_type##_t; \
	VECTOR_PUBLIC_FUNCTIONS_DECLARE(data_type)\
    VECTOR_VTBL_T(data_type) \
    VECTOR_T(data_type)

/* -------------------- Public Enums ------------------------------------ */

/* -------------------- Public Structs ---------------------------------- */

VECTOR_DATA_STRUCTURE(int)
VECTOR_DATA_STRUCTURE(double)
VECTOR_DATA_STRUCTURE(char)
VECTOR_DATA_STRUCTURE(uint8_t)
VECTOR_DATA_STRUCTURE(uint16_t)
VECTOR_DATA_STRUCTURE(uint32_t)
VECTOR_DATA_STRUCTURE(uint64_t)

/* -------------------- Public (global) Vars ---------------------------- */


/* -------------------- Public Function Declarations -------------------- */

/**
 * @brief Create a new vector. Supports int, double, char, uint8_t, uint16_t, uint32_t, and uint64_t.
 *
 * @param data_type The c type of the vector.
 * @param variable_name Name of the variable to create.
 * @param initial_capacity Initial capacity of the vector.
 * @return vector_<data_type>_t* Pointer to the newly created vector.
 */
#define vector_create(data_type, variable_name, initial_capacity) \
    vector_##data_type##_t *variable_name = vector_##data_type##_create((initial_capacity))

/**
 * @brief Destroy a vector. Supports int, double, char, uint8_t, uint16_t, uint32_t, and uint64_t.
 *
 * @param data_type The c type of the vector.
 * @param vector The vector to destroy.
 * @return int 0 on success, -1 on failure.
 */
#define vector_destroy(data_type, vector) \
    vector_##data_type##_destroy(vector)



#ifdef __cplusplus
    }
#endif

#endif /* _VECTOR_H_ */