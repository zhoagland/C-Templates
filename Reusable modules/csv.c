/**
* @file csv.c
* @author Zachary Hoagland (zachary.hoagland@mircochip.com)
* @brief Implementation for CSV file storage functions
* @version 0.1
* @date 2025-04-03
* 
* @copyright Copyright (c) 2025
* 
*/
/* -------------------- Private Includes -------------------- */
#include "csv.h"
#include "file_io.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

/* -------------------- Private Macros/Defines -------------------- */

/** definition for the length of a csv file extension including the period. */
#define EXTENSION_LENGTH            (4)
/** definition for the max number of current csv files that can be open. */
#define MAX_CONCURRENT_CSV_FILES    (1)
/** definition of a macro for the csv file extension string. */
#define CSV_EXTENSION_STRING        ".csv"

/* -------------------- Private Enums -------------------- */

/* -------------------- Private Structs -------------------- */

/**
 * @brief Collection of data for a csv file.
 * 
 */
typedef struct _csv_file {
    FILE *p_file;                            /**< File pointer to the csv file. */
    size_t number_of_rows;                   /**< Current number of rows in the csv file. */
    size_t number_of_columns;                /**< Number of columns to make the csv file. */
    char absolute_path[FILE_PATH_LENGTH];    /**< Copy of the absolute file path */
} csv_file_t;

/* -------------------- Private (static) Vars -------------------- */

/** Array of structs for holding the information for open csv files. */
static csv_file_t s_csv_files[MAX_CONCURRENT_CSV_FILES] = {0};
/** Counter for the number of files currently open. */
static unsigned int s_number_of_open_files = 0;
/** Array of flags to know which of the indexes in the struct array is free to use. */
static unsigned int s_free_indexes[MAX_CONCURRENT_CSV_FILES] = {0};

/* -------------------- Private (static) Function Declarations */

static int check_for_extension(const char* filename);
static int calculate_row_count(int csv_file_handle);
static int calculate_column_count(int csv_file_handle);
static int convert_handle_to_index(int csv_file_handle);
static void go_to_row(int csv_file_handle, int row);
static void go_to_column(int csv_file_handle, int column);

/* -------------------- Public (global) Vars -------------------- */

extern int errno;

/* -------------------- Private and Public Function Definitions -------------------- */

/**
 * @brief Create a csv file
 * 
 * @param absolute_path_to_file Absolute path to the csv file to be created
 * @param number_of_columns Number of columns for the csv file
 * @return 0 on success, errno on fail.
 */
int create_csv_file(const char *absolute_path_to_file, int number_of_columns) {

    int file_handle = 0;

    /* 
        Create file with open csv a+ will open for append if file exist or create a file if it doesn't.
        Also checking to make sure it open the file correctly and a handle was assigned. If not return
        an "error code".
    */
    if((file_handle = open_csv_file(absolute_path_to_file)) <= 0) {
        return errno;
    }

    /* Set the column count because it is a new file. */
    set_column_count(file_handle, number_of_columns);
    /* Making sure that the row count is set to zero since, again, it is an empty file. */
    set_row_count(file_handle, 0);

    /* Return the file handle for the user. */
    return file_handle;
}

/**
 * @brief Opens an already existing csv file
 * 
 * @param absolute_path_to_file Absolute path of the file to be opened.
 * @return 0 on success, errno on fail.
 */
int open_csv_file(const char *absolute_path_to_file) {

    int next_free_index = 0;

    /* Check to see if we have room to open any more csv files. If not return an invalid handle. */
    if ((s_number_of_open_files+1) > MAX_CONCURRENT_CSV_FILES) {
        errno = ENFILE;
        return errno;
    }

    /* Loop through the indexes of the array that stores whether or not an index is free in the main csv array. */
    for (; next_free_index < MAX_CONCURRENT_CSV_FILES; next_free_index++) {
        /* If an index is free return it and set it as occupied */
        if(s_free_indexes[next_free_index] == 0) {
            s_free_indexes[next_free_index] = 1;
            break;
        }
        else {
            continue;
        }
    }

    /* Open the csv file and check it opened successfully. */
    if((s_csv_files[next_free_index].p_file = fopen(absolute_path_to_file, "a+")) == NULL) {
        return errno;
    }

	/* Update number of files open */
	s_number_of_open_files++;	
    /* store the column count for later use. */
    s_csv_files[next_free_index].number_of_rows = calculate_row_count(next_free_index+1);
    /* store the current row count for later use. */
    s_csv_files[next_free_index].number_of_columns = calculate_column_count(next_free_index+1);
    /* store the file path. */
    strcpy(s_csv_files[next_free_index].absolute_path, absolute_path_to_file);

    /* Return the handle. (Array index plus 1. a handle of 0 is invalid. )*/
    return (next_free_index+1);
}

