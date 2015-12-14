#!/bin/sh

echo "Install database accounts ... Enter root password"
mysql -u root -p < install_database.sql
echo "Install main database tables ... Enter r1emu password"
mysql -u r1emu -p < r1emu_main.sql
echo "Install database dummy accounts entries ... Enter r1emu password"
mysql -u r1emu -p < fill_database.sql
echo "Install database items entries ... Enter r1emu password"
mysql -u r1emu -p < items_equip.sql
echo "Install database items details entries ... Enter r1emu password"
mysql -u r1emu -p < items_all.sql