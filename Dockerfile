FROM ubuntu:22.04

# Отключаем интерактивные запросы
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем зависимости для сборки
RUN apt-get update && \
    apt-get install -y \
    qt6-base-dev \
    qt6-tools-dev \
    cmake \
    build-essential \
    libsqlite3-dev \
    && rm -rf /var/lib/apt/lists/*

# Создаём рабочую директорию
WORKDIR /app

# Копируем исходники сервера
COPY CMakeLists.txt .
COPY main.cpp .
COPY server.cpp server.h .
COPY database.cpp database.h .
COPY functionsserver.cpp functionsserver.h .
COPY clienthandler.cpp clienthandler.h .

# Собираем сервер
RUN cmake . && make

# Открываем порт (сервер слушает 12345)
EXPOSE 12345

# Запускаем сервер при старте контейнера
CMD ["./Server"]

#docker build -t graph-server . - создание образа
#docker run -d -p 12345:12345 --name graph-server-container graph-server - создание контейнера
