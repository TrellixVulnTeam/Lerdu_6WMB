#
# FreeType 2 configuration rules templates for Unix + configure
#


# Copyright 1996-2000, 2002, 2004 by
# David Turner, Robert Wilhelm, and Werner Lemberg.
#
# This file is part of the FreeType project, and may only be used, modified,
# and distributed under the terms of the FreeType project license,
# LICENSE.TXT.  By continuing to use, modify, or distribute this file you
# indicate that you have read the license and understand and accept it
# fully.


ifndef TOP_DIR
  TOP_DIR := .
endif
TOP_DIR := $(shell cd $(TOP_DIR); pwd)

DELETE    := rm -f
DELDIR    := rmdir
SEP       := /

# this is used for `make distclean' and `make install'
ifndef OBJ_BUILD
  OBJ_BUILD := $(BUILD_DIR)
endif

# don't use `:=' here since the path stuff will be included after this file
#
FTSYS_SRC = $(BUILD_DIR)/ftsystem.c

INSTALL         := /usr/bin/install -c
INSTALL_DATA    := ${INSTALL} -m 644
INSTALL_PROGRAM := ${INSTALL}
INSTALL_SCRIPT  := ${INSTALL}
MKINSTALLDIRS   := $(BUILD_DIR)/mkinstalldirs

DISTCLEAN += $(OBJ_BUILD)/config.cache    \
             $(OBJ_BUILD)/config.log      \
             $(OBJ_BUILD)/config.status   \
             $(OBJ_BUILD)/unix-def.mk     \
             $(OBJ_BUILD)/unix-cc.mk      \
             $(OBJ_BUILD)/ftconfig.h      \
             $(OBJ_BUILD)/freetype-config \
             $(OBJ_BUILD)/freetype2.pc    \
             $(LIBTOOL)                   \
             $(OBJ_BUILD)/Makefile


# Standard installation variables.
#
prefix       := /usr/local
exec_prefix  := ${prefix}
libdir       := ${exec_prefix}/lib
bindir       := ${exec_prefix}/bin
includedir   := ${prefix}/include
datadir      := ${prefix}/share

version_info := 9:8:3


# The directory where all object files are placed.
#
# This lets you build the library in your own directory with something like
#
#   set TOP_DIR=.../path/to/freetype2/top/dir...
#   set OBJ_DIR=.../path/to/obj/dir
#   make -f $TOP_DIR/Makefile setup [options]
#   make -f $TOP_DIR/Makefile
#
ifndef OBJ_DIR
  OBJ_DIR := $(shell cd $(TOP_DIR)/objs; pwd)
endif


# The directory where all library files are placed.
#
# By default, this is the same as $(OBJ_DIR); however, this can be changed
# to suit particular needs.
#
LIB_DIR := $(OBJ_DIR)


# The SYSTEM_ZLIB macro is defined if the user wishes to link dynamically
# with its system wide zlib. If SYSTEM_ZLIB is 'yes', the zlib part of the
# ftgzip module is not compiled in.
SYSTEM_ZLIB := 


# The NO_OUTPUT macro is appended to command lines in order to ignore
# the output of some programs.
#
NO_OUTPUT := 2> /dev/null


# EOF
