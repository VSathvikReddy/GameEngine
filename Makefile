TARGET_EXEC := game

BUILD_DIR := build
SRC_DIR := src
INC_DIR := src 
EXTERNAL_DIR := external/glm external/glew/include external/glfw/include external/stb

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INCS := $(shell find $(INC_DIR) -type d) $(EXTERNAL_DIR)
INC_FLAGS := $(addprefix -I,$(INCS))

# (Preprocessor Flags)
CPPFLAGS := $(INC_FLAGS) -MMD -MP


# (External Libraries and Linker Flags)
LDFLAGS := \
	external/glew/lib/libGLEW.a \
	external/glfw/lib/libglfw3.a \
	-lGL -lX11 -lXrandr -lXi -lXcursor -lXinerama \
	-ldl -pthread



Warning_FLAGS := -Wall -Wextra # -Wpedantic -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wnull-dereference -Wold-style-cast -Wsuggest-override -Wpessimizing-move -Wmisleading-indentation -Wduplicated-cond -Wunused -Wcast-align
# -Wconversion, -Wsign-conversion, -Wdouble-promotion

# (C++ Compiler Flags)
CXXFLAGS := -std=c++17 $(Warning_FLAGS)
# (C Compiler Flags)
# CFLAGS   := 


CXX := g++
# CC  := gcc

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

# $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
# 	mkdir -p $(dir $@)
# 	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(BUILD_DIR)/$(TARGET_EXEC)
	./$(BUILD_DIR)/$(TARGET_EXEC)


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
