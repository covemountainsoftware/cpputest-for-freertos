include_directories(include)
add_library(buttonReader include/buttonReader.h src/buttonReader.c)
target_include_directories(buttonReader PUBLIC include)
