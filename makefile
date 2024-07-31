# ---------
# PATHS
# ---------

bffr= Assets/Buffers
shdr= Assets/Shaders
txtre= Assets/Textures
cam= Assets/Camera

# ---------
# VARIABLES
# ---------

CXX=g++
CXXFLAGS= -g -Wall -std=c++17 -I./Assets/Libraries/include -I./imgui
LDFLAGS= -L./Assets/Libraries/lib -lrt -lm -ldl -lglfw3

BUFFERS= $(bffr)/VBO.o $(bffr)/EBO.o $(bffr)/VAO.o
SHADERS= $(shdr)/shader.o
TEXTURES= $(txtre)/texture.o
CAMERA= $(cam)/camera.o

DEPS= buffers.o shader.o texture.o camera.o
OBJ= $(BUFFERS) $(SHADERS) $(TEXTURES) $(CAMERA)

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
	rm -rf Assets/*/*.o

buffers.o:
	$(CXX) $(CXXFLAGS) -c $(bffr)/VBO.cpp -o $(bffr)/VBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(bffr)/EBO.cpp -o $(bffr)/EBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(bffr)/VAO.cpp -o $(bffr)/VAO.o $(LDFLAGS)

shader.o:
	$(CXX) $(CXXFLAGS) -c $(shdr)/shaderClass.cpp -o $(shdr)/shader.o $(LDFLAGS)

texture.o: shader.o
	$(CXX) $(CXXFLAGS) -c $(txtre)/texture.cpp -o $(txtre)/texture.o $(LDFLAGS)

camera.o: shader.o
	$(CXX) $(CXXFLAGS) -c $(cam)/camera.cpp -o $(cam)/camera.o $(LDFLAGS)
