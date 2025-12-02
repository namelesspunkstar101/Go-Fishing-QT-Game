# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\COP_3003_Final_Project_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\COP_3003_Final_Project_autogen.dir\\ParseCache.txt"
  "COP_3003_Final_Project_autogen"
  )
endif()
