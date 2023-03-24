cmake_minimum_required(VERSION 3.7)
add_library(RdEditorRoot STATIC 
instantiations_RdEditorRoot.h
instantiations_RdEditorRoot.cpp
RdEditorModel/RdEditorModel.Generated.cpp
RdEditorModel/RdEditorModel.Generated.h
RdEditorRoot/RdEditorRoot.Generated.cpp
RdEditorRoot/RdEditorRoot.Generated.h)
target_include_directories(RdEditorRoot PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RdEditorRoot PUBLIC rd_framework_cpp)
