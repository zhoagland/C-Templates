/**
 * @file parent_class.h
 * @author Zachary Hoagland (zahoagland@gmail.com)
 * @brief Template Interface for parent class, or class with no children, for OOP C programs
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright Zachary Hoagland(c) 2025
 * 
 */

#ifndef PARENT_CLASS_H
#define PARENT_CLASS_H

#ifdef __cplusplus
    extern "C" {
#endif

/* Struct Declarations */

/**
 * @brief Parent Class Virtual Function Table
 * 
 * Virtual functions are member functions that can be
 * redefined in child classes
 * 
 */
typedef struct ParentClassVtbl{
    void(*example_method)(void *const pVoid);   //! Example method for the class
} ParentClassVtbl_t;

/**
 * @brief Parent Class Template
 * 
 */
typedef struct ParentClass {
    const ParentClassVtbl_t *vptr;  //! ptr to the classes vtbl
    int IntTypeData;                //! Example member of type Int.
    double DoubleTypeData;          //! Example member of type Double.
} ParentClass_t;

/* Function Declarations */

/**
 * @brief Example Constructor declaration
 * 
 * @param[in] self Pointer to the struct object.
 * @param[in] example_int Example member data to be set.
 * @param[in] example_double Example member data to be set.
 */
void ParentClass_ctor(ParentClass_t *const self, int example_int, double example_double);

/**
 * @brief Example method declaration.
 * 
 * Methods can also have return types but the vtbl function ptr
 * needs to have the right signature. 
 * 
 * @param pVoid Void pointer to the struct object.
 */
void example_method(void *const pVoid);

#ifdef __cplusplus
    }
#endif

#endif /* PARENT_CLASS_H */