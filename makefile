# flags needed to build the target (compiler, target name, and compiler flags)
CC = g++
CFLAGS := `wxWidgets/build/linux/wx-config --cppflags` `wxWidgets/build/linux/wx-config --libs` -Wl,-rpath,wxWidgets/build/linux/lib/
target = wxWidgetsTemplate

# location of source files
source_dir = source
build_dir = linux-build

# library build information
lib_build_path = wxWidgets/build/linux
lib_file_detect = Makefile

# derives names of object files (only looks at .cpp files)
sources := $(shell cd $(source_dir) && find . -type f -name '*.cpp' -printf "$(build_dir)/%f ")
objects := $(subst .cpp,.o,$(sources))

debug:
	@make --no-print-directory mode=-g all
	
release:
	@make --no-print-directory mode=-O3 all

# Compiles the app, and the library if needed. Uses all the available processor cores.
all: $(lib_build_path)/$(lib_file_detect)
	@make --no-print-directory $(build_dir)/$(target) -j$(shell nproc)

# link the object files together into the executable
$(build_dir)/$(target): $(objects)
	$(CC) $(CFLAGS) $(mode) -o $@ $^

# compile object files from source files
# This only looks at changes in the .cpp files, so if headers are modified, a rebuild is needed.
$(build_dir)/%.o: $(source_dir)/%.cpp
	mkdir -p $(build_dir)
	$(CC) $(CFLAGS) $(mode) $(CPPFLAGS) -c -o $@ $<
	
# build the library
$(lib_build_path)/$(lib_file_detect): $(lib_build_path)/$(lib_file_detect)
	./setup-linux.sh

# remove the build folder
clean:
	rm -rf $(build_dir)
	
clean-library:
	rm -rf wxWidgets/build/linux/
	
clean-all: clean clean-library
	
# launch the app
run: debug
	@./$(build_dir)/$(target)
