# Find script for DirectX on Windows
# Specifically designed to find dxguid and dinput8 for OIS
# Once loaded this will define
#   DIRECTX_FOUND        - system has DirectX
#   DIRECTX_INCLUDE_DIR  - include directory for DirectX
#   DIRECTX_LIBRARIES    - libraries for DirectX
#
# Set ENV{DXSD_DIR} if that has not been done the SDK installation.
#
# Several changes and additions by Fabian 'x3n' Landau
# Simplifications and CMake 2.6.0 bugfix by Reto Grieder
#                 > www.orxonox.net <

INCLUDE(FindPackageHandleStandardArgs)
INCLUDE(HandleLibraryTypes)

FIND_PATH(DIRECTX_INCLUDE_DIR dinput.h
  PATHS $ENV{DXSDK_DIR}
  PATH_SUFFIXES include
)
FIND_LIBRARY(DIRECTX_LIBRARY_input dinput8
  PATHS $ENV{DXSDK_DIR}
  PATH_SUFFIXES lib Lib lib/x86 Lib/x86
  NO_DEFAULT_PATH # Or else CMake 2.6.0 will find the dll in system32 on windows
)
FIND_LIBRARY(DIRECTX_LIBRARY_input dinput8
  PATHS $ENV{DXSDK_DIR}
  PATH_SUFFIXES lib Lib lib/x86 Lib/x86
)
FIND_LIBRARY(DIRECTX_LIBRARY_guid dxguid
  PATHS $ENV{DXSDK_DIR}
  PATH_SUFFIXES lib Lib lib/x86 Lib/x86
  NO_DEFAULT_PATH # Or else CMake 2.6.0 will find the dll in system32 on windows
)
FIND_LIBRARY(DIRECTX_LIBRARY_guid dxguid
  PATHS $ENV{DXSDK_DIR}
  PATH_SUFFIXES lib Lib lib/x86 Lib/x86
)

# Handle the REQUIRED argument and set DIRECTX_FOUND
FIND_PACKAGE_HANDLE_STANDARD_ARGS(DirectX DEFAULT_MSG
  DIRECTX_LIBRARY_input
  DIRECTX_LIBRARY_guid
  DIRECTX_INCLUDE_DIR
)

SET(DIRECTX_LIBRARIES ${DIRECTX_LIBRARY_input} ${DIRECTX_LIBRARY_guid})

MARK_AS_ADVANCED(
  DIRECTX_INCLUDE_DIR
  DIRECTX_LIBRARY_input
  DIRECTX_LIBRARY_guid
)
