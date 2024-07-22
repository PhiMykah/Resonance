# ---------
# VARIABLES
# ---------

CXX=g++
CXXFLAGS= -g -Wall -std=c++17 -I./libraries/include
LDFLAGS= -L./libraries/lib -lrt -lm -ldl -lglfw3

# ----------------
# MAKEFILE SCRIPTS
# ----------------

###########
# Compile #
###########

all: main

main:
	$(CXX) $(CXXFLAGS) main.cpp glad.c -o main $(LDFLAGS)
