
# [ MAIN VARIABLES ]  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
LIB_FULL_NAME  = libEMMA.a
COMPILER       = g++
ALWAYS_FLAGS   = -std=c++17
NORMAL_FLAGS   = -Wall -Wextra -Werror -O3 -flto -march=native
DEBUG_FLAGS    = -g -D EMMA_ENABLE_EXCEPTIONS=1
AR             = ar rcs

SRC_FOLDER     = src/
BUILD_FOLDER   = build/
INCLUDE_PATH   = include/

# Add files here to include them in the compilation
SRC_FILES = \
allocators/FreeList.cpp \
allocators/RedBlackTree.cpp

TEST_SRC_FILES=\
tester/main_tester_file.cpp

OBJ_FILES := $(SRC_FILES:%.cpp=%.o)

SRC_FILES := $(addprefix $(SRC_FOLDER), $(SRC_FILES))
OBJ_FILES = $(notdir $(SRC_FILES:.cpp=.o))
TEST_SRC_FILES := $(addprefix $(SRC_FOLDER), $(TEST_SRC_FILES))

# [ COMMANDS ]  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
all: ALWAYS_FLAGS += $(NORMAL_FLAGS)
all: $(LIB_FULL_NAME)

# Build the library
$(LIB_FULL_NAME): create_folders $(SRC_FILES)
	$(COMPILER) -c $(SRC_FILES) $(ALWAYS_FLAGS) -I $(INCLUDE_PATH)
	@mv $(OBJ_FILES) $(BUILD_FOLDER)
	$(AR) $@ $(addprefix $(BUILD_FOLDER), $(OBJ_FILES))
	@mv $(LIB_FULL_NAME) $(BUILD_FOLDER)

# Rebuild
re: fclean all

# Build the allocator using debug flags & macros
debug: ALWAYS_FLAGS += $(DEBUG_FLAGS)
debug: $(LIB_FULL_NAME)

# Clean up binaries
clean:
	rm -rf $(BUILD_FOLDER)*.o
	rm -f *.o

# Clean up everything. Binaries + executables/libraries
fclean: 
	rm -rf $(BUILD_FOLDER)
	rm -f tester_program

# Run pre-made tests for allocator
# Note, these tests aren't intended to be run in embedded systems! Linux only!
# Compiler flags etc. are also deliberately hardcoded because of this
LIB_SHORT_NAME = $(subst .a,,$(subst lib,,${LIB_FULL_NAME}))
test: all
	g++ -O3 -std=c++17 $(TEST_SRC_FILES) -L $(BUILD_FOLDER) \
	-l $(LIB_SHORT_NAME) -I $(INCLUDE_PATH) -o tester_program
	chmod +x tester_program
	clear
	./tester_program

create_folders:
	@mkdir -p $(BUILD_FOLDER)

.PHONY: all re debug clean fclean test create_folders
