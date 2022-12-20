set(LIBRARY_NAME libWink)

set(TARGET_NAME Empty)

set(SOURCE
    "empty.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

set(TARGET_NAME Leaf)

set(SOURCE
    "leaf.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

set(TARGET_NAME Simple)

set(SOURCE
    "simple.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

set(TARGET_NAME Bigger)

set(SOURCE
    "bigger.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})

set(TARGET_NAME Forrest)

set(SOURCE
    "forrest.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})
