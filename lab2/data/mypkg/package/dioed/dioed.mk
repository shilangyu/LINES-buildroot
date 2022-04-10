################################################################################
#
# dioed
#
################################################################################

DIOED_VERSION = 0.1
DIOED_SITE = $(BR2_EXTERNAL_RPI_MINI_PATH)/src/dioed
DIOED_SITE_METHOD = local
DIOED_DEPENDENCIES = libgpiod

define DIOED_BUILD_CMDS
   $(MAKE) $(TARGET_CONFIGURE_OPTS) dioed -C $(@D)
endef
define DIOED_INSTALL_TARGET_CMDS 
   $(INSTALL) -D -m 0755 $(@D)/dioed $(TARGET_DIR)/usr/bin 
endef
DIOED_LICENSE = MIT

$(eval $(generic-package))
