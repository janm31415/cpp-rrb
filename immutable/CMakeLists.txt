
set(HDRS
rrb.h
rrb_debug.h
rrb_transient.h
vector.h
)
	
set(SRCS
immutable.cpp
)

set(CMAKE_C_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "/W4 /MP /GF /RTCu /Od /MDd /Zi")
set(CMAKE_C_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "/W4 /MP /GF /O2 /Ob2 /Oi /Ot /MD /Zi /DNDEBUG")

add_definitions(-D_SCL_SECURE_NO_WARNINGS)

add_library(immutable STATIC ${HDRS} ${SRCS})
source_group("Header Files" FILES ${HDRS})
source_group("Source Files" FILES ${SRCS})

target_include_directories(immutable
  PRIVATE
  )	
	
target_link_libraries(immutable
  PRIVATE	
  )	