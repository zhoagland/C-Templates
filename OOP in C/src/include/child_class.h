/**
 * @file child_class.h
 * @author Zachary Hoagland (zahoagland@gmail.com)
 * @brief Template Interface for child class, or a class that inherits from another class, for OOP C programs
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright Zachary Hoagland(c) 2025
 * 
 */

#ifndef CHILD_CLASS_H
#define CHILD_CLASS_H

#ifdef __cplusplus
    extern "C" {
#endif

/* Include Files */
#include "parent_class.h"

/* Struct Declarations */

/**
 * @brief Child Class Virtual Function Table
 * 
 * Virtual functions are member functions that can be
 * redefined in child classes
 * 
 */
typedef struct ChildClassVtbl{
    void(*example_child_method)(void *const pVoid);   //! Example method for the class
} ChildClassVtbl_t;

/**
 * @brief Child Class Template
 * 
 */
typedef struct ChildClass {
    ParentClass_t parent;           //! Class this type inherits from.
    const ChildClassVtbl_t *vptr;   //! ptr to classes specific methods vtbl.
    int IntTypeData2;               //! Example member of type Int exclusive to this object.
    double DoubleTypeData2;         //! Example member of type Double exclusive to this object.
} ChildClass_t;

/* Function Declarations */

/**
 * @brief Example Constructor declaration
 * 
 * @param[in] self Pointer to the struct object.
 * @param[in] example_int Example member data to be set.
 * @param[in] example_double Example member data to be set.
 */
void ChildClass_ctor(ChildClass_t *const self, int example_int, int example_int_child, double example_double, double example_double_child);

/**
 * @brief Example method declaration.
 * 
 * Methods can also have return types but the vtbl function ptr
 * needs to have the right signature. 
 * 
 * @param pVoid Void pointer to the struct object.
 */
void example_method(void *const pVoid);

/**
 * @brief Example method declaration.
 * 
 * Methods can also have return types but the vtbl function ptr
 * needs to have the right signature. 
 * 
 * @param pVoid Void pointer to the struct object.
 */
void example_child_method(void *const pVoid);

#ifdef __cplusplus
    }
#endif

#endif /* CHILD_CLASS_H */