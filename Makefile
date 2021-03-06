#
# Copyright (c) 2012-2014, Ari Suutari <ari@stonepile.fi>.
# All rights reserved. 
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. The name of the author may not be used to endorse or promote
#     products derived from this software without specific prior written
#     permission. 
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
# INDIRECT,  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

#
# Compile uIP & drivers using Pico]OS library Makefile
#

RELROOT = ../picoos/
PORT ?= unix
BUILD ?= RELEASE

include $(RELROOT)make/common.mak

TARGET = picoos-lwip

#
# lwip source files
#
LWIPDIR=lwip/src
include $(LWIPDIR)/Filelists.mk

ARCHFILES =	sys_arch.c

SRC_TXT =	sockets.c \
		apps/dhcps/dhcps.c \
		$(COREFILES) \
		$(CORE4FILES) \
		$(CORE6FILES) \
		$(SNMPFILES) \
		$(APIFILES) \
		$(NETIFFILES) \
		$(ARCHFILES) \
		$(SNTPFILES)

EXTRA_CFLAGS = -Wno-address
SRC_HDR =	
SRC_OBJ =

ifeq '$(PORT)' 'unix'
SRC_TXT +=  netif/tapif.c
endif
		
ifeq '$(PORT)' 'lpc2xxx'
SRC_TXT +=  netif/cs8900a_lpc_e2129.c
endif

MODULES +=	$(RELROOT)/../picoos-micro 
DIR_USRINC +=	include \
		$(LWIPDIR)/include \
		$(LWIPDIR)/include/ipv4 \
		$(LWIPDIR)/include/ipv6 \
		$(LWIP_DRIVER_INC)

ifeq '$(strip $(DIR_OUTPUT))' ''
DIR_OUTPUT = $(CURRENTDIR)/bin
endif


include $(MAKE_LIB)
