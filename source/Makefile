CC = gcc
CXX = g++

# location of the Python header files
PYTHON_VERSION = 2.7
PYTHON_INCLUDE = /usr/include/python$(PYTHON_VERSION)

# location of the Boost Python include files and library
BOOST_INC = /usr/include
BOOST_LIB = /usr/lib


# Let’s leave a place holder for additional include directories

INCLUDES = -I$(PYTHON_INCLUDE) -I$(BOOST_INC)

# Compilation options:
# -g for debugging info and -Wall enables all warnings
CFLAGS = -g -Wall --std=c++11
CXXFLAGS = -fPIC $(CFLAGS) $(INCLUDES)


# Linking options:
# -g for debugging info

LDFLAGS = -g

# List the libraries you need to link with in LDLIBS
# For example, use "-lm" for the math library

LDLIBS =

OBJS = main.o readcsv.o process.o graph.o node.o pattern.o util.o

OUTLIB = libpatmin

LIBOBJS = py_wrapper.o process.o graph.o node.o pattern.o util.o

TARGET = patmin

default: $(TARGET)
	@echo  Done.


$(TARGET): $(OBJS)
	$(CXX) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS) $(LDLIBS)


lib: $(LIBOBJS)
	g++ -shared -Wl,--export-dynamic $(LIBOBJS) -L$(BOOST_LIB) -l:libboost_python-py$(subst .,,$(PYTHON_VERSION)).so -L/usr/lib/python$(PYTHON_VERSION)/config -lpython$(PYTHON_VERSION) -o $(TARGET).so $(CFLAGS)


main.o: main.cpp readcsv.h process.h

py_wrapper.o: py_wrapper.cpp process.h

readcsv.o: readcsv.cpp readcsv.h util.h

process.o: process.cpp process.h graph.h util.h

graph.o: graph.cpp graph.h pattern.h node.h

node.o: node.cpp node.h pattern.h

pattern.o: pattern.cpp pattern.h

util.o: util.h



.PHONY: clean
clean:
	rm -f *.o a.out *.so core $(TARGET)


.PHONY: all
all: clean $(TARGET) lib
