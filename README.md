![Coverage on main](https://ellysparky.github.io/GDM/main.svg)

![Coverage on develop](https://ellysparky.github.io/GDM/develop.svg)

# Game Dev Mate

Game Dev Mate (**GDM**) is a game development library on C++, meant to be used by the [GDM GUI](https://github.com/EllySparky/GDM_GUI.git) development engine, but you're more than welcome to use it for any personal project.

This project is still on an alpha state, any collaboration is welcomed.

# Build the library

To build the library from source clone the main branch of this repository and inside the project's folder execute the following commands.

```shell
mkdir build
cd build
cmake ..
make
```

After running the above commands, the static library file (e.g., libGDMBasics.a) will be located in the build/lib/GDMBasics/ directory. Please note that this will also build all the examples of the project which might not be what you're looking for

# Including the library in your project

To include the libGDMBasics.a library in your own project, follow these steps:

- Copy the lib/ folder to your own project.
- Include library to your project's build system (e.g., CMake).

Example CMakeLists.txt:

```cmake
cmake_minimum_required(VERSION 3.25)
project(MyProject)

# Build the library
add_subdirectory(lib)

# Add source file/s
add_executable(${PROJECT_NAME} main.cpp)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC GDMBasics)

#[[
    Copy the resources directory (where image or other type of files can be found)
    to the build directory. If you don't have a resources directory delete this. If your
    resources directory has a different name update the last two lines inside the parenthesis.
]]
add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
)
```

# How to use the library

The GDM library can be found on the lib/GDMBasics folder.

To make your own project you should make a main.cpp following this basic structure.

```c++
    #include "GDMBasics.h"

    int main()
    {
        const auto game = mate::start();
        game->gameLoop();
    }

    namespace mate
    {
        game_instance start()
        {
            const auto main_room = std::make_shared<Room>();
            // getGame(widnow's width, window's height, widnow'w name, main_room)
            auto game = Game::getGame(480, 360, "Game Name", main_room);
            
            // ADD YOUR CODE HERE
            
            return game;
        }
    }
```

To see more on how the GDM library works check the examples/ folders starting from the examples/example_template/ folder, or check the library's code yourself.
