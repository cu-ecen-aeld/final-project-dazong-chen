##############################################################
#
# AESD-ASSIGNMENTS
#
##############################################################

#TODO: Fill up the contents below in order to reference your printf git contents
FINAL_PROJECT_VERSION = 40ba860bc039aacba16ff12bded2f02267aa0fb1
# Note: Be sure to reference the *ssh* repository URL here (not https) to work properly
# with ssh keys and the automated build/test system.
# Your site should start with git@github.com:
FINAL_PROJECT_SITE = git@github.com:cu-ecen-aeld/final-project-dazong-chen.git
FINAL_PROJECT_SITE_METHOD = git
FINAL_PROJECT_GIT_SUBMODULES = YES

define FINAL_PROJECT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/final_application/client_server all
endef

define FINAL_PROJECT_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/final_application/client_server/client $(TARGET_DIR)/usr/bin
	$(INSTALL) -m 0755 $(@D)/final_application/client_server/server $(TARGET_DIR)/usr/bin
endef

$(eval $(generic-package))
