macro(COLLECT_SOURCE_CODE_AND_RESOURCES PATHS)

    #retrieve headers, sources and include paths
    set(SOURCES "")
    set(HEADERS "")
    set(DIRECTORIES "")
    set(RESOURCES "")

    foreach(PATH IN LISTS PATHS)

        #retrieve source files
        file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PATH}/*.cpp")
        file(GLOB_RECURSE CC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PATH}/*.cc")
        file(GLOB_RECURSE C_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PATH}/*.c")
        foreach(file IN LISTS CPP_FILES CC_FILES C_FILES)
        list(APPEND SOURCES ${file})
        endforeach()

        #retrieve header files
        file(GLOB_RECURSE HPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PATH}/*.hpp")
        file(GLOB_RECURSE H_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PATH}/*.h")
        foreach(file IN LISTS HPP_FILES H_FILES)
        list(APPEND HEADERS ${file})
        endforeach()

        # retrieve directories that have to be added to include paths of the compiler
        foreach(file IN LISTS HEADERS)
        GET_FILENAME_COMPONENT(f ${file} PATH)
        list(APPEND DIRECTORIES ${f})
        endforeach()

        # retrieve resources
        file(GLOB_RECURSE QRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${PATH}/*.qrc")
        foreach(file IN LISTS QRC_FILES)
        list(APPEND RESOURCES ${file})
        endforeach()

        # remove duplicates
        list(REMOVE_DUPLICATES SOURCES)
        list(REMOVE_DUPLICATES HEADERS)
        list(REMOVE_DUPLICATES DIRECTORIES)
        list(REMOVE_DUPLICATES QRC_FILES)

    endforeach()

endmacro()