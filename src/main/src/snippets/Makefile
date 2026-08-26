
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
	    --usr-config-dir /work/craps/assets/

run-royalcraps-local:  # Execute on local host
	@$(STARTING_FOLDER) \
	_build/debug/bin/royalcraps \
	    --sys-config-dir assets/ \
	    --usr-config-dir assets/

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
