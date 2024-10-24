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
FTFLAGS= -I/usr/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/sysprof-6 -pthread
CXXFLAGS= -g -Wall -std=c++17 -I./$(inc) $(HEADERS) -I./$(inc)/imgui -I./$(inc)/GLMathematics -I./$(inc)/json $(FTFLAGS)
LDFLAGS= -L./$(a)/Libraries/lib -lrt -lm -ldl -lglfw3

SHAPES= $(h)/Shapes.hpp
CONST= $(h)/Constants.hpp

IGFD = ImGuiFileDialog
IGZM = ImGuizmo
BACKEND= $(a)/Backend.o
BUFFERS= $(a)/VBO.o $(a)/EBO.o $(a)/VAO.o
SHADERS= $(a)/Shader.o
TEXTURES= $(a)/Texture.o
CAMERA= $(a)/Camera.o
LIGHT= $(a)/Light.o
TYPE= $(a)/Type.o
MESH= $(a)/Mesh.o
LINE= $(a)/Line.o
NMR= $(a)/NMRMesh.o
MODEL= $(a)/Model.o
FBO = $(a)/FBO.o
CUBEMAP = $(a)/Cubemap.o
UI= $(a)/UI.o

DEPS= $(IGFD).o $(IGZM).o Backend.o Buffers.o Shader.o Texture.o Camera.o Mesh.o Type.o Light.o Line.o NMRMesh.o Model.o FBO.o Cubemap.o

OBJ= $(BACKEND) $(BUFFERS) $(FBO) $(SHADERS) $(TEXTURES) $(CAMERA) $(MESH) $(LINE) $(NMR) $(MODEL) $(LIGHT) $(TYPE) $(CUBEMAP) $(a)/$(IGFD).o $(a)/$(IGZM).o $(SHAPES) $(UI) $(CONST)
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
	$(CXX) $(CXXFLAGS) $(NMRFLAGS) main.cpp $(glad) $(NMR_OBJ) $(OBJ) $(IMGUI) -o main $(LDFLAGS) $(FTFLAGS)
clean:
	rm -rf $(a)/*.o

$(IGFD).o:
	$(CXX) $(CXXFLAGS) -c $(inc)/$(IGFD)/$(IGFD).cpp -o $(a)/$(IGFD).o $(LDFLAGS)

$(IGZM).o:
	$(CXX) $(CXXFLAGS) -c $(inc)/$(IGZM)/$(IGZM).cpp -o $(a)/$(IGZM).o $(LDFLAGS)

Backend.o: $(IGZM).o $(IGFD).o
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

UI.o : Backend.o Shader.o Camera.o
	$(CXX) $(CXXFLAGS) -c $(src)/UI.cpp -o $(UI) $(LDFLAGS)
	
Mesh.o : Shader.o Buffers.o Camera.o Texture.o
	$(CXX) $(CXXFLAGS) -c $(src)/Mesh.cpp -o $(MESH) $(LDFLAGS)

Line.o : Buffers.o Camera.o Texture.o
	$(CXX) $(CXXFLAGS) -c $(src)/Line.cpp -o $(LINE) $(LDFLAGS)

NMRMesh.o : Mesh.o UI.o
	$(CXX) $(CXXFLAGS) $(NMRFLAGS) -c $(src)/NMRMesh.cpp -o $(NMR) $(LDFLAGS)

Model.o : Mesh.o
	$(CXX) $(CXXFLAGS) -c $(src)/Model.cpp -o $(MODEL) $(LDFLAGS)

Light.o : Mesh.o
	$(CXX) $(CXXFLAGS) -c $(src)/Light.cpp -o $(LIGHT) $(LDFLAGS)

Type.o : Shader.o
	$(CXX) $(CXXFLAGS) -c $(src)/Type.cpp -o $(TYPE) $(LDFLAGS) $(FTFLAGS)

FBO.o: Buffers.o Shader.o Camera.o NMRMesh.o
	$(CXX) $(CXXFLAGS) $(NMRFLAGS) -c $(src)/FBO.cpp -o $(FBO) $(LDFLAGS)

Cubemap.o : Shader.o Camera.o
	$(CXX) $(CXXFLAGS) -c $(src)/Cubemap.cpp -o $(CUBEMAP) $(LDFLAGS)	