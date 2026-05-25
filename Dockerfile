FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Europe/Moscow
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Устанавливаем Qt5 и инструменты сборки
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    qtbase5-dev \
    qt5-qmake \
    libqt5sql5-sqlite \
    && rm -rf /var/lib/apt/lists/*

# Копируем код
WORKDIR /app
COPY . /app/

# Создаём server.pro и собираем
RUN echo 'QT -= gui\n\
QT += network sql\n\
CONFIG += c++11 console\n\
CONFIG -= app_bundle\n\
\n\
SOURCES = main.cpp server.cpp database.cpp functionsserver.cpp\n\
HEADERS = server.h database.h functionsserver.h\n\
\n\
TARGET = server' > server.pro

RUN qmake server.pro && make

# Папка для БД
RUN mkdir -p /db_storage

EXPOSE 12345

WORKDIR /db_storage
ENTRYPOINT ["/app/server"]


# docker build -t my-server . - создание образа (сборка)
# docker run -d --name server-container -p 12345:12345 -v "${PWD}/data:/db_storage" my-server - создание и запуск контейнера
# docker stop server-container - остановка контейнера
# docker rm server-container - удаление контейнера
# docker rmi my-server - удаление образа