/**
 * @brief Closes a csv file from reading and writing.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return 0 on success, errno on fail.
 */
int close_csv_file(int csv_file_handle) {

    /* Realign the csv handle to the actual index into the csv files array. */
    int csv_file_index = convert_handle_to_index(csv_file_handle);    

    /* Check that we have a valid handle and that we have files that are open. */
    if ( (csv_file_index < 0) || (s_number_of_open_files-1 < 0)) {
        errno = ENOENT;
        return errno;
    }

    /* Decrement the amount of open files. */
    s_number_of_open_files--;
    /* Mark the index as free. */
    s_free_indexes[csv_file_index] = 0;

    /* Close the csv file and check it closed successfully. */
    if(fclose(s_csv_files[csv_file_index].p_file)){
        return errno;
    }

    /* Reset the member values in the struct. */
    s_csv_files[csv_file_index].p_file = NULL;
    s_csv_files[csv_file_index].number_of_columns = 0;
    s_csv_files[csv_file_index].number_of_rows = 0;
    sprintf(s_csv_files[csv_file_index].absolute_path, "");

    return 0;
}

/**
 * @brief Updates the contents of a cell in a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param data_to_insert Pointer to the string of data to insert.
 * @param cell Cell struct that specifies the location to update.
 * @return 0 on success, errno on fail.
 */
