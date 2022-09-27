include_directories(src)
add_executable(Sonograf main.cpp imageCapture.cpp imageRead.cpp physicalControls.cpp scManager.cpp)

if(NOT APPLE)
    add_subdirectory(i2c)
endif()

target_link_libraries(${PROJECT_NAME} raylib)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} oscpack)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()