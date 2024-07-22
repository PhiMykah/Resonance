# ---------
# VARIABLES
# ---------

CXX=g++
CXXFLAGS= -g -Wall -std=c++17 -I./libraries/include -I./imgui
LDFLAGS= -L./libraries/lib -lrt -lm -ldl -lglfw3

# Reference files
OBJ = glad.c imgui/imgui.cpp \
	  imgui/imgui_demo.cpp imgui/imgui_draw.cpp \
	  imgui/imgui_impl_glfw.cpp \
	  imgui/imgui_impl_opengl3.cpp \
	  imgui/imgui_tables.cpp imgui/imgui_widgets.cpp


# ----------------
# MAKEFILE SCRIPTS
# ----------------

###########
# Compile #
###########

all: main

main:
	$(CXX) $(CXXFLAGS) main.cpp $(OBJ) -o main $(LDFLAGS)
