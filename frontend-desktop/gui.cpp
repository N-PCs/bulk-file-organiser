// gui.cpp now delegated to gui_win32.cpp (Windows) or gui_fltk.cpp (Linux)
// Windows: build.bat compiles gui_win32.cpp
// Linux:   build.sh compiles gui_fltk.cpp + core.cpp
//
// The platform-specific files contain the full GUI implementations.
// This file remains as a placeholder/redirect for backward compatibility.

#if defined(_WIN32)
#  include "gui_win32.cpp"
#else
#  error "Use build.sh (Linux) which compiles gui_fltk.cpp directly."
#endif
