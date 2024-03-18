project(ATOM)


find_package(OpenCV REQUIRED )
find_package(Threads REQUIRED)




set(SOURCES
        main.cpp
        ATOM/Application.cpp
        ATOM/Application.h
        ATOM/atompch.h
        ATOM/macros.h
        ATOM/Core/Logging/Logging.cpp
        ATOM/Core/Logging/Logging.h
        Networking/Client.cpp
        Networking/Client.h
        Networking/Server.h
        Networking/Server.cpp
        Networking/Networking.h
        Networking/Networking.cpp
        ATOM/Core/Layers/Layer.cpp
        ATOM/Core/Layers/Layer.h
        ATOM/Core/Layers/LayerStack.cpp
        ATOM/Core/Layers/LayerStack.h
        ATOM/Application.cpp

        ATOM/Core/VideoServer/Frame.cpp
        ATOM/Core/VideoServer/Frame.h
        ATOM/Core/ServerLayer.cpp
        ATOM/Core/ServerLayer.h
        ATOM/Core/VideoServer/VideoServer.cpp
        ATOM/Core/VideoServer/VideoServer.h


)



add_executable(executable_headless ${SOURCES})

target_link_libraries(executable_headless
        glm
        spdlog
        GameNetworkingSockets
        ${OpenCV_LIBS}
        Threads::Threads
)



target_include_directories(executable_headless PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)


