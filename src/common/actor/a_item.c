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

#include "a_item.h"

bool a_itemInit(Item *self) {
    return true;
}

uint32_t itemGetAmount(Item *self) {
    return self->amount;
}

uint32_t itemGetItemId(Item *self) {
    return self->itemId;
}

uint32_t itemGetCategory(Item *self) {
    return self->itemCategory;
}

void itemGenKey(uint64_t itemIdKey, ItemKey itemKey) {
    snprintf(itemKey, ITEM_KEY_SIZE, "%I64x", itemIdKey);
}
