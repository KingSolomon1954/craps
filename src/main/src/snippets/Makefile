# What you need to do
#
# Add --usr-data-dir assets/ to your dev command. Without it, your run
# seeds copies of the assets players into ~/.local/share and works on
# those. I updated the two lines in snippets/Makefile. Your command
# becomes _build/debug/bin/royalcraps --sys-config-dir assets/
# --usr-config-dir assets/ --usr-data-dir assets/ --debug --trace.
#
# Your dev runs will now also write
# ~/.local/state/royalcraps/state.yaml, which overrides
# assets/config.yaml on the next run. Delete it or pass --usr-state-dir
# somewhere else if that gets in the way.
#
# To exercise first startup, run with scratch directories:
# --usr-config-dir /tmp/rc/cfg --usr-data-dir /tmp/rc/data
# --usr-state-dir /tmp/rc/state.
#

WORKDIR := /work/craps/_build/debug

define STARTING_FOLDER
	if [ "$$(basename $$(pwd))" = "snippets" ]; then \
	    cd ../../../..; \
	elif [ "$$(basename $$(pwd))" = "craps" ]; then \
	    : ; \
	else \
	    echo "Execute from either snippets or craps folder"; \
	    echo "From snippets: make <target>"; \
	    echo "From craps: make -f src/main/src/Makefile <target>"; \
	    echo "From anywhere: make -C <path/to/snippets> <target>  "; \
	    exit; \
	fi; \

endef

#-----------------------------
#
# Running royalcraps
#
#-----------------------------

run-royalcraps:  # Execute on container
	podman exec -it -w $(WORKDIR) \
	    -e LANG=en_US_UTF-8 -e LC_ALL=en_US.UTF-8 \
	    gcc14-tools bin/royalcraps \
	    --sys-config-dir /work/craps/assets/ \
	    --usr-config-dir /work/craps/_build/local/share/royalcraps \
	    --usr-data-dir   /work/craps/_build/local/share/royalcraps \
	    --usr-state-dir  /work/craps/_build/local/state/royalcraps \
	--debug --trace

run-royalcraps-local:  # Execute on local host
	@$(STARTING_FOLDER) \
	_build/debug/bin/royalcraps \
	    --sys-config-dir  assets/ \
	    --usr-config-dir _build/local/share/royalcraps \
	    --usr-data-dir   _build/local/share/royalcraps \
	    --usr-state-dir  _build/local/state/royalcraps \
	--debug --trace

#-----------------------------
#
# Running snippets
#
#-----------------------------

run-ncurses-snippet:  # Execute on container
	podman exec -it -w $(WORKDIR) \
	    -e LANG=en_US_UTF-8 -e LC_ALL=en_US.UTF-8 \
	    gcc14-tools bin/ncurses-snippet

run-ncurses-snippet-local:  # Execute on local host
	@$(STARTING_FOLDER) \
	_build/debug/bin/ncurses-snippet

run-ncurses-animation:  # Execute on container
	podman exec -it -w $(WORKDIR) \
	    -e LANG=en_US_UTF-8 -e LC_ALL=en_US.UTF-8 \
	    gcc14-tools bin/ncurses-animation

run-ncurses-animation-local:  # Execute on local host
	@$(STARTING_FOLDER) \
	    _build/debug/bin/ncurses-animation-color

run-ncurses-animation-color:  # Execute on container
	podman exec -it -w $(WORKDIR) \
	    -e LANG=en_US_UTF-8 -e LC_ALL=en_US.UTF-8 \
	    gcc14-tools bin/ncurses-animation-color

run-ncurses-animation-color-local: # Execute on local host
	@$(STARTING_FOLDER) \
	_build/debug/bin/ncurses-animation-color

run-ncurses-dynamic-menu:  # Execute on container
	podman exec -it -w $(WORKDIR) \
	    -e LANG=en_US_UTF-8 -e LC_ALL=en_US.UTF-8 \
	    gcc14-tools bin/ncurses-dynamic-menu

run-ncurses-dynamic-menu-local:  # Execute on local host
	@$(STARTING_FOLDER) \
	_build/debug/bin/ncurses-dynamic-menu

run-ncurses-too-small-resize:  # Execute on container
	podman exec -it -w $(WORKDIR) \
	    -e LANG=en_US_UTF-8 -e LC_ALL=en_US.UTF-8 \
	    gcc14-tools bin/ncurses-too-small-resize

run-ncurses-too-small-resize-local:  # Execute on local host
	@$(STARTING_FOLDER) \
	_build/debug/bin/ncurses-too-small-resize
