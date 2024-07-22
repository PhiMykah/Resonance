# ---------
# VARIABLES
# ---------

CXX=g++
CXXFLAGS= -g -Wall -std=c++17 -I./libraries/include
LDFLAGS= -L./libraries/lib -lglfw3 -lrt -lm -ldl

# ----------------
# MAKEFILE SCRIPTS
# ----------------

###########
# Compile #
###########

all: main

main:
	$(CXX) $(CXXFLAGS) main.cpp -o main $(LDFLAGS)
