#-----------------------------------
# Recursive file lookup
# Credit to SciresM
#-----------------------------------
export BUILD	:= build
export OUTDIR	:= dist
#-----------------------------------
# Common code
#-----------------------------------
export COMMON_INCLUDES ?= include
export COMMON_SOURCES  ?= source
#-----------------------------------
# Build
#-----------------------------------
all: ctr hac

ctr:
	@$(MAKE) -f platform/ctr/Makefile DEBUG=$(DEBUG) __APP_NAME__=3dsxupdate

hac:
	@$(MAKE) -f platform/hac/Makefile DEBUG=$(DEBUG) __APP_NAME__=nroupdate

#-----------------------------------
# Build & Distribute (Release)
#-----------------------------------
COMMIT_HASH := $(shell git rev-parse --short HEAD)

#-----------------------------------
# Debug/Development
#-----------------------------------
debug: ctr-debug hac-debug

ctr-debug: DEBUG=1
ctr-debug: ctr

hac-debug: DEBUG=1
hac-debug: hac

#-----------------------------------
# Clean
#-----------------------------------
clean:
	@rm -rf $(OUTDIR) $(BUILD)
