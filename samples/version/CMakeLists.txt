set(LIBRARY_NAME libWink)

set(INCLUDE_DIR "../../include")

foreach(version IN ITEMS 0.0.1 0.0.2 0.0.3)

    set(TARGET_NAME Version_v${version})

    set(SOURCE
        "version.cpp"
    )

    include_directories(${INCLUDE_DIR})

    add_executable(${TARGET_NAME} ${SOURCE})

    target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

endforeach()
