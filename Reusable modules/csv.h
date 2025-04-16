/**
 * @file csv.h
 * @author Zachary Hoagland (zachary.hoagland@microchip.com)
 * @brief CSV Storage Module Application Interface.
 * @version 0.1
 * @date 2025-04-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#ifndef CSV_H
#define CSV_H


#ifdef __cplusplus
    extern "C" {
#endif


/* -------------------- Public Includes -------------------- */
#include <stdint.h>

/* -------------------- Public Macros/Defines -------------------- */

/* -------------------- Public Enums -------------------- */


/* -------------------- Public Structs -------------------- */

/**
 * @brief Struct to encapsulate the row and column of a cell. 0 based indexing for both members.
 * 
 */
typedef struct _cell {
    size_t row;         /**< The row; 0 based indexed. */
    size_t column;      /**< The column; 0 based indexed. */
}cell_t;

/* -------------------- Public (global) Vars -------------------- */


/* -------------------- Public Function Declarations -------------------- */

/* Open / close functions */

/**
 * @brief Create a csv file
 * 
 * @param absolute_path_to_file Absolute path to the csv file to be created
 * @param number_of_columns Number of columns for the csv file
 * @return 0 on success, errno on fail.
 */
int create_csv_file(const char *absolute_path_to_file, int number_of_columns);

/**
 * @brief Opens an already existing csv file
 * 
 * @param absolute_path_to_file Absolute path of the file to be opened.
 * @return 0 on success, errno on fail.
 */
int open_csv_file(const char *absolute_path_to_file);

/**
 * @brief Closes a csv file from reading and writing.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return 0 on success, errno on fail.
 */
int close_csv_file(int csv_file_handle);

/* Row and cell manipulation functions */

/**
 * @brief Clears the contents of a cell in a csv file. 
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param cell Cell struct that specifies the location to clear.
 * @return 0 on success, errno on fail.
 */
int clear_cell(int csv_file_handle, cell_t cell);

/**
 * @brief Updates the contents of a cell in a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param data_to_insert Pointer to the string of data to insert.
 * @param cell Cell struct that specifies the location to update.
 * @return 0 on success, errno on fail.
 */
int update_cell(int csv_file_handle, const char *data_to_insert, cell_t cell);

/**
 * @brief Updates the contents of a row in a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param row The row in which to update (0 based index).
 * @param memory_spacing The offset in memory from the base address to the next string address
 * @param data_array_to_insert Pointer to a 2D array of strings containing the data to insert. The length should match the csv column width.
 * @return 0 on success, errno on fail.
 */
int update_row(int csv_file_handle, int row_to_update, int memory_spacing, const char *data_array_to_insert);

/**
 * @brief Inserts a row of data into a csv file
 * 
 * @param data_array_to_insert Pointer to a 2D array of strings containing the data to insert. The length should match the csv column width.
 * @param row_to_insert_before The row to insert before. -1 indicates insert at the end (append row)
 * @param memory_spacing The offset in memory from the base address to the next string address
 * @return 0 on success, errno on fail.
 */
int insert_row(int csv_file_handle, int row_to_insert_before, int memory_spacing, const char *data_array_to_insert);

/**
 * @brief Appends a row of data to a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param memory_spacing The offset in memory from the base address to the next string address
 * @param data_array_to_insert Pointer to a 2D array of strings containing the data to insert. The length should match the csv column width. If NULL appends a blank row.
 * @return 0 on success, errno on fail.
 */
int append_row(int csv_file_handle, int memory_spacing, const char *data_array_to_insert);

/**
 * @brief Deletes the specified row from a csv file.
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param row_to_delete The row to delete (0 based index).
 * @return 0 on success, errno on fail.
 */
int delete_row(int csv_file_handle, int row_to_delete);

/* File data functions */

/**
 * @brief Set the column count of the csv file in the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param column_count New column count value.
 */
void set_column_count(int csv_file_handle, int column_count);

/**
 * @brief Get the column count of the csv file from the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return Column count
 */
int get_column_count(int csv_file_handle);

/**
 * @brief Set the row count of the csv file in the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param row_count New row count value.
 */
void set_row_count(int csv_file_handle, int row_count);

/**
 * @brief Get the row count of the csv file from the struct
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @return Row count
 */
int get_row_count(int csv_file_handle);

/**
 * @brief Get the contents of a cell in a csv file 
 * 
 * @param csv_file_handle Handle of the csv file to operate on.
 * @param content_string String to write the contents to.
 * @param cell Cell struct that specifies the location to clear.
 * @return 0 on success.
 */
int get_cell_contents(int csv_file_handle, char *content_string, cell_t cell);


#ifdef __cplusplus
    }
#endif


#endif /* CSV_H */