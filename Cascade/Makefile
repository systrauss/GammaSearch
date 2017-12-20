#to compile using root libraries
ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

#this is the compiler that will be used:
CXX           = g++
#these are some compiler flags
CXXFLAGS      = -O2 -fPIC 

#folder including all the header files
INCLUDES := -Iinclude -Iuser
#folder with all the source files
SOURCES := src
#source files
FILES := Search.cxx fileRead.cxx main.cxx
#this is the executable name
EXECUTABLES := cascadeSearch
#libraries
LIBS          = $(ROOTLIBS) $(SYSLIBS)
#this are object files (take the names of sources and change the extensions)
OBJECTS := $(FILES:.cxx=.o)

#don't do anything to files named 'clean' and 'rmobjects'
.PHONY: clean rmobjects

$(EXECUTABLES): $(OBJECTS)
	$(CXX) $(LIBS) $(INCLUDES) $(CXXFLAGS) $(OBJECTS) -o $(EXECUTABLES)

#compile all source files and generate object files
# %.cxx = any file that matches the pattern
# $< = name of the first dependency file
# $@ = name of the target file
%.o: $(SOURCES)/%.cxx
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@

#this will remove the executable file and objects
clean:
	rm -rf $(EXECUTABLES) $(OBJECTS)

rmobjects:
	rm -rf $(OBJECTS)
