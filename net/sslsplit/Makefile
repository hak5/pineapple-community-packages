#
# Copyright (C) 2016-2018 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=sslsplit
PKG_VERSION:=0.5.5-tls13
PKG_RELEASE:=1
PKG_LICENSE:=BSD-2-Clause
PKG_LICENSE_FILES:=LICENSE LICENSE.contrib LICENSE.third

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_INSTALL:=1

# Thanks to Adde88 (github.com/adde88)
PKG_MAINTAINER:=Andreas Nilsen <adde88@gmail.com>

include $(INCLUDE_DIR)/package.mk

define Package/sslsplit
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libevent2 +libopenssl +libevent2-openssl +libevent2-pthreads +libnet-1.2.x +libpcap +USE_MUSL:musl-fts
  TITLE:=Transparent SSL/TLS interception
  URL:=https://www.roe.ch/SSLsplit
endef

define Package/sslsplit/description
  Transparent SSL/TLS interception
endef

MAKE_FLAGS += prefix=/usr \
	OSNAME=Linux

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -rf ./src/* $(PKG_BUILD_DIR)/
endef

define Package/sslsplit/install
	$(INSTALL_DIR)	$(1)/bin
	$(INSTALL_DIR)	$(1)/etc/sslsplit
	$(INSTALL_BIN)	$(PKG_INSTALL_DIR)/usr/local/bin/sslsplit			$(1)/bin/
	$(INSTALL_CONF) $(PKG_INSTALL_DIR)/usr/local/etc/sslsplit/sslsplit.conf.sample	$(1)/etc/sslsplit/
endef

$(eval $(call BuildPackage,sslsplit))
