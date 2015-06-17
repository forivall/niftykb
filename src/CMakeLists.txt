# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)
find_package(Qt5Widgets REQUIRED)
qt5_wrap_ui(UI_HEADERS ShortcutDialog.ui)
# Tell CMake to create the helloworld executable
add_executable(dinputkeydebug WIN32 main.cpp ShortcutDialog.cpp ${UI_HEADERS})

# Use the Widgets module from Qt 5.
target_link_libraries(dinputkeydebug Qt5::Widgets)