int update_cell(int csv_file_handle, const char *data_to_insert, cell_t cell) {

    long splice_offset = 0;
    FILE *p_temp_file = NULL;
    char read_char = 0;
    char temp_file_name[FILE_PATH_LENGTH] = {0};
	
	/* If row doesn't exist, append empty rows until the row count is correct */
	for ( size_t row = get_row_count(csv_file_handle); row < cell.row; row++) {
		append_row(csv_file_handle, 0, NULL);
	}
	
    /* Move cursor in file to where the insertion should be. */    
    go_to_row(csv_file_handle, cell.row);
    go_to_column(csv_file_handle, cell.column);

    /* Save the spot of the cursor. */
    splice_offset = ftell(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
    /* Create a temp file to copy old data and insert new data into. */
    sprintf(temp_file_name, "%stemp", s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path);
    
    if((p_temp_file = fopen(temp_file_name, "w+")) == NULL) {
        return errno;
    }

    /* Reset cursor position to start of file. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);

    /* Copy the old data into the new file until the splice mark is hit or EOF is reached. */
    while ((ftell(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file) != splice_offset) && ((read_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != EOF)) {
        fprintf(p_temp_file, "%c", read_char);
    }


    /* Insert new cell data. */
    fprintf(p_temp_file, "%s,", data_to_insert);


    /* Fast forward in the file until we reach the end of the cell */
    while (((read_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != ',') && read_char != EOF) {
        continue;
    }

    /* Write out the rest of the data into the new file. */
    while ((read_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != EOF) {
        fprintf(p_temp_file, "%c", read_char);
    }
    
    /* Close the files and assign the pointers null */
    fclose(p_temp_file);
    p_temp_file = NULL;
    fclose(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
    s_csv_files[convert_handle_to_index(csv_file_handle)].p_file = NULL;

    /* Remove old file. */
	if(remove(s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path)) {
        return errno;
    }
    
    /* Rename temp file to the old file to "save" the changes. */
    if(rename(temp_file_name, s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path)) {
        return errno;   
    }

    /* Reopen the file so that the user can still interact with it. */
    if((s_csv_files[convert_handle_to_index(csv_file_handle)].p_file = fopen(s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path, "a+")) == NULL) {
        return errno;
    }
    
    return 0;
}

/**
 * @brief Clears the contents of a cell in a csv file. 
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param cell Cell struct that specifies the location to clear.
 * @return 0 on success, errno on fail.
 */
inline int clear_cell(int csv_file_handle, cell_t cell) {
    return update_cell(csv_file_handle, "", cell);
}

/**
 * @brief Updates the contents of a row in a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param row The row in which to update (0 based index).
 * @param width_of_string The memory length allocated for the string.
 * @param data_array_to_insert Pointer to a 2D array of strings containing the data to insert. The length should match the csv column width.
 * @return 0 on success, errno on fail.
 */
int update_row(int csv_file_handle, int row, int width_of_string, const char *data_array_to_insert) {

    int rv = 0;

    rv = delete_row(csv_file_handle, row);

    if(rv){
        return rv;
    }
    
    rv = insert_row(csv_file_handle, row+1, width_of_string, data_array_to_insert);
    
    return rv;
}

/**
 * @brief Inserts a row of data into a csv file
 * 
 * @param data_array_to_insert Pointer to a 2D array of strings containing the data to insert. The length should match the csv column width.
 * @param row_to_insert_before The row to insert before. -1 indicates insert at the end (append row)
 * @param memory_spacing The offset in memory from the base address to the next string address
 * @return 0 on success, errno on fail.
 */
int insert_row(int csv_file_handle, int row_to_insert_before, int memory_spacing, const char *data_array_to_insert) {

    long splice_offset = 0;
    FILE *p_temp_file = NULL;
    char read_char = 0;
    char temp_file_name[FILE_PATH_LENGTH] = {0};
	
    /* Check to ensure the row is valid. */
    if(row_to_insert_before < -1) {
        row_to_insert_before = 0;
    }

    /* If -1 just append the data. */
    if(row_to_insert_before == -1) {
        append_row(csv_file_handle,memory_spacing, data_array_to_insert);
    }

    /* Move cursor in file to where the insertion should be. */    
    go_to_row(csv_file_handle, row_to_insert_before);
    
    /* Save the spot of the cursor. */
    splice_offset = ftell(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
    /* Create a temp file to copy old data and insert new data into. */
    sprintf(temp_file_name, "%stemp", s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path);
    p_temp_file = fopen(temp_file_name, "w+");

    /* Reset cursor position to start of file. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);

    /* Copy the old data into the new file until the splice mark is hit or EOF is reached. */
    while (((read_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != EOF) && (ftell(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file) != splice_offset)) {
        fprintf(p_temp_file, "%c", read_char);
    }

	/* Move to a new line. */
    fprintf(p_temp_file, "\n");
	
    /* Insert new row data. */
    for (size_t idx = 0; idx < s_csv_files[convert_handle_to_index(csv_file_handle)].number_of_columns; idx++) {
        fprintf(p_temp_file, "%s,", data_array_to_insert+(idx*memory_spacing));
    }
    
    /* End with new line. */
    fprintf(p_temp_file, "\n");

    /* Write out the rest of the data into the new file. */
    while ((read_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != EOF) {
        fprintf(p_temp_file, "%c", read_char);
    }
    
    /* Close the files and assign the pointers null */
    fclose(p_temp_file);
    p_temp_file = NULL;
    fclose(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
    s_csv_files[convert_handle_to_index(csv_file_handle)].p_file = NULL;

    /* Remove old file. */
	if(remove(s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path)) {
        return errno;
    }
    
    /* Rename temp file to the old file to "save" the changes. */
    if(rename(temp_file_name, s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path)) {
        return errno;   
    }

    /* Reopen the file so that the user can still interact with it. */
    if((s_csv_files[convert_handle_to_index(csv_file_handle)].p_file = fopen(s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path, "a+")) == NULL) {

        return errno;
    }

	/* Increment internal row counter. */
    set_row_count(csv_file_handle, get_row_count(csv_file_handle)+1);

    return 0;
}

/**
 * @brief Appends a row of data to a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param memory_spacing The offset in memory from the base address to the next string address
 * @param data_array_to_insert Pointer to a 2D array of strings containing the data to insert. The length should match the csv column width.
 * @return 0 on success, errno on fail.
 */
int append_row(int csv_file_handle, int memory_spacing, const char *data_array_to_insert) {
    
	if(get_row_count(csv_file_handle) != 0) {
		/* Move to the last char in the file */
	    if(fseek(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, -1, SEEK_END)) {
	        return errno;
	    }
		
		/* If last char in the file is not a new line add one. */
		if(fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file) != '\n') {
			/* Move back to the end of file */
			fseek(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, 0, SEEK_END);
			fprintf(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, "\n");	
		}
	}

	/* Move to ensure the cursor is at the end of file */
	fseek(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, 0, SEEK_END);
	
	
    /* Insert new row data. */
    for (size_t idx = 0; idx < s_csv_files[convert_handle_to_index(csv_file_handle)].number_of_columns; idx++) {
		if(data_array_to_insert) {
        	fprintf(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, "%s,", data_array_to_insert+(idx*memory_spacing));
		}
		else {
        	fprintf(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, ",");
		}
    }
    
    /* End with new line. */
    fprintf(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, "\n");
   
	/* Increment internal row counter. */
	set_row_count(csv_file_handle, get_row_count(csv_file_handle)+1);
	
    return (0);
}

/**
 * @brief Deletes the specified row from a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param row_to_delete The row to delete (0 based index).
 * @return 0 on success, errno on fail.
 */
int delete_row(int csv_file_handle, int row_to_delete) {

    long splice_offset = 0;
    FILE *p_temp_file = NULL;
    char read_char = 0;
    char temp_file_name[FILE_PATH_LENGTH] = {0};

    /* Move cursor in file to where the insertion should be. */    
    go_to_row(csv_file_handle, row_to_delete);
    
    /* Save the spot of the cursor. */
    splice_offset = ftell(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
    /* Create a temp file to copy old data and insert new data into. */
    sprintf(temp_file_name, "%stemp", s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path);
    p_temp_file = fopen(temp_file_name, "w+");

    /* Reset cursor position to start of file. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);

    /* Copy the old data into the new file until the splice mark is hit or EOF is reached. */
    while (((read_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != EOF) && (ftell(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file) != splice_offset)) {
        fprintf(p_temp_file, "%c", read_char);
    }

    /* Fast forward in the file until we reach the end of the row to delete */
    while (fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file) != '\n') {
        continue;
    }
	
	/* Back up to before the new line */
	fseek(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file, -1, SEEK_CUR);
		  
    /* Write out the rest of the data into the new file. */
    while ((read_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != EOF) {
        fprintf(p_temp_file, "%c", read_char);
    }
    
    /* Close the files and assign the pointers null */
    fclose(p_temp_file);
    p_temp_file = NULL;
    fclose(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
    s_csv_files[convert_handle_to_index(csv_file_handle)].p_file = NULL;

    /* Remove old file. */
	if(remove(s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path)) {
        return errno;
    }
    
    /* Rename temp file to the old file to "save" the changes. */
    if(rename(temp_file_name, s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path)) {
        return errno;   
    }

    /* Reopen the file so that the user can still interact with it. */
    if((s_csv_files[convert_handle_to_index(csv_file_handle)].p_file = fopen(s_csv_files[convert_handle_to_index(csv_file_handle)].absolute_path, "a+")) == NULL) {
        return errno;
    }
    
	/* Decrement the internal row counter. */
    set_row_count(csv_file_handle, get_row_count(csv_file_handle)-1);
	
	return 0;
}

/**
 * @brief Set the row count of the csv file in the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param row_count New row count value.
 */
inline void set_row_count(int csv_file_handle, int row_count) {
    s_csv_files[convert_handle_to_index(csv_file_handle)].number_of_rows = row_count;
}

/**
 * @brief Get the row count of the csv file from the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return Row count
 */
inline int get_row_count(int csv_file_handle) {    
    return s_csv_files[convert_handle_to_index(csv_file_handle)].number_of_rows;
}

/**
 * @brief Set the column count of the csv file in the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param column_count New column count value.
 */
inline void set_column_count(int csv_file_handle, int column_count) {
    s_csv_files[convert_handle_to_index(csv_file_handle)].number_of_columns = column_count;
}

/**
 * @brief Get the column count of the csv file from the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return Column count
 */
inline int get_column_count(int csv_file_handle) {    
    return s_csv_files[convert_handle_to_index(csv_file_handle)].number_of_columns;
}

/**
 * @brief Helper function to determine if the file name has the extension attached.
 * 
 * @param filename Absolute path name of a file.
 * @return 0 if file name ends with ".csv".
 */
inline static int check_for_extension(const char* filename) {
    return strncmp((filename+(strlen(filename)-EXTENSION_LENGTH)), CSV_EXTENSION_STRING, EXTENSION_LENGTH);
}

/**
 * @brief Helper function to convert a csv file handle to the index in the csv file array.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return The index in the csv file struct array.
 */
inline static int convert_handle_to_index(int csv_file_handle) {
    return (csv_file_handle-1);
}

/**
 * @brief Helper function to calculate the row count of a file when it is opened.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return The row count of the file when it was opened.
 */
static int calculate_row_count(int csv_file_handle) {
    
	size_t row_count = 0;
	char current_char = 0;
	
	/* Rewind to begining of file to make sure we count the rows correctly. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
	/* Loop until we have seen the correct amount of new lines. */
    while ((current_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != EOF) {
        if(current_char == '\n') {
			row_count++;
		}
    }
	
	/* Rewind to begining of file to leave no trace. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
    return row_count;
}

/**
 * @brief Helper function to calculate the column count of a file when it is opened.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return The column count of the file when it was opened.
 */
static int calculate_column_count(int csv_file_handle) {
	
    size_t column_count = 0;
	char current_char = 0;
	
	/* Rewind to begining of file to make sure we count the rows correctly. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
	/* Loop until we have seen the correct amount of new lines. */
    while (((current_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != '\n') && (current_char != EOF)) {
        if(current_char == ',') {
			column_count++;
		}
	}
   
	/* Rewind to begining of file to leave no trace. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
    return column_count;
}

/**
 * @brief Helper function to seek the file cursor to a specified row
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param row The row number to seek to (0 based index).
 */
static void go_to_row(int csv_file_handle, int row) {
	
	char current_char = 0;
	
	/* Rewind to begining of file to make sure we count the rows correctly. */
    rewind(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file);
	
	/* If row zero return after rewind */
	if (row == 0) {
		return;
	}
	
	/* Loop until we have seen the correct amount of new lines. */
    for(size_t file_row = 0; file_row < row; file_row++) {
        while (((current_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != '\n') && (current_char != EOF)) {
            continue;
        }
    }
}

/**
 * @brief Helper function to seek the file cursor to a specified column
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param column The column number to seek to (0 based index).
 */
static void go_to_column(int csv_file_handle, int column) {
	
	char current_char = 0;
	
	/* if column is zero return immediately */
	if (column == 0) {
		return;
	}
	
	/* This function assumes that we are at the row we want to go into. */
	/* Loop until we have seen the correct amount of commas. */
    for(size_t file_column = 0; file_column < column; file_column++) {
        while (((current_char = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != ',') && (current_char != EOF)) {
            continue;
        }
    }
}

/**
 * @brief Get the contents of a cell in a csv file 
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param content_string String to write the contents to.
 * @param cell Cell struct that specifies the location to clear.
 * @return 0 on success.
 */
int get_cell_contents(int csv_file_handle, char *content_string, cell_t cell) {
    
    char read_char[2] = {0};

    /* Move cursor in file to the location to copy. */    
    go_to_row(csv_file_handle, cell.row);
    go_to_column(csv_file_handle, cell.column);

    /* Fast forward in the file until we reach the end of the cell */
    while (((read_char[0] = (char)fgetc(s_csv_files[convert_handle_to_index(csv_file_handle)].p_file)) != ',') && read_char != EOF) {
        strcat(content_string, read_char);
    }

    return 0;
}