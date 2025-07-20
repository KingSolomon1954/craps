# -----------------------------------------------------------------
#
# Base level submake for using Conan.
#
# Used by
# * conan-consumer.mak
# * conan-producer.mak
#
# -----------------------------------------------------------------

ifndef _INCLUDE_CONAN_MAK
_INCLUDE_CONAN_MAK := 1

ifndef D_CONAN
    $(error cpp.mak must define 'D_CONAN')
endif
ifndef D_BLD_DEBUG
    $(error cpp.mak must define 'D_BLD_DEBUG')
endif
ifndef D_BLD_PROD
    $(error cpp.mak must define 'D_BLD_PROD')
endif
ifndef D_MAK
    $(error cpp.mak must define 'D_MAK')
endif
ifndef DEFAULT_BUILD_TYPE
    $(error cpp.mak must define 'DEFAULT_BUILD_TYPE')
endif
ifndef CONAN_INSTALL_DONE_PROD
    $(error cpp.mak must define 'CONAN_INSTALL_DONE_PROD')
endif
ifndef CONAN_INSTALL_DONE_DEBUG
    $(error cpp.mak must define 'CONAN_INSTALL_DONE_DEBUG')
endif

include $(D_MAK)/conan-registry.mak

# ------------ Conan Folders/Files Section ------------
#
_CONAN_CFG_DIR        := $(D_CONAN)
_CONAN_PY_FILE        := $(_CONAN_CFG_DIR)/conanfile.py
_CONAN_LOCKFILE_PROD  := $(_CONAN_CFG_DIR)/prod.lock
_CONAN_LOCKFILE_DEBUG := $(_CONAN_CFG_DIR)/debug.lock
_CONAN_INST_PROD      := $(D_BLD_PROD)/generators
_CONAN_INST_DEBUG     := $(D_BLD_DEBUG)/generators

# ------------ Conan Misc Section ------------

conan-clear-cache:
	@$(CPP_BLD_CNTR_EXEC) conan remove "*" --confirm

.PHONY: conan-clear-cache

# ------------ Conan Clean Section ------------

clean::

conan-clean:
	$(info Not supported. Issue make conan-clean-both, conan-clean-prod or conan-clean-debug)

conan-clean-both: conan-clean-prod conan-clean-debug

conan-clean-prod:
	rm -rf $(_CONAN_INST_PROD)

conan-clean-debug:
	rm -rf $(_CONAN_INST_DEBUG)

.PHONY: conan-clean-prod conan-clean-debug conan-clean-both

# TODO Move to conan-consumer.mak

# ------------ Conan Install Section ------------

ifeq "$(DEFAULT_BUILD_TYPE)" "Release"
    conan: conan-prod
else ifeq "$(DEFAULT_BUILD_TYPE)" "Debug"
    conan: conan-debug
else
    $(error bad value for DEFAULT_BUILD_TYPE: "$(DEFAULT_BUILD_TYPE)")
endif

conan-both: conan-prod conan-debug

conan-prod: $(CONAN_INSTALL_DONE_PROD)

conan-debug: $(CONAN_INSTALL_DONE_DEBUG)

$(CONAN_INSTALL_DONE_PROD): _ARG_BLD_TYPE := Release
$(CONAN_INSTALL_DONE_PROD): _ARG_LOCKFILE := $(_CONAN_LOCKFILE_PROD)

$(CONAN_INSTALL_DONE_DEBUG): _ARG_BLD_TYPE := Debug
$(CONAN_INSTALL_DONE_DEBUG): _ARG_LOCKFILE := $(_CONAN_LOCKFILE_DEBUG)

$(CONAN_INSTALL_DONE_DEBUG) $(CONAN_INSTALL_DONE_PROD): \
		$(CONAN_REGISTRY_SETUP_DONE) $(_CONAN_PY_FILE)
	$(CPP_BLD_CNTR_EXEC) conan install \
	    --settings=build_type=$(_ARG_BLD_TYPE) \
	    --lockfile=$(_ARG_LOCKFILE) \
            --build=missing \
	    $(_CONAN_PY_FILE)
	@touch $@

.PHONY: conan conan-both conan-prod conan-debug

# ------------ Conan Lockfile Section ------------

ifeq "$(DEFAULT_BUILD_TYPE)" "Release"
    conan-lock: conan-lock-prod
else ifeq "$(DEFAULT_BUILD_TYPE)" "Debug"
    conan-lock: conan-lock-debug
else
    $(error bad value for DEFAULT_BUILD_TYPE: "$(DEFAULT_BUILD_TYPE)")
endif

conan-lock-both: conan-lock-prod conan-lock-debug

conan-lock-prod: _ARG_LOCKFILE := $(_CONAN_LOCKFILE_PROD)
conan-lock-prod: _ARG_BLD_TYPE := Release
conan-lock-prod: _conan-lock1

conan-lock-debug: _ARG_LOCKFILE := $(_CONAN_LOCKFILE_DEBUG)
conan-lock-debug: _ARG_BLD_TYPE := Debug
conan-lock-debug: _conan-lock2

_conan-lock1 _conan-lock2:
	@echo "(conan) Creating Conan lockfile: $(_ARG_LOCKFILE)"
	$(CPP_BLD_CNTR_EXEC) conan lock create $(_CONAN_PY_FILE) \
	    --settings=build_type=Release \
	    --lockfile-out=$(_ARG_LOCKFILE)

.PHONY: conan-lock conan-lock-both \
        conan-lock-prod conan-lock-debug \
       _conan-lock1 _conan-lock2

# ------------ Help Section ------------

HELP_TXT += "\n\
conan,                   Install Conan libs (for default build)\n\
conan-prod,              Install Conan libs for production build\n\
conan-debug,             Install Conan libs for debug build\n\
conan-both,              Install Conan libs for both debug and prod builds\n\
conan-clean-prod,        Deletes $(_CONAN_INST_PROD) tree\n\
conan-clean-debug,       Deletes $(_CONAN_INST_DEBUG) tree\n\
conan-clean-both,        Deletes both Conan debug and prod tree\n\
conan-clear-cache,       Removes all packages from Conan cache\n\
conan-lock,              Create/update lockfile (default) in $(_CONAN_CFG_DIR)\n\
conan-lock-prod,         Create/update prod lockfile in $(_CONAN_CFG_DIR)\n\
conan-lock-debug,        Create/update debug lockfile in $(_CONAN_CFG_DIR)\n\
conan-lock-both,         Create/update prod and debug lockfile in $(_CONAN_CFG_DIR)\n\
"

endif
