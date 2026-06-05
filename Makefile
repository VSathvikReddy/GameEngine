TARGET_EXEC := game

BUILD_DIR := build
SRC_DIR := src
INC_DIR := include

CPP_SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
C_SRCS   := $(shell find $(SRC_DIR) -name '*.c')
CPP_OBJS := $(CPP_SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
C_OBJS   := $(C_SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
#SRCS := $(CPP_SRCS) $(C_SRCS)
OBJS := $(CPP_OBJS) $(C_OBJS)
DEPS := $(OBJS:.o=.d)

INCS := $(shell find $(INC_DIR) -type d) external/glm external/glew/include external/glfw/include external/stb
INC_FLAGS := $(addprefix -I,$(INCS))
# (C Preprocessor Flags)
CPPFLAGS := $(INC_FLAGS) -MMD -MP


# (External Libraries and Linker Flags)
LDFLAGS := \
	external/glew/lib/libGLEW.a \
	external/glfw/lib/libglfw3.a \
	-lGL -lX11 -lXrandr -lXi -lXcursor -lXinerama \
	-ldl -pthread

# (C++ Compiler Flags)
CXXFLAGS := -std=c++20
Warning_FLAGS := -Wall -Wextra -Wpedantic -Wshadow -Wnon-virtual-dtor -Woverloaded-virtual -Wnull-dereference -Wold-style-cast -Wsuggest-override -Wpessimizing-move -Wmisleading-indentation -Wduplicated-cond -Woverloaded-virtual -Wunused -Wcast-align
# -Wconversion, -Wsign-conversion, -Wdouble-promotion
# (C Compiler Flags)
CFLAGS   := 

CXX := g++
CC  := gcc

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	mkdir -p $(BUILD_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: $(BUILD_DIR)/$(TARGET_EXEC)
	./$(BUILD_DIR)/$(TARGET_EXEC)


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

setup:
	@mkdir -p external

	@# ---- GLEW ----
	@if [ ! -d external/glew ]; then \
		echo "Cloning GLEW..."; \
		git clone https://github.com/nigels-com/glew.git external/glew; \
	else \
		echo "GLEW already cloned"; \
	fi
	cd external/glew && \
	make extensions && \
	make

	@# ---- GLFW ----
	@if [ ! -d external/glfw ]; then \
		echo "Cloning GLFW..."; \
		git clone https://github.com/glfw/glfw.git external/glfw; \
	else \
		echo "GLFW already cloned"; \
	fi
	cd external/glfw && \
	cmake -S . -B build \
		-DBUILD_SHARED_LIBS=OFF \
		-DGLFW_BUILD_WAYLAND=OFF \
		-DGLFW_BUILD_EXAMPLES=OFF \
		-DGLFW_BUILD_TESTS=OFF \
		-DGLFW_BUILD_DOCS=OFF && \
	cmake --build build && \
	mkdir -p lib && \
	cp build/src/libglfw3.a lib/

	@# ---- GLM ----
	@if [ ! -d external/glm ]; then \
		echo "Cloning GLM..."; \
		git clone https://github.com/g-truc/glm.git external/glm; \
	else \
		echo "GLM already cloned"; \
	fi


-include $(DEPS)
