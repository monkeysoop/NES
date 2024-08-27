FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    openbox \
    tigervnc-standalone-server \
    tigervnc-common \
    novnc \
    websockify \
    cmake \
    libsdl2-dev \
    libsdl2-image-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*


WORKDIR /usr/src/app

COPY . .

RUN cmake -Wno-dev -B build -S . && cd build && cmake .. && cmake --build .

EXPOSE 5901 6080

RUN chmod +x entrypoint.sh

ENTRYPOINT ["/usr/src/app/entrypoint.sh"]

CMD []