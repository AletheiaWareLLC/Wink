OPTION(BUILD_RPI_SAMPLES "Build the Raspberry Pi samples" OFF)
if(BUILD_RPI_SAMPLES)

    set(LIBRARY_NAME libWink)

    set(WIRING_PI_NAME wiringPi)

    set(INCLUDE_DIR "../../include")

    set(TARGET_NAME GPIOReader)

    set(SOURCE
        "gpioreader.cpp"
    )

    include_directories(${INCLUDE_DIR})

    add_executable(${TARGET_NAME} ${SOURCE})

    target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME} ${WIRING_PI_NAME})

    set(TARGET_NAME GPIOWriter)

    set(SOURCE
        "gpiowriter.cpp"
    )

    include_directories(${INCLUDE_DIR})

    add_executable(${TARGET_NAME} ${SOURCE})

    target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME} ${WIRING_PI_NAME})

    set(TARGET_NAME Blinky)

    set(SOURCE
        "blinky.cpp"
    )

    include_directories(${INCLUDE_DIR})

    add_executable(${TARGET_NAME} ${SOURCE})

    target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME} ${WIRING_PI_NAME})

endif()
