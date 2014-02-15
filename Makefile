# Project structure
TARGET = bin/main
SRCDIR = src
BUILDDIR = build
INCDIR = include
LIBDIR = lib

# Source and object files
SRCEXT = cpp
SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Main compiler
CC = g++

# compiler flags:
# -g	tells the compiler to produce symbolic information that a debugger
#	(like gdb) needs.
# -Wall	tells the compiler to print warnings about pretty much everything.
# -w    Hide all of the warnings because I don't like seeing them

# CFLAGS contains flags for the compiler
CFLAGS = -g -Wall

# Specify include directories
IPATH = -I$(INCDIR) -I/usr/local/include/swirl

# Specify library paths
LPATH = -L$(LIBDIR) -L/usr/local/lib -L$(WNHOME)/lib
LIB = -lm -lwn -lswirlmain -lswirlcha -lswirlab

# Build all target(s)
all: $(TARGET)

# Linking & compiling target(s)
$(TARGET): $(OBJECTS)
	@echo " Linking ..."
	@echo " $(CC) $^ -o $(TARGET) $(LPATH) $(LIB)"; $(CC) $^ -o $(TARGET) $(LPATH) $(LIB)

# Compile source into objects
$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(IPATH) -c -o $@ $<"; $(CC) $(CFLAGS) $(IPATH) -c -o $@ $<

# Clean build
clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
tester:
	$(CC) $(CFLAGS) test/tester.cpp $(IPATH) $(LPATH) $(LIB) -o bin/tester

.PHONY: clean
