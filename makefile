# ---------
# PATHS
# ---------

a = Assets
bffr= $(a)/Buffers
shdr= $(a)/Shaders
txtre= $(a)/Textures
cam= $(a)/Camera

# ---------
# VARIABLES
# ---------

CXX=g++
CXXFLAGS= -g -Wall -std=c++17 -I./$(a)/Libraries/include -I./imgui
LDFLAGS= -L./$(a)/Libraries/lib -lrt -lm -ldl -lglfw3

BUFFERS= $(bffr)/VBO.o $(bffr)/EBO.o $(bffr)/VAO.o
SHADERS= $(shdr)/shader.o
TEXTURES= $(txtre)/texture.o
CAMERA= $(cam)/camera.o
SHAPES= $(a)/shapes.h

DEPS= buffers.o shader.o texture.o camera.o

OBJ= $(BUFFERS) $(SHADERS) $(TEXTURES) $(CAMERA) $(SHAPES)

# Reference files
glad = glad.c stb.cpp

IMGUI = imgui/imgui.cpp \
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

all: main clean

main: $(DEPS)
	$(CXX) $(CXXFLAGS) main.cpp $(glad) $(OBJ) $(IMGUI) -o main $(LDFLAGS)

clean:
	rm -rf $(a)/*/*.o

buffers.o:
	$(CXX) $(CXXFLAGS) -c $(bffr)/VBO.cpp -o $(bffr)/VBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(bffr)/EBO.cpp -o $(bffr)/EBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(bffr)/VAO.cpp -o $(bffr)/VAO.o $(LDFLAGS)

shader.o:
	$(CXX) $(CXXFLAGS) -c $(shdr)/shader.cpp -o $(shdr)/shader.o $(LDFLAGS)

texture.o: shader.o
	$(CXX) $(CXXFLAGS) -c $(txtre)/texture.cpp -o $(txtre)/texture.o $(LDFLAGS)

camera.o: shader.o
	$(CXX) $(CXXFLAGS) -c $(cam)/camera.cpp -o $(cam)/camera.o $(LDFLAGS)
