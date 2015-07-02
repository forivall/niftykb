# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)
find_package(Qt5Gui)

include_directories(${Qt5Gui_PRIVATE_INCLUDE_DIRS})
find_package(Qt5Widgets REQUIRED)
find_package(DirectX REQUIRED)
find_package(Boost REQUIRED)

qt5_wrap_ui(UI_HEADERS ShortcutEditor.ui)
qt5_add_resources(UI_RESOURCES resources.qrc)

include_directories(
  ${DIRECTX_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
)

# Tell CMake to create the helloworld executable
add_executable(
  niftykb WIN32 main.cpp
  ShortcutEditor.cpp ShortcutEditorWidgets.cpp
  MainWindow.cpp
  GlobalShortcut.cpp
  GlobalShortcut_win.cpp os_win.cpp
  Timer.cpp Settings.cpp Global.cpp
  niftykb.rc
  ${UI_RESOURCES}
  ${UI_HEADERS}
)

# Use the Widgets module from Qt 5.
target_link_libraries(niftykb Qt5::Widgets ${DIRECTX_LIBRARY_input} ${DIRECTX_LIBRARY_guid})
