.PHONY: clean

TARGET=universe
TARGET_RELEASE=release

CXX=clang++
CXX_FLAGS=-std=c++11 -Wall $(shell sdl2-config --cflags)
LN_FLAGS=$(shell sdl2-config --libs) -lSDL2_ttf
RELEASE_FLAGS=-O3

BUILD_DIR=./build
SRC_DIR=./src
RLS_DIR=./rls

SOURCE = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCE))
RLS_OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(RLS_DIR)/%.o, $(SOURCE))

# Gcc/Clang will create these .d files containing dependencies.
DEP = $(OBJECTS:%.o=%.d)
DEP_RELEASE= $(RLS_OBJECTS:%.o=%.d)

default: $(TARGET)

$(TARGET): $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXX_FLAGS) $(LN_FLAGS) $^ -o $@

$(TARGET_RELEASE): $(RLS_DIR)/$(TARGET_RELEASE)

$(RLS_DIR)/$(TARGET_RELEASE): $(RLS_OBJECTS)
	$(CXX) $(CXX_FLAGS) $(RELEASE_FLAGS) $(LN_FLAGS) $^ -o $@

-include $(DEP)
-include $(DEP_RELEASE)

# The potential dependency on header files is covered
# by calling `-include $(DEP)`.
# The -MMD flags additionaly creates a .d file with
# the same name as the .o file.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@

$(RLS_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXX_FLAGS) $(RELEASE_FLAGS) -MMD -c $< -o $@

clean:
	-rm -rf $(BUILD_DIR)
	-rm -rf $(RLS_DIR)

run: $(TARGET)
	$(BUILD_DIR)/$(TARGET)

run_release: $(TARGET_RELEASE)
	$(RLS_DIR)/$(TARGET_RELEASE)
