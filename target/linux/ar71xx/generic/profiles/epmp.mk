#
# Copyright (C) 2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/EPMP
	NAME:=Cambium ePMP
	PACKAGES:=
endef

define Profile/EPMP/Description
	Package set optimized for the Cambium ePMP.
endef
$(eval $(call Profile,EPMP))
