/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file mysql.h
 * @brief MySQL contains all database related functions
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#pragma once

#include "R1EMU.h"

typedef enum MySQLStatus {
    SQL_SUCCESS = 0,
    SQL_ERROR = -1,
} MySQLStatus;

#define MAX_QUERY_SIZE 1024

typedef struct MySQLInfo {
    // the MySQL hostname
    char *hostname;

    // the username of the MySQL user with read/write privilege
    char *user;

    // the password of the MySQL user
    char *password;

    // the database name containing all the data
    char *database;

} MySQLInfo;

struct MySQL {
    // information about the database connection
    MySQLInfo info;

    // handle to the MySQL connection
	MYSQL *handle;

	// last result
	MYSQL_RES *result;
};

typedef struct MySQL MySQL;

/**
 * @brief Allocate a new MySQL structure.
 * @param info An initialized MySQLInfo.
 * @return A pointer to an allocated MySQL, or NULL if an error occured.
 */
MySQL *mySqlNew(MySQLInfo *info);

/**
 * @brief Initialize an allocated MySQL structure.
 * @param self An allocated MySQL to initialize.
 * @param info An initialized MySQLInfo.
 * @return true on success, false otherwise.
 */
bool mySqlInit(MySQL *self, MySQLInfo *info);

/**
 * @brief Initialize an allocated MySQLInfo structure.
 * @param self An allocated MySQLInfo to initialize.
 * @param hostname The MySQL hostname
 * @param login The username of the MySQL user with read/write privilege
 * @param password The password of the MySQL user
 * @param database The database name containing all the data
 * @return true on success, false otherwise.
 */
bool mySqlInfoInit(MySQLInfo *self, char *hostname, char *login, char *password, char *database);

/**
 * @brief : Connect to the MySQL database
 * @param self An allocated MySQL instance
 * @return true on success, false otherwise
 */
bool mySqlConnect(MySQL *self);

/**
 * @brief Send a query to the MySQL server
 * @param self An allocated MySQL
 * @param query The SQL formatted query
 * @return a MySQLStatus
 */
MySQLStatus mySqlQuery(MySQL *self, const char *query, ...);

/**
 * @brief Free the result of the last query
 * @param self An allocated MySQL
 */
void mySqlFreeResult(MySQL *self);

/**
 * @brief Free the members of the MySQLInfo structure
 * @param self A pointer to an allocated MySQLInfo.
 */
void mySqlInfoFree(MySQLInfo *self);

/**
 * @brief Free an allocated MySQL structure and nullify the content of the pointer
 * @param self A pointer to an allocated MySQL
 */
void mySqlDestroy(MySQL **_self);
