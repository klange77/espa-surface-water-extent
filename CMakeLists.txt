
cmake_minimum_required ( VERSION 2.8 )

#-----------------------------------------------------------------------------
# To install where you want do the following
# cmake CMakeLists.txt -DCMAKE_INSTALL_PREFIX=$PREFIX

set ( ESPA_COMPILE_FLAGS "-Wall -m64 -O2 -g" )
set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${ESPA_COMPILE_FLAGS}" )
#SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )

add_subdirectory ( scripts )
add_subdirectory ( src )

########################### Un-Installing software ###########################
# Do this for a complex uninstall
#add_custom_target (
#    uninstall
#    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
#)
# Do this for a simple uninstall everything
add_custom_target ( uninstall COMMAND xargs rm < install_manifest.txt )

# Add a distclean target to the Makefile
add_custom_target (distclean COMMAND ${CMAKE_COMMAND} -P ${CMAKE_SOURCE_DIR}/distclean.cmake
)

