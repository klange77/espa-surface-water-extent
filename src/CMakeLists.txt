
cmake_minimum_required ( VERSION 2.6.4 )

include ( FindESPALibXML2.cmake )
include ( FindESPALibCommon.cmake )

find_library ( Zlib_Library z )
find_library ( Pthread_Library pthread )
find_library ( Rt_Library rt )
find_library ( Math_Library m )

include_directories ( ${LibESPA_INCLUDES} ${LibXML2_INCLUDE_DIRS} )

add_executable ( dswe dswe.c utilities.c get_args.c input.c output.c )

target_link_libraries ( dswe ${LibESPA_LIBRARIES} ${LibXML2_LIBRARIES} ${Z_Library} ${Pthread_Library} ${Rt_Library} ${Math_Library} )

install_programs ( /bin FILES dswe )

