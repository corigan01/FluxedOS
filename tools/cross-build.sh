#!/bin/bash

BINUTILS_VERSION=${BINUTILS_VERSION:?}
GCC_VERSION=${GCC_VERSION:?}
GDB_VERSION=${GDB_VERSION:?}
BUILD_DIR=${BUILD_DIR:?}
TARGET=${TARGET:?}
PREFIX=${PREFIX:?}

MXE_DIR="/opt"
args=$@

function main {
    
    if [[ $args == *"build"* ]]; then
        installPackages
        installMXE
        downloadSources
        compileAll
    elif [[ $args == *"remove"* ]]; then
        rm -rf $BUILD_DIR
    else
        echo "ERROR: Usage $0 <build|remove>"
        exit 1
    fi
}

function installPackages {
    
    echoColor "Installing packages"

    sudo -E apt-get install git \
        autoconf automake autopoint bash bison bzip2 lzip flex gettext\
        git g++ gperf intltool libffi-dev libgdk-pixbuf2.0-dev \
        libtool libltdl-dev libssl-dev libxml-parser-perl make \
        openssl p7zip-full patch perl pkg-config python ruby scons \
        sed unzip wget xz-utils libtool-bin texinfo g++-multilib -y
}

# MXE

function installMXE {

    echoColor "Installing MXE"

    if [ ! -d "${MXE_DIR}/mxe/usr/bin" ]
    then
        echoColor "    Cloning mxe and compiling mingw32.static GCC"
        cd ${MXE_DIR}
        sudo -E git clone https://github.com/mxe/mxe.git
        cd mxe
        sudo make gcc

        echo "export PATH=${MXE_DIR}/mxe/usr/bin:$PATH" >> ~/.bashrc
        export PATH=${MXE_DIR}/mxe/usr/bin:$PATH
    else
       echoColor "    mxe is already installed. You'd better make sure ${MXE_DIR}/mxe/usr/bin is on your path and that you've previously made mxe's gcc!"
    fi
}

# Downloads

function downloadSources {
    sudo mkdir -p $BUILD_DIR
    sudo chown -R $USER $BUILD_DIR
    cd $BUILD_DIR
    
    echoColor "Downloading all sources"
    
    downloadAndExtract "binutils" $BINUTILS_VERSION

    downloadAndExtract "gcc" $GCC_VERSION "http://ftp.gnu.org/gnu/gcc/gcc-$GCC_VERSION/gcc-$GCC_VERSION.tar.gz"
    
    echoColor "        Downloading GCC prerequisites"
    
    # Automatically download GMP, MPC and MPFR. These will be placed into the right directories.
    # You can also download these separately, and specify their locations as arguments to ./configure
    cd ./gcc-$GCC_VERSION
    ./contrib/download_prerequisites
    cd ..
    
    downloadAndExtract "gdb" $GDB_VERSION    
}

function downloadAndExtract {
    name=$1
    version=$2
    override=$3
    
    pwd
    
    echoColor "    Processing $name"
    
    if [ ! -f $name-$version.tar.gz ]
    then
        echoColor "        Downloading $name-$version.tar.gz"
        
        if [ -z $3 ]
        then
            wget http://ftp.gnu.org/gnu/$name/$name-$version.tar.gz
        else
            wget $override
        fi
    else
        echoColor "        $name-$version.tar.gz already exists"
    fi

    if [ ! -d $name-$version ]
    then
        echoColor "        Extracting $name-$version.tar.gz"
        tar -xf $name-$version.tar.gz
    else
        echoColor "        Folder $name-$version already exists"
    fi
}

function compileAll {

    echoColor "Compiling..."

    export PATH="${PREFIX}/bin:${PATH}"
    mkdir -p ${PREFIX}

    compileBinutils
    compileGCC
    compileGDB
}

function compileBinutils {    
    if [[ $args == *"binutils"* ]]; then
        echoColor "    Compiling binutils"
    
        mkdir -p build-binutils-$BINUTILS_VERSION
        cd build-binutils-$BINUTILS_VERSION
        
        configureArgs="--target=${TARGET} --with-sysroot --disable-nls --disable-werror --prefix=$PREFIX"
        
        # Configure
        echoColor "        Configuring binutils (binutils_configure.log)"
        ../binutils-$BINUTILS_VERSION/configure $configureArgs >> binutils_configure.log
        
        # Make
        echoColor "        Making (binutils_make.log)"
        make >> binutils_make.log
        
        # Install
        echoColor "        Installing (binutils_install.log)"
        sudo make install >> binutils_install.log
        cd ..
    else
        echoColor "    Skipping binutils as 'binutils' was ommitted from commandline args '$args'"
    fi
}

function compileGCC {
    if [[ $args == *"gcc"* ]]; then
    
        echoColor "    Compiling gcc"

        mkdir -p build-gcc-$GCC_VERSION
        cd build-gcc-$GCC_VERSION
        
        configureArgs="--target=${TARGET} --disable-nls --enable-languages=c,c++ --without-headers --prefix=$PREFIX"
        
        # Configure
        echoColor "        Configuring gcc (gcc_configure.log)"
        ../gcc-$GCC_VERSION/configure $configureArgs >> gcc_configure.log
        
        # Make GCC
        echoColor "        Making gcc (gcc_make.log)"
        make all-gcc >> gcc_make.log
        
        # Install GCC
        echoColor "        Installing gcc (gdb_install.log)"
        sudo make install-gcc >> gcc_install.log
        
        # Make libgcc
        echoColor "        Making libgcc (libgcc_make.log)"
        make all-target-libgcc >> libgcc_make.log
        
        # Install libgcc
        echoColor "        Installing libgcc (libgcc_install.log)"
        sudo make install-target-libgcc >> libgcc_install.log
        
        cd ..
    else
        echoColor "    Skipping gcc as 'gcc' was ommitted from commandline args '$args'"
fi
}

function compileGDB {
    if [[ $args == *"gdb"* ]]; then

        echoColor "    Compiling gdb"
    
        configureArgs="--target=${TARGET} --disable-nls --disable-werror --prefix=$PREFIX"
        
        mkdir -p build-gdb-$GDB_VERSION
        cd build-gdb-$GDB_VERSION
        
        # Configure        
        echoColor "        Configuring (gdb_configure.log)"
        ../gdb-$GDB_VERSION/configure $configureArgs >> gdb_configure.log
        
        # Make
        echoColor "        Making (gdb_make.log)"
        make >> gdb_make.log
        
        # Install
        echoColor "        Installing (gdb_install.log)"
        sudo make install >> gdb_install.log
        cd ..
    else
        echoColor "    Skipping gdb as 'gdb' was ommitted from commandline args '$args'"
    fi
}

function echoColor {
    echo -e "\e[96m$1\e[39m"
}

main