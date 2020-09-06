include $(TOPDIR)/rules.mk

PKG_NAME:=mk7led

PKG_VERSION:=20xx-XX-XX
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)
PKG_SOURCE_VERSION:=0
PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION)-$(PKG_SOURCE_VERSION).tar.gz
PKG_SOURCE_SUBDIR:=$(PKG_NAME)-$(PKG_VERSION)

PKG_USE_MIPS16:=0

include $(INCLUDE_DIR)/package.mk

define Package/mk7led
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=MK7LED add-on LED control
  URL:=https://www.kismetwireless.net/
  DEPENDS:=
endef

define Package/mk7led/description
  Control program for setting the LEDs on the MK7LED add-on board
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp src/mk7led.c $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

define Build/Configure
endef

define Build/Compile
	$(TARGET_CC) $(PKG_BUILD_DIR)/mk7led.c -o $(PKG_BUILD_DIR)/LEDMK7
endef

define Package/mk7led/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/LEDMK7 $(1)/usr/bin/
endef

$(eval $(call BuildPackage,mk7led))
