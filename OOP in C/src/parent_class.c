/**
 * @file parent_class.c
 * @author Zachary Hoagland (zahoagland@gmail.com)
 * @brief Template parent class, or class with no children, for OOP C programs
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright Zachary Hoagland (c) 2025
 * 
 */

/* Include Files */
#include "parent_class.h"
#include <stdio.h>

/* Function Definitions */
static void example_method_(void *const pVoid);

/**
 * @brief Example Constructor definition
 * 
 * @param[in] self Pointer to the struct object.
 * @param[in] example_int Example member data to be set.
 * @param[in] example_double Example member data to be set.
 */
void ParentClass_ctor(ParentClass_t *const self, int example_int, double example_double){

    /* Create an instance of the virtual function ptr table */
    static const ParentClassVtbl_t vtbl = {
        &example_method_, /* More methods would be listed if there were some. */
    };

    /* Associate the vtbl with the struct by assigning vptr with the address of vtbl. */
    self->vptr = &vtbl;

    /* Load any default member values assigned in the constructor. */
    self->IntTypeData = example_int;
    self->DoubleTypeData = example_double;

    return;
}

/**
 * @brief Example method definition.
 * 
 * Methods can also have return types but the vtbl function ptr
 * needs to have the right signature. 
 * 
 * @param pVoid Pointer to the struct object.
 */
void example_method(void *const pVoid){

    /* Typecasting the void pointer to a pointer of the parent class that is assigned to self. */
    ParentClass_t *const self = (ParentClass_t *)pVoid;

    /* Now that the pointer is of type parent class we can access the vtbl and call the base method. */
    self->vptr->example_method(pVoid);

    /* We return void. If */
    return;
}

/**
 * @brief Default implementation of example method definition.
 * 
 * @param pVoid Pointer to the struct object.
 */
static void example_method_(void *const pVoid){
    
    /* Manipulate data here */
    printf("Default Method Ran\n");
    return;
}