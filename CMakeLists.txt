
cmake_minimum_required(VERSION 3.6)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/OS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Layer/App)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Layer/Link)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Api)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Iec)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/OS os_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Core core_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Layer/App app_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Layer/Link link_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Api api_src)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/Iec iec_src)

add_library(newiec ${os_src} ${core_src} ${app_src}  ${link_src} ${api_src} ${iec_src})
