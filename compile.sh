# -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
mkdir -p build
mkdir -p build/src

# g++ -Wall -g -c ./src/utils.cpp -o ./build/src/utils.o

g++ -Wall -g -c main.cpp -o ./build/main.o

# g++ -Wall -g -o myBlender ./build/main.o ./build/src/utils.o glad.c  -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp
g++ -Wall -g -o myBlender ./build/main.o glad.c  -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lassimp

# g++ -Wall -g -o myBlender ./build/main.o glad.c  -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl


# g++ test.cpp glad.c -lglfw -lGL -lX11 -lpthread -lXrandr -lXi -ldl