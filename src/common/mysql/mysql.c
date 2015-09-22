/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
 *          See LICENSE file for further information
 */

#include "mysql.h"

MySQL *mySqlNew(MySQLInfo *info) {
    MySQL *self;

    if ((self = calloc(1, sizeof(MySQL))) == NULL) {
        return NULL;
    }

    if (!mySqlInit(self, info)) {
        mySqlDestroy(&self);
        error("MySQL failed to initialize.");
        return NULL;
    }

    return self;
}

bool mySqlInit(MySQL *self, MySQLInfo *info) {
    self->result = NULL;
    self->handle = mysql_init(NULL);

    mySqlInfoInit(&self->info, info->hostname, info->user, info->password, info->database);

    return true;
}

bool mySqlInfoInit(
    MySQLInfo *self,
    char *hostname,
    char *user,
    char *password,
    char *database)
{
    self->hostname = strdup(hostname);
    self->user     = strdup(user);
    self->password = strdup(password);
    self->database = strdup(database);

    return true;
}

bool mySqlConnect(MySQL *self) {
    MySQLInfo *sqlInfo = &self->info;

    info("Connecting to the MySQL Server(%s@%s -> %s)...", sqlInfo->user, sqlInfo->hostname, sqlInfo->database);

    if (!mysql_real_connect(
        self->handle,
        sqlInfo->hostname,
        sqlInfo->user,
        sqlInfo->password,
        sqlInfo->database,
        0, NULL, 0))
    {
        error("Could not connect to the '%s' database(%s@%s.(mysql_errno = %d)",
            sqlInfo->database, sqlInfo->user, sqlInfo->hostname, mysql_errno(self->handle));
        return false;
    }

    info("Connected to the MySQL Server !");
    return true;
}

MySQLStatus mySqlQuery(MySQL *self, const char* query, ...) {
	va_list	args;
	char buf[MAX_QUERY_SIZE];

	if (self->handle == NULL) {
        error("MySQL hasn't been initialized yet.");
		return SQL_ERROR;
	}

	mySqlFreeResult(self);

	va_start(args, query);
	vsnprintf(buf, MAX_QUERY_SIZE - 1, query, args);
	va_end(args);

	dbg("Launching SQL command : %s", buf);

	if (mysql_real_query(self->handle, buf,(unsigned long) strlen(buf))) {
		return SQL_ERROR;
	}

	self->result = mysql_store_result(self->handle);

	return SQL_SUCCESS;
}

void mySqlFreeResult(MySQL *self) {
	if (self && self->result) {
		mysql_free_result(self->result);
		self->result = NULL;
	}
}

void mySqlInfoFree(MySQLInfo *self) {
    free(self->hostname);
    free(self->database);
    free(self->user);
    free(self->password);
}

void mySqlDestroy(MySQL **_self) {
    MySQL *self = *_self;

    mySqlInfoFree(&self->info);
    mySqlFreeResult(self);
    mysql_close(self->handle);

    free(self);
    *_self = NULL;
}
