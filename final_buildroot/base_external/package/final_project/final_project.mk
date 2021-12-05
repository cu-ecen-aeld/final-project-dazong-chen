##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

#TODO: Fill up the contents below in order to reference your printf git contents
FINAL_PROJECT_VERSION = f7b9696104e74fcc1701bb3a65b27c386bd0708f
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
FINAL_PROJECT_SITE = git@github.com:cu-ecen-aeld/final-project-dazong-chen.git
FINAL_PROJECT_SITE_METHOD = git
FINAL_PROJECT_GIT_SUBMODULES = YES

define FINAL_PROJECT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/final_application all
endef

define FINAL_PROJECT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/final_application/bin/client $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/final_application/bin/server $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
