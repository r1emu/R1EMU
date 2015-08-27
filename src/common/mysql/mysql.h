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

#define MYSQL_HOSTNAME_DEFAULT     (char []) {"localhost"}
#define MYSQL_LOGIN_DEFAULT        (char []) {"r1emu"}
#define MYSQL_PASSWORD_DEFAULT     (char []) {"r1emu"}
#define MYSQL_DATABASE_DEFAULT     (char []) {"r1emu"}

typedef struct MySQLStartupInfo {
    // the MySQL hostname
    char *hostname;

    // the username of the MySQL user with read/write privilege
    char *login;

    // the password of the MySQL user
    char *password;

    // the database name containing all the data
    char *database;

} MySQLStartupInfo;

struct MySQL {
    // information about the database connection
    MySQLStartupInfo info;

    // handle to the MySQL connection
	MYSQL *handle;

	// last result
	MYSQL_RES *result;
};

typedef struct MySQL MySQL;

/**
 * @brief Allocate a new MySQL structure.
 * @param info An initialized MySQLStartupInfo.
 * @return A pointer to an allocated MySQL, or NULL if an error occured.
 */
MySQL *mySqlNew(MySQLStartupInfo *info);

/**
 * @brief Initialize an allocated MySQL structure.
 * @param self An allocated MySQL to initialize.
 * @param info An initialized MySQLStartupInfo.
 * @return true on success, false otherwise.
 */
bool mySqlInit(MySQL *self, MySQLStartupInfo *info);

/**
 * @brief Initialize an allocated MySQLStartupInfo structure.
 * @param self An allocated MySQLStartupInfo to initialize.
 * @param hostname The MySQL hostname
 * @param login The username of the MySQL user with read/write privilege
 * @param password The password of the MySQL user
 * @param database The database name containing all the data
 * @return true on success, false otherwise.
 */
bool mySqlStartupInfoInit(MySQLStartupInfo *self, char *hostname, char *login, char *password, char *database);

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
 * @brief Free the members of the MySQLStartupInfo structure
 * @param self A pointer to an allocated MySQLStartupInfo.
 */
void mySqlStartupInfoFree(MySQLStartupInfo *self);

/**
 * @brief Free an allocated MySQL structure and nullify the content of the pointer
 * @param self A pointer to an allocated MySQL
 */
void mySqlDestroy(MySQL **_self);
