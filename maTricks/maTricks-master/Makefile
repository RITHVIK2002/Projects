
PROJDIR := $(realpath $(CURDIR)/)
SOURCEDIR := $(PROJDIR)/src
BUILDDIR := $(PROJDIR)/build
INCLUDE_PATH := $(SOURCEDIR)/include

# Name of final Executable
TARGET = maTricks


# Create the list of directories
DIRS = parser lexer codegen .
SOURCEDIRS = $(foreach dir, $(DIRS), $(addprefix $(SOURCEDIR)/, $(dir)))
TARGETDIRS = $(foreach dir, $(DIRS), $(addprefix $(BUILDDIR)/, $(dir)))


INCLUDES = $(addprefix -I,$(INCLUDE_PATH))


VPATH = $(SOURCEDIRS)


# Create a list of *.c sources in DIRS
SOURCES = $(foreach dir,$(SOURCEDIRS),$(wildcard $(dir)/*.c))

# Define objects for all sources
OBJS = $(subst $(SOURCEDIR),$(BUILDDIR),$(SOURCES:.c=.o))


# Define dependencies files for all objects
DEPS = $(OBJS:.o=.d)


# Name the compiler
CC = gcc


RM = rm -rf 
RMDIR = rm -rf 
MKDIR = mkdir -p
ERRIGNORE = 2>/dev/null
SEP=/

PSEP = $(strip $(SEP))


# Define the function that will generate each rule
define generateRules
$(1)/%.o: %.c
	@$(CC) -c $$(INCLUDES) -o $$(subst /,$$(PSEP),$$@) $$(subst /,$$(PSEP),$$<) -MMD
endef


## Flex and Yacc Compiling
FLEX_DIR := $(SOURCEDIR)/lexer
YACC_DIR := $(SOURCEDIR)/parser

# Indicate to make which targets are not files
.PHONY: all clean run 

all:  $(TARGET)
	@g++ $(SOURCEDIR)/codegen/gen.cpp -o $(BUILDDIR)/codegen/gen.out -I/$(INCLUDE_PATH)

$(YACC_DIR)/parser.tab.c: $(YACC_DIR)/parser.y
	@bison  -d -t  -o $@ $<
	@mv $(YACC_DIR)/parser.tab.h  $(INC_DIRS)/$(INCLUDE_PATH)/parser.tab.h

$(FLEX_DIR)/lex.yy.c: $(FLEX_DIR)/lexer.l
	@lex  -o $@ $<
	@

# Executable 
$(TARGET): $(OBJS)
	@$(CC) $(OBJS) -o $(TARGET)

# #Include mk file contianing dependices
DUMMY.mk := $(shell $(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)) #Just to create dirs before start of make
-include  $(DUMMY.mk)  $(BUILDDIR)/include.mk

$(BUILDDIR)/include.mk:  $(YACC_DIR)/parser.tab.c $(FLEX_DIR)/lex.yy.c
	@$(shell echo "-include $(DEPS)" > $@)

 

# Generate rules 
$(foreach targetdir, $(TARGETDIRS), $(eval $(call generateRules, $(targetdir))))

# # Create nesscary build dirs
config:
	@$(MKDIR) $(subst /,$(PSEP),$(TARGETDIRS)) $(ERRIGNORE)

# Remove all objects, dependencies and executable files generated during the build
clean:
	@$(RM) $(YACC_DIR)/parser.tab.c $(FLEX_DIR)/lex.yy.c $(INCLUDE_PATH)/parser.tab.h
	@$(RM) $(TARGET) $(ERRIGNORE)
	@$(RMDIR) ${BUILDDIR} $(ERRIGNORE)
	@$(RM) *.ll *.out run.cpp


################# For Testing ########################

run: all
	@./maTricks  ./examples/Exampletest2.mt --show_ir
	@./a.out
	
