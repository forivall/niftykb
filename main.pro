TEMPLATE = subdirs
CONFIG *= ordered debug_and_release

!CONFIG(no-client) {

  SUBDIRS *= mumblesrc/src/mumble

  win32:CONFIG(static) {
    SUBDIRS *= src/mumble_exe
  }

}
DIST=LICENSE.md README.md
