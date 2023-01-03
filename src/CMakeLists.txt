################################################################
# Libraries

set(LIBRARY_SOURCES
  "address.cpp"
  "client.cpp"
  "delay.cpp"
  "log.cpp"
  "machine.cpp"
  "udp.cpp"
)

include_directories("../include")

set(LIBRARY_NAME libWink)

add_library(${LIBRARY_NAME} ${LIBRARY_SOURCES})

set(SERVER_LIBRARY_SOURCES
  "server/server.cpp"
)

include_directories("../include")

set(SERVER_LIBRARY_NAME libWinkServer)

add_library(${SERVER_LIBRARY_NAME} ${SERVER_LIBRARY_SOURCES})

################################################################
# Executables

set(CLIENT_SOURCES
  "client/main.cpp"
)

set(CLIENT_NAME Wink)

add_executable(${CLIENT_NAME} ${CLIENT_SOURCES})

target_link_libraries(${CLIENT_NAME} ${LIBRARY_NAME})

set(SERVER_SOURCES
  "server/main.cpp"
)

set(SERVER_NAME WinkServer)

add_executable(${SERVER_NAME} ${SERVER_SOURCES})

target_link_libraries(${SERVER_NAME} ${SERVER_LIBRARY_NAME})
target_link_libraries(${SERVER_NAME} ${LIBRARY_NAME})
