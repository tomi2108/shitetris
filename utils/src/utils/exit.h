#ifndef UTILS_EXIT_H_
#define UTILS_EXIT_H_

#include <commons/log.h>
#include <stdlib.h>

#define EXIT_SUCCESS 0
#define EXIT_SERVER_CONNECTION_ERROR 1
#define EXIT_CLIENT_CONNECTION_ERROR 2
#define EXIT_MALFORMATTED_CONFIG 3
#define EXIT_NOT_ENOUGH_ARGUMENTS 4
#define EXIT_MALFORMATTED_ARGUMENTS 5
#define EXIT_ENOENT 6
#define EXIT_CONFIG_FIELD_ERROR 7
#define EXIT_INPUT_ERROR 8

/**
 * @fn    exit_success
 * @param logger Logger to log the success message
 * @brief Exits the thread with succes return code
 */
void exit_success(t_log *logger);

/**
 * @fn    exit_server_connection_error
 * @param logger Logger to log the error message
 * @brief Exits the thread with server connection error code
 */
void exit_server_connection_error(t_log *logger);

/**
 * @fn    exit_client_connection_error
 * @param logger Logger to log the error message
 * @brief Exits the thread with client connection error code
 */
void exit_client_connection_error(t_log *logger);

/**
 * @fn    exit_malformatted_config_error
 * @param logger Logger to log the error message
 * @brief Exits the thread with malformatted config error code
 */
void exit_malformatted_config_error(t_log *logger);

/**
 * @fn    exit_not_enough_arguments_error
 * @param logger Logger to log the error message
 * @brief Exits the thread with not enough arguments error code
 */
void exit_not_enough_arguments_error(t_log *logger);

/**
 * @fn    exit_malformatted_arguments_error
 * @param logger Logger to log the error message
 * @brief Exits the thread with not enough arguments error code
 */
void exit_malformatted_arguments_error(t_log *logger);

/**
 * @fn    exit_enoent_error
 * @param logger Logger to log the error message
 * @brief Exits the thread with enoent error code
 */
void exit_enoent_error(t_log *logger, char *path);

/**
 * @fn    exit_enoent_error
 * @param logger Logger to log the error message
 * @param field  Field of the config where error was found
 * @brief Exits the thread with config field error code
 */
void exit_config_field_error(t_log *logger, char *field);

/**
 * @fn    exit_input_error
 * @param logger Logger to log the error message
 * @brief Exits the thread with input error code
 */
void exit_input_error(t_log *logger);
#endif
