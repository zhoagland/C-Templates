/**
 * @file child_class.c
 * @author Zachary Hoagland (zahoagland@gmail.com)
 * @brief Template child class, or class with no children, for OOP C programs
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright Zachary Hoagland (c) 2025
 * 
 */

/* Include Files */
#include "child_class.h"
#include "parent_class.h"
#include <stdio.h>

/* Function Definitions */
static void example_method_override_(void *const pVoid);

/**
 * @brief Example Constructor definition
 * 
 * @param[in] self Pointer to the struct object.
 * @param[in] example_int Example member data to be set.
 * @param[in] example_double Example member data to be set.
 */
void ChildClass_ctor(ChildClass_t *const self, int example_int, int example_int_child, double example_double, double example_double_child){

    /* Override the parent virtual function ptr table if you want. */
    static const ParentClassVtbl_t parent_vtbl = {
        &example_method_override_, /* Example of a method override. Every method needs to be overwritten */
    };

    ParentClass_ctor(&self->parent, example_int, example_double);
    
    /* gets overriden here. */
    self->parent.vptr=&parent_vtbl;

    /* Create an instance of the virtual function ptr table for class specific methods. */
    static const ChildClassVtbl_t vtbl = {
        &example_child_method, /* More methods would be listed if there were some. */
    };

    /* Associate the vtbl with the struct by assigning vptr with the address of vtbl. */
    self->vptr = &vtbl;

    /* Load any default member values assigned in the constructor. */
    self->IntTypeData2 = example_int_child;
    self->DoubleTypeData2 = example_double_child;

    return;
}

/**
 * @brief Default implementation of example method definition.
 * 
 * @param pVoid Pointer to the struct object.
 */
static void example_method_override_(void *const pVoid){
    
    /* Manipulate data here */
    printf("Overriden Default Method Ran\n");
    return;
}

/**
 * @brief Default implementation of example method definition.
 * 
 * @param pVoid Pointer to the struct object.
 */
void example_child_method(void *const pVoid){
    
    /* Manipulate data here */
    printf("Child Method Ran\n");
    return;
}