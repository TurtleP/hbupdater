#-----------------------------------
# Recursive file lookup
# Credit to SciresM
#-----------------------------------
export OUTDIR	:= dist
#-----------------------------------
# Common code
#-----------------------------------
export COMMON_INCLUDES ?= include
export COMMON_SOURCES  ?= source
#-----------------------------------
export VERSION := 0.1.0
export DEFINES := -D__APP_VERSION__=\"${VERSION}\"
#-----------------------------------
# Build
#-----------------------------------
all: ctr hac

ctr:
	@$(MAKE) -f platform/ctr/Makefile DEBUG=$(DEBUG) BUILD=ctr-build __APP_NAME__=3dsxupdate

hac:
	@$(MAKE) -f platform/hac/Makefile DEBUG=$(DEBUG) BUILD=hac-build __APP_NAME__=nroupdate

#-----------------------------------
# Build & Distribute (Release)
#-----------------------------------
COMMIT_HASH := $(shell git rev-parse --short HEAD)

#-----------------------------------
# Debug/Development
#-----------------------------------
debug:   ctr-debug hac-debug
release: ctr-release hac-release

ctr-debug: DEBUG=1
ctr-debug: ctr

hac-debug: DEBUG=1
hac-debug: hac

ctr-release: DEBUG=
ctr-release: ctr

hac-release: DEBUG=
hac-release: hac

#-----------------------------------
# Clean
#-----------------------------------
clean:
	@rm -rf $(OUTDIR) ctr-build hac-build
