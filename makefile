# ---------
# PATHS
# ---------

a = Assets
h = $(a)/Headers
src = $(a)/Source
shdr= $(a)/Shaders
tex= $(a)/Textures
inc= $(a)/Libraries/include

# ---------
# VARIABLES
# ---------

CXX=g++
HEADERS= -I./$(h)
NMRFLAGS= -DNMR64 -DLINUX -I./rd
CXXFLAGS= -g -Wall -std=c++17 -I./$(a)/Libraries/include $(HEADERS) -I./imgui
LDFLAGS= -L./$(a)/Libraries/lib -lrt -lm -ldl -lglfw3

SHAPES= $(h)/Shapes.h
UI= $(h)/UI.h

IGFD = ImGuiFileDialog
BACKEND= $(a)/Backend.o
BUFFERS= $(a)/VBO.o $(a)/EBO.o $(a)/VAO.o
SHADERS= $(a)/Shader.o
TEXTURES= $(a)/Texture.o
CAMERA= $(a)/Camera.o
MESH= $(a)/Mesh.o
NMR= $(a)/NMRMesh.o
MODEL= $(a)/Model.o
CUBEMAP = $(a)/Cubemap.o

DEPS= $(IGFD).o Backend.o Buffers.o Shader.o Texture.o Camera.o Mesh.o NMRMesh.o Model.o Cubemap.o

OBJ= $(BACKEND) $(BUFFERS) $(SHADERS) $(TEXTURES) $(CAMERA) $(MESH) $(NMR) $(MODEL) $(CUBEMAP) $(a)/$(IGFD).o $(SHAPES) $(UI)
NMR_H= 
NMR_OBJ= rd/readnmr.o rd/fdatap.o rd/cmndargs.o \
rd/token.o rd/stralloc.o rd/memory.o rd/fdataio.o rd/dataio.o \
rd/inquire.o rd/testsize.o rd/namelist.o rd/vutil.o rd/syscalls.o \
rd/getstat.o rd/rand.o rd/specunit.o rd/raise.o \
rd/conrecnx.o rd/drawaxis.o rd/paper.o rd/nmrgraphics.o

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

draw: $(DEPS)
	$(CXX) $(CXXFLAGS) $(NMRFLAGS) draw.cpp $(glad) $(NMR_OBJ) $(OBJ) $(IMGUI) -o draw $(LDFLAGS)

main: $(DEPS)
	$(CXX) $(CXXFLAGS) $(NMRFLAGS) main.cpp $(glad) $(NMR_OBJ) $(OBJ) $(IMGUI) -o main $(LDFLAGS)
clean:
	rm -rf $(a)/*.o

$(IGFD).o:
	$(CXX) $(CXXFLAGS) -c $(inc)/$(IGFD)/$(IGFD).cpp -o $(a)/$(IGFD).o $(LDFLAGS)
Backend.o:
	$(CXX) $(CXXFLAGS) -c $(src)/Backend.cpp -o $(BACKEND) $(LDFLAGS)

Buffers.o:
	$(CXX) $(CXXFLAGS) -c $(src)/VBO.cpp -o $(a)/VBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(src)/EBO.cpp -o $(a)/EBO.o $(LDFLAGS)
	$(CXX) $(CXXFLAGS) -c $(src)/VAO.cpp -o $(a)/VAO.o $(LDFLAGS)

Shader.o:
	$(CXX) $(CXXFLAGS) -c $(src)/Shader.cpp -o $(SHADERS) $(LDFLAGS)

Texture.o: Shader.o
	$(CXX) $(CXXFLAGS) -c $(src)/Texture.cpp -o $(TEXTURES) $(LDFLAGS)

Camera.o: Shader.o
	$(CXX) $(CXXFLAGS) -c $(src)/Camera.cpp -o $(CAMERA) $(LDFLAGS)

Mesh.o : Shader.o Buffers.o Camera.o Texture.o
	$(CXX) $(CXXFLAGS) -c $(src)/Mesh.cpp -o $(MESH) $(LDFLAGS)

NMRMesh.o : Mesh.o
	$(CXX) $(CXXFLAGS) $(NMRFLAGS) -c $(src)/NMRMesh.cpp -o $(NMR) $(LDFLAGS)

Model.o : Mesh.o
	$(CXX) $(CXXFLAGS) -c $(src)/Model.cpp -o $(MODEL) $(LDFLAGS)

Cubemap.o : Shader.o Camera.o
	$(CXX) $(CXXFLAGS) -c $(src)/Cubemap.cpp -o $(CUBEMAP) $(LDFLAGS)