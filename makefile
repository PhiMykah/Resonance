# ---------
# PATHS
# ---------

a = Assets
h = $(a)/Headers
src = $(a)/Source
shdr= $(a)/Shaders
tex= $(a)/Textures

# ---------
# VARIABLES
# ---------

CXX=g++
HEADERS= -I./$(h)
CXXFLAGS= -g -Wall -std=c++17 -I./$(a)/Libraries/include $(HEADERS) -I./imgui
LDFLAGS= -L./$(a)/Libraries/lib -lrt -lm -ldl -lglfw3

SHAPES= $(h)/Shapes.h

BUFFERS= $(a)/VBO.o $(a)/EBO.o $(a)/VAO.o
SHADERS= $(a)/Shader.o
TEXTURES= $(a)/Texture.o
CAMERA= $(a)/Camera.o
MESH= $(a)/Mesh.o

DEPS= Buffers.o Shader.o Texture.o Camera.o Mesh.o

OBJ= $(BUFFERS) $(SHADERS) $(TEXTURES) $(CAMERA) $(MESH) $(SHAPES)

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
	rm -rf $(a)/*.o

Buffers.o:
	$(CXX) $(CXXFLAGS) -c $(src)/VBO.cpp -o $(a)/VBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(src)/EBO.cpp -o $(a)/EBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(src)/VAO.cpp -o $(a)/VAO.o $(LDFLAGS)

Shader.o:
	$(CXX) $(CXXFLAGS) -c $(src)/Shader.cpp -o $(a)/Shader.o $(LDFLAGS)

Texture.o: Shader.o
	$(CXX) $(CXXFLAGS) -c $(src)/Texture.cpp -o $(a)/Texture.o $(LDFLAGS)

Camera.o: Shader.o
	$(CXX) $(CXXFLAGS) -c $(src)/Camera.cpp -o $(a)/Camera.o $(LDFLAGS)

Mesh.o : Shader.o Buffers.o Camera.o Texture.o
	$(CXX) $(CXXFLAGS) -c $(src)/Mesh.cpp -o $(a)/Mesh.o $(LDFLAGS)
