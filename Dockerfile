FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    qt6-base-dev \
    qt6-tools-dev \
    cmake \
    build-essential \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt .
COPY main.cpp .
COPY server.cpp server.h .
COPY database.cpp database.h .
COPY functionsserver.cpp functionsserver.h .
COPY clienthandler.cpp clienthandler.h .

RUN cmake . && make

EXPOSE 12345

CMD ["./Server"]