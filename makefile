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
HEADERS= -I./$(h) -DGLM_ENABLE_EXPERIMENTAL
NMRFLAGS= -DNMR64 -DLINUX -I./rd
CXXFLAGS= -g -Wall -std=c++17 -I./$(inc) $(HEADERS) -I./$(inc)/imgui -I./$(inc)/GLMathematics -I./$(inc)/json
LDFLAGS= -L./$(a)/Libraries/lib -lrt -lm -ldl -lglfw3

SHAPES= $(h)/Shapes.hpp
UI= $(h)/UI.hpp

IGFD = ImGuiFileDialog
BACKEND= $(a)/Backend.o
BUFFERS= $(a)/VBO.o $(a)/EBO.o $(a)/VAO.o
SHADERS= $(a)/Shader.o
TEXTURES= $(a)/Texture.o
CAMERA= $(a)/Camera.o
MESH= $(a)/Mesh.o
LINE= $(a)/Line.o
NMR= $(a)/NMRMesh.o
MODEL= $(a)/Model.o
CUBEMAP = $(a)/Cubemap.o

DEPS= $(IGFD).o Backend.o Buffers.o Shader.o Texture.o Camera.o Mesh.o Line.o NMRMesh.o Model.o Cubemap.o

OBJ= $(BACKEND) $(BUFFERS) $(SHADERS) $(TEXTURES) $(CAMERA) $(MESH) $(LINE) $(NMR) $(MODEL) $(CUBEMAP) $(a)/$(IGFD).o $(SHAPES) $(UI)
NMR_H= 
NMR_OBJ= rd/readnmr.o rd/fdatap.o rd/cmndargs.o \
rd/token.o rd/stralloc.o rd/memory.o rd/fdataio.o rd/dataio.o \
rd/inquire.o rd/testsize.o rd/namelist.o rd/vutil.o rd/syscalls.o \
rd/getstat.o rd/rand.o rd/specunit.o rd/raise.o \
rd/conrecnx.o rd/drawaxis.o rd/paper.o rd/nmrgraphics.o

# Reference files
glad = glad.c stb.cpp

IMGUI = $(inc)/imgui/imgui.cpp \
		$(inc)/imgui/imgui_demo.cpp $(inc)/imgui/imgui_draw.cpp \
		$(inc)/imgui/backends/imgui_impl_glfw.cpp \
		$(inc)/imgui/backends/imgui_impl_opengl3.cpp \
		$(inc)/imgui/imgui_tables.cpp $(inc)/imgui/imgui_widgets.cpp

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

Line.o : Buffers.o Camera.o Texture.o
	$(CXX) $(CXXFLAGS) -c $(src)/Line.cpp -o $(LINE) $(LDFLAGS)

NMRMesh.o : Mesh.o
	$(CXX) $(CXXFLAGS) $(NMRFLAGS) -c $(src)/NMRMesh.cpp -o $(NMR) $(LDFLAGS)

Model.o : Mesh.o
	$(CXX) $(CXXFLAGS) -c $(src)/Model.cpp -o $(MODEL) $(LDFLAGS)

Cubemap.o : Shader.o Camera.o
	$(CXX) $(CXXFLAGS) -c $(src)/Cubemap.cpp -o $(CUBEMAP) $(LDFLAGS)