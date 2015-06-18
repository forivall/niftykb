# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)
find_package(Qt5Widgets REQUIRED)
find_package(DirectX REQUIRED)
qt5_wrap_ui(UI_HEADERS ShortcutDialog.ui)

include_directories(
  ${DIRECTX_INCLUDE_DIR}
)

# Tell CMake to create the helloworld executable
add_executable(
  dinputkeydebug WIN32 main.cpp
  ShortcutDialog.cpp
  GlobalShortcut.cpp
  GlobalShortcut_win.cpp
  Timer.cpp Settings.cpp Global.cpp
  ${UI_HEADERS}
)

# Use the Widgets module from Qt 5.
target_link_libraries(dinputkeydebug Qt5::Widgets ${DIRECTX_LIBRARY_input} ${DIRECTX_LIBRARY_guid})
