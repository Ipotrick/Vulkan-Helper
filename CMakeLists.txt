cmake_minimum_required(VERSION 3.12)

project(Vulkan-Helper VERSION 0.1.1)

find_package(Vulkan REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(${PROJECT_NAME} INTERFACE Vulkan::Vulkan fmt::fmt)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

option(VULKAN_HELPER_BUILD_TESTS "Turn on to build tests" OFF)
option(VULKAN_HELPER_BUILD_SAMPLES "Turn on to build samples" OFF)

if (VULKAN_HELPER_BUILD_TESTS)
    add_subdirectory(tests)
endif()
if (VULKAN_HELPER_BUILD_SAMPLES)
    add_subdirectory(samples)
endif()

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(cmake/config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/Vulkan-Helper-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_DATADIR}/Vulkan-Helper
	NO_SET_AND_CHECK_MACRO)
write_basic_package_version_file(
	${CMAKE_CURRENT_BINARY_DIR}/Vulkan-Helper-config-version.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/Vulkan-Helper-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/Vulkan-Helper-config-version.cmake
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/Vulkan-Helper)

install(TARGETS Vulkan-Helper EXPORT Vulkan-Helper-targets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT Vulkan-Helper-targets
	NAMESPACE Vulkan-Helper::
	DESTINATION ${CMAKE_INSTALL_DATADIR}/Vulkan-Helper)
