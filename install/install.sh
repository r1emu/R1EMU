#!/bin/sh

# Configurations
installPath="$PWD"

# === Installation starts from here ===

# Get packages
sudo apt-get update
sudo apt-get install g++
sudo apt-get install make
sudo apt-get install automake
sudo apt-get install pkg-config
sudo apt-get install git
sudo apt-get install cmake
sudo apt-get install p7zip-full
sudo apt-get install libmysqlclient-dev
sudo apt-get install mysql-server

# =========================================================
# =============           Redis             ===============
# =========================================================

# Get & Compile Redis environment
wget http://download.redis.io/releases/redis-2.8.19.tar.gz
tar -xvf redis-2.8.19.tar.gz
cd redis-2.8.19/
make
sudo make install
cd $installPath
rm redis-2.8.19.tar.gz
rm -rf redis-2.8.19

# Get & Compile hiredis (C Client) environment
wget https://github.com/redis/hiredis/archive/v0.12.1.tar.gz
tar -xvf v0.12.1.tar.gz
cd hiredis-0.12.1
make
sudo make install
cd $installPath
rm -rf hiredis-0.12.1
rm v0.12.1.tar.gz

# =========================================================
# =============            ZMQ              ===============
# =========================================================

# Get & Compile ZMQ environment
wget http://download.zeromq.org/zeromq-4.1.0-rc1.tar.gz
tar -xvf zeromq-4.1.0-rc1.tar.gz
cd zeromq-4.1.0/
./configure
make
sudo make install
cd $installPath
rm -rf zeromq-4.1.0/
rm zeromq-4.1.0-rc1.tar.gz

# Get & Compile CZMQ environment
wget http://download.zeromq.org/czmq-3.0.0-rc1.tar.gz
tar -xvf czmq-3.0.0-rc1.tar.gz
cd czmq-3.0.0/
./configure
make
sudo make install
cd $installPath
rm -rf czmq-3.0.0/
rm czmq-3.0.0-rc1.tar.gz

# =========================================================
# =============           MySQL             ===============
# =========================================================

# Get & Compile mysql connector (C Client) environment
wget http://cdn.mysql.com/Downloads/Connector-C/mysql-connector-c-6.1.6-src.tar.gz
tar -xvzf mysql-connector-c-6.1.6-src.tar.gz
cd mysql-connector-c-6.1.6-src/
cmake -G "Unix Makefiles"
make
sudo make install
cd $installPath
rm -rf mysql-connector-c-6.1.6-src/
rm mysql-connector-c-6.1.6-src.tar.gz

# =========================================================
# =============         CBP2MAKE          =================
# =========================================================
wget http://heanet.dl.sourceforge.net/project/cbp2make/cbp2make-stl-rev147-all.tar.7z
7za x cbp2make-stl-rev147-all.tar.7z
cd cbp2make-stl-rev147-all
make -f cbp2make.cbp.mak.unix
mkdir ~/.cbp2make
cp bin/Release/cbp2make ~/.cbp2make/
cd $installPath
rm -rf cbp2make-stl-rev147-all
rm cbp2make-stl-rev147-all.tar.7z

# =========================================================
# =============           JSON              ===============
# =========================================================
wget http://www.digip.org/jansson/releases/jansson-2.7.tar.gz
tar -xvf jansson-2.7.tar.gz
cd jansson-2.7
cmake -G "Unix Makefiles"
make
sudo make install
cd $installPath
rm -rf jansson-2.7
rm jansson-2.7.tar.gz

# =========================================================
# =============           R1EMU             ===============
# =========================================================

# Update ld
sudo ldconfig

# Compile R1EMU
cd $installPath
git pull
cd $installPath/../projects/codeblocks/
~/.cbp2make/cbp2make -in R1EMU.workspace
mv R1EMU.workspace.mak Makefile
make clean
make

# Install database
cd $installPath/../sql-files/
echo "Installing mysql database (using root user)..."
mysql -u root -p < install_database.sql

echo "Launching redis server ..."
redis-server &

echo "Launching GlobalServer ..."
cd $installPath/../bin/
./GlobalServer &
