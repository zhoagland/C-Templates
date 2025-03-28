cmake_minimum_required(VERSION 3.10) # Or a more recent version

# Project name
project(C_Class_Template)

# Set C standard (optional, but recommended)
set(CMAKE_C_STANDARD 11) # Or whatever standard you want
set(CMAKE_C_STANDARD_REQUIRED TRUE)

# Find include directories (if you have them in a separate folder)
include_directories(src/include) # Adjust if needed

# Source files (list all your .c files)
set(SOURCE_FILES
    src/main.c
    src/child_class.c
    src/parent_class.c
    # Add other .c files here
)

# Executable creation
add_executable(C_Class_Template ${SOURCE_FILES})

# Link libraries (if needed)
# target_link_libraries(myprogram m) # For math library, for example
# target_link_libraries(myprogram your_library) # for your own or 3rd party libraries.
# If you have libraries in a specific directory:
# link_directories(lib) # or where your libraries are located.
# target_link_libraries(myprogram -L/path/to/libs -lmylib) # alternative linking.

# Set compiler flags (optional)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g") # -g for debug

# Install target (optional, for installing the executable)
# install(TARGETS myprogram DESTINATION bin) # Installs the executable to the 'bin' directory.