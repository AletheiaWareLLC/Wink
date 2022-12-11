set(LIBRARY_NAME libWink)

set(TARGET_NAME Hierarchy)

set(SOURCE
    "hierarchy.cpp"
)

include_directories("../../include")

add_executable(${TARGET_NAME} ${SOURCE})

target_link_libraries(${TARGET_NAME} ${LIBRARY_NAME})
