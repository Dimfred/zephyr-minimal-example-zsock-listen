.ONESHELL:

all: help

# BOARD = nucleo_u5a5zj_q
BOARD = nucleo_u575zi_q

################################################################################
# BUILD AND RUN
build: build-debug ## alias for build-debug

build-debug: ## build the application in debugging mode
	west build \
		-b $(BOARD) \
		app/ \
		-- \
		-DSHIELD="esp_8266"
		# TODO (see app/prj.conf)
		# -DEXTRA_DTC_OVERLAY_FILE="$${PWD}/assets/flash_partition.overlay"

flash: ## flash the application
	west flash

serial: ## open a serial console to read print output
	minicom -D /dev/ttyACM* -b 115200

debugserver: ## start a gdb debugserver
	west debugserver

clean: ## clean the build folder
	rm -r build/

################################################################################
# CONFIG
menuconfig: ## create a kernel configuration for the application (terminal)
	west build -t menuconfig

guiconfig: ## create a kernel configuration for the application (gui)
	west build -t guiconfig

################################################################################
# INIT WORKSPACE
init: ## initialize the workspace
	cp assets/nucleo_u5a5zj_q.overlay zephyr/boards/shields/esp_8266/boards/
	cp assets/nucleo_u575zi_q.overlay zephyr/boards/shields/esp_8266/boards/
	echo "Cloning zephyr, this will take some time..."
	git submodule update --init --recursive
	echo "Update west toolchain..."
	west update

################################################################################
# HELP
help: ## print this help
	@grep '##' $(MAKEFILE_LIST) \
		| grep -Ev 'grep|###' \
		| sed -e 's/^\([^:]*\):[^#]*##\([^#]*\)$$/\1:\2/' \
		| awk -F ":" '{ printf "%-25s%s\n", "\033[1;32m" $$1 ":\033[0m", $$2 }' \
		| grep -v 'sed'
