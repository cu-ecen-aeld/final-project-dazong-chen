##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

#TODO: Fill up the contents below in order to reference your printf git contents
PRINTF_VERSION = 50833976a11f7e2b6cd5786c288c54258413d699
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
PRINTF_SITE = git@github.com:cu-ecen-aeld/final-project-dazong-chen.git
PRINTF_SITE_METHOD = git
PRINTF_GIT_SUBMODULES = YES

define PRINTF_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/final_application/i2c all
endef

define PRINTF_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/final_application/i2c/main $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
