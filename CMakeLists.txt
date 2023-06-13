cmake_minimum_required(VERSION 3.13)



################################################################################
## PROJECT
## name and version
################################################################################
project(VSource LANGUAGES CXX)



################################################################################
## SETTINGS
## basic project settings before use
################################################################################
# If this project is used as a submodule, the variable should be overridden
# to "OFF" in the top-level application (to disable forced cache rewriting)
option(${PARENT}_SUBMODULE_CACHE_OVERWRITE "Enable forced cache rewriting" ON)
if (${PARENT}_SUBMODULE_CACHE_OVERWRITE)
    SET(REWRITE_FORCE "FORCE")
else()
    SET(REWRITE_FORCE "")
endif()



################################################################################
## CONFIGURATION
## project configuration
################################################################################
SET(${PARENT}_VSOURCE                        ON  CACHE BOOL "" ${REWRITE_FORCE})
SET(${PARENT}_VSOURCE_PARAMS_TEST            ON  CACHE BOOL "" ${REWRITE_FORCE})



################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the project configuration
################################################################################
if (${PARENT}_VSOURCE)
    add_subdirectory(3rdparty)
    add_subdirectory(src)
endif()

if (${PARENT}_VSOURCE_PARAMS_TEST)
    add_subdirectory(test)
endif()
