set(LIBRARY_NAME libWink)

set(TARGET_NAME Delay)

set(SOURCE
    "delay.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

set(TARGET_NAME StopWatch)

set(SOURCE
    "stopwatch.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

set(TARGET_NAME Timer)

set(SOURCE
    "timer.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})
