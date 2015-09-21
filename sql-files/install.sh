#!/bin/sh

mysql -u root -p < install_database.sql 
mysql -u r1emu -p < r1emu_main.sql 
mysql -u r1emu -p < fill_database.sql 