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

#include "mysql_item_data.h"
#include "common/static_data/fields/item_data.h"

/**
 * @brief Request Item Data from MySQL
 */
static bool mySqlRequestItemsData(MySQL *self, size_t *rowsCount);



static bool mySqlRequestItemsData(MySQL *self, size_t *rowsCount) {

    bool status = false;

    if (mySqlQuery(self, "SELECT * in ies_item_equip")) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    *rowsCount = mysql_num_rows(self->result);

    status = true;

cleanup:
    return status;
}

bool mySqlBuildItemDataDb(MySQL *self, StaticData **_db) {

    bool status = false;
    StaticData *db = NULL;
    MYSQL_ROW row;

    if (!(db = staticDataNew("itemDataDb"))) {
        error("Cannot allocate a new static data for item data.");
        goto cleanup;
    }

    size_t rowsCount;
    if (!(mySqlRequestItemsData(self, &rowsCount))) {
        error("Cannot request items data from MySQL.");
        goto cleanup;
    }

    while ((row = mysql_fetch_row(self->result)) != NULL) {
        ItemData *itemData = itemDataNew(row);
        if (!(staticDataAdd(db, itemData->ClassID, itemData))) {
            error("Cannot add the item data '%d' to the db.", itemData->ClassID);
            goto cleanup;
        }
    }

    status = true;
    *_db = db;

cleanup:
    if (!status) {
        staticDataDestroy(&db);
    }
    mySqlFreeResult(self);
    return status;
}
