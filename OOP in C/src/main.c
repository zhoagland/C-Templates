/**
 * @file main.c
 * @author Zachary Hoagland (zahoagland@gmail.com)
 * @brief Template main for OOP C programs
 * @version 0.1
 * @date 2025-03-26
 * 
 * @copyright Copyright Zachary Hoagland (c) 2025
 * 
 */


/* Include Files */
#include <stdlib.h>
#include "parent_class.h"
#include "child_class.h"

/**
 * @brief Entry point of the application
 * 
 * @param[in] argc Number of command line arguments.
 * @param[in] argv Array of pointers to the command line argument strings.
 * @return exit status 
 */
int main(int argc, char *argv[]) {

    ParentClass_t childless_class;

    ParentClass_ctor(&childless_class, 1, 1.0);
    example_method((void*)&childless_class);

    ChildClass_t child_class;

    ChildClass_ctor(&child_class, 1, 1.0, 2, 2.0);
    example_method((void*)&child_class);
    example_child_method((void*)&child_class);

    
    return EXIT_SUCCESS;
}