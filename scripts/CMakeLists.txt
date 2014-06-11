
cmake_minimum_required ( VERSION 2.8 )

find_package ( PythonInterp )
find_package ( PythonLibs )

if ( PYTHON_VERSION_MAJOR LESS 2 )
    message ( FATAL_ERROR "Insufficient Python Major Version Found(${PYTHON_VERSION_MAJOR}) (Requires(>=2)" )
endif()

if ( PYTHON_VERSION_MINOR LESS 7 )
    message ( FATAL_ERROR "Insufficient Python Minor Version Found(${PYTHON_VERSION_MINOR}) (Requires(>=7)" )
endif()

install_programs ( /bin FILES do_surface_water_extent.py )

