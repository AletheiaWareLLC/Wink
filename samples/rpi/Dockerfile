FROM balenalib/raspberrypi3-debian:latest

EXPOSE 42000/udp

RUN apt-get update -y
RUN apt-get install -y --no-install-recommends build-essential cmake wget

RUN wget https://project-downloads.drogon.net/wiringpi-latest.deb
RUN dpkg -i wiringpi-latest.deb

COPY ./include /Wink/include
COPY ./samples /Wink/samples
COPY ./src /Wink/src
COPY ./test /Wink/test
COPY ./CMakeLists.txt /Wink/CMakeLists.txt
COPY ./LICENSE /Wink/LICENSE
COPY ./README.md /Wink/README.md

WORKDIR /Wink

RUN cmake -S . -B build -D BUILD_RPI_SAMPLES=ON
RUN cmake --build build
RUN (cd build/test/src/ && ctest)
