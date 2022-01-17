EXEC_NAME := $(notdir $(CURDIR))

DEBUG_FLAGS := -g -Og
RELEASE_FLAGS := -O3 -flto

SOURCES		:= source
INCLUDES	:= include

BUILD_RELEASE := build_release
BUILD_DEBUG := build

OUT_RELEASE := dist_release
OUT_DEBUG := dist

CFLAGS := $(CFLAGS)
CXXFLAGS := $(CXXFLAGS) -std=c++17
LDFLAGS := $(LDFLAGS)

# Past here there shouldn't be much that needs editing unless extra file types are necessary
#------------------------------------------------------------------------------------------------
CFILES := $(patsubst ./%,%,$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.c))) $(filter %.c,$(EXTRA_SOURCE_FILES))
NOFORMAT_CFILES := $(patsubst ./%,%,$(foreach dir,$(NOFORMAT_SOURCES),$(wildcard $(dir)/*.c))) $(filter %.c,$(NOFORMAT_EXTRA_SOURCE_FILES))
CPPFILES := $(patsubst ./%,%,$(foreach dir,$(SOURCES),$(wildcard $(dir)/*.cpp))) $(filter %.cpp,$(EXTRA_SOURCE_FILES))
NOFORMAT_CPPFILES := $(patsubst ./%,%,$(foreach dir,$(NOFORMAT_SOURCES),$(wildcard $(dir)/*.cpp))) $(filter %.cpp,$(NOFORMAT_EXTRA_SOURCE_FILES))

ifeq ($(strip $(RELEASE)),)
OPTIMIZE := $(DEBUG_FLAGS)
BUILD := $(BUILD_DEBUG)
OUT := $(OUT_DEBUG)
else
OPTIMIZE := $(RELEASE_FLAGS)
BUILD := $(BUILD_RELEASE)
OUT := $(OUT_RELEASE)
endif

CFLAGS := $(CFLAGS) $(foreach dir, $(INCLUDES), -I$(CURDIR)/$(dir)) $(OPTIMIZE)
CXXFLAGS := $(CFLAGS) $(CXXFLAGS) $(OPTIMIZE)
LDFLAGS := $(LDFLAGS) $(OPTIMIZE)

OFILES := $(CFILES:.c=.c.o) $(CPPFILES:.cpp=.cpp.o) $(NOFORMAT_CFILES:.c=.c.o) $(NOFORMAT_CPPFILES:.cpp=.cpp.o)
BUILD_OFILES := $(subst //,/,$(subst /../,/__PrEvDiR/,$(subst /,//, $(OFILES))))
BUILD_OFILES := $(patsubst ../%,__PrEvDiR/%,$(BUILD_OFILES))
BUILD_OFILES := $(addprefix $(BUILD)/, $(BUILD_OFILES))
DEPSFILES := $(BUILD_OFILES:.o=.d)

LD := $(if $(strip $(CPPFILES) $(NOFORMAT_CPPFILES)),$(CXX),$(CC))

ifeq ($(OS),Windows_NT)
WHICH = where
else
WHICH = which
endif

.PHONY: all clean format release debug spotless

all: format $(OUT)/$(EXEC_NAME)

release:
	$(MAKE) RELEASE=1 all

debug: all

clean:
	@rm -rf $(BUILD_DEBUG)
	@rm -rf $(OUT_DEBUG)

spotless: clean
	@rm -rf $(BUILD_RELEASE)
	@rm -rf $(OUT_RELEASE)

format:
ifneq ($(strip $(shell $(WHICH) clang-format)),)
	clang-format -i $(CFILES) $(CPPFILES) $(foreach dir, $(INCLUDES), $(call rwildcard,$(dir),*.hpp *.h))
endif

# Add extra file type rules below. Note how to place files in the build and out directories
#------------------------------------------------------------------------------------------------

$(OUT)/$(EXEC_NAME): $(BUILD_OFILES)
	@mkdir -p $(dir $@)
	$(LD) $(BUILD_OFILES) $(LDFLAGS) -o $@

$(BUILD)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(CC) -MMD -MP -MF $(@:.o=.d) $(CFLAGS) -c -o $@ $<

$(BUILD)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -MMD -MP -MF $(@:.o=.d) $(CXXFLAGS) -c -o $@ $<

include $(wildcard $(DEPSFILES))
