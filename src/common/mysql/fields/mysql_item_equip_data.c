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

#include "mysql_item_equip_data.h"
#include "common/static_data/fields/item_equip_data.h"

/**
 * @brief Request Item Data from MySQL
 */
static bool mySqlRequestItemsCommonData(MySQL *self, size_t *rowsCount);



static bool mySqlRequestItemsCommonData(MySQL *self, size_t *rowsCount) {

    bool status = false;

    if (mySqlQuery(self, "SELECT * FROM ies_item_equip")) {
        error("SQL Error : %s" , mysql_error(self->handle));
        goto cleanup;
    }

    *rowsCount = mysql_num_rows(self->result);

    status = true;

cleanup:
    return status;
}

bool mySqlBuildItemEquipDataDb(MySQL *self, StaticData **_db) {

    bool status = false;
    StaticData *db = NULL;
    MYSQL_ROW row;

    if (!(db = staticDataNew("itemEquipDataDb"))) {
        error("Cannot allocate a new static data for item data.");
        goto cleanup;
    }

    size_t rowsCount;
    if (!(mySqlRequestItemsCommonData(self, &rowsCount))) {
        error("Cannot request items data from MySQL.");
        goto cleanup;
    }

    while ((row = mysql_fetch_row(self->result)) != NULL) {
        ItemEquipData *itemEquipData = itemEquipDataNew(row);
        if (!(staticDataAdd(db, itemEquipData->ClassID, itemEquipData))) {
            error("Cannot add the item data '%d' to the db.", itemEquipData->ClassID);
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
