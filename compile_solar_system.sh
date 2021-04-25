# Download ImGui and paste the files imgui_demo.cpp, imgui_draw.cpp, imgui_impl_glfw.cpp, imgui_impl_opengl3.cpp, imgui_tables.cpp, imgui_widgets.cpp
# into the vender/imgui folder.
# Make sure you have Assimp, GLEW, GLFW, and GLM installed in the global /usr/include/ folder.
# Then run this script.

BUILD_DIR="./build"
IMGUI_DIR="./vendor/imgui"

CXXFLAGS="-g -Wall -Wformat"
LDLIBS="-lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp"

mkdir -p $BUILD_DIR

# IMPORTANT #
# g++ $CXXFLAGS -c -o  $BUILD_DIR/imgui.o $IMGUI_DIR/imgui.cpp
# g++ $CXXFLAGS -c -o  $BUILD_DIR/imgui_demo.o $IMGUI_DIR/imgui_demo.cpp
# g++ $CXXFLAGS -c -o  $BUILD_DIR/imgui_draw.o $IMGUI_DIR/imgui_draw.cpp
# g++ $CXXFLAGS -c -o  $BUILD_DIR/imgui_impl_glfw.o $IMGUI_DIR/imgui_impl_glfw.cpp
# g++ $CXXFLAGS -c -o  $BUILD_DIR/imgui_impl_opengl3.o $IMGUI_DIR/imgui_impl_opengl3.cpp
# g++ $CXXFLAGS -c -o  $BUILD_DIR/imgui_tables.o $IMGUI_DIR/imgui_tables.cpp
# g++ $CXXFLAGS -c -o  $BUILD_DIR/imgui_widgets.o $IMGUI_DIR/imgui_widgets.cpp
# echo ">> Finished compiling ImGui."

g++ $CXXFLAGS -c -o $BUILD_DIR/solar_system.o solar_system.cpp
echo ">> Finished compiling solar_system."

g++ $CXXFLAGS -o mySolarSystem $BUILD_DIR/solar_system.o $BUILD_DIR/imgui.o $BUILD_DIR/imgui_demo.o $BUILD_DIR/imgui_draw.o $BUILD_DIR/imgui_impl_glfw.o $BUILD_DIR/imgui_impl_opengl3.o $BUILD_DIR/imgui_tables.o $BUILD_DIR/imgui_widgets.o $LDLIBS
echo ">> Finished compiling, linking, and building mySolarSystem."