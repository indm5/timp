# Базовый образ Ubuntu 22.04
FROM ubuntu:22.04

# Отключаем интерактивные запросы при установке пакетов
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем зависимости для сборки сервера
RUN apt-get update && \
    apt-get install -y \
    qt6-base-dev \          # Qt6 Core
    qt6-tools-dev \         # Qt6 инструменты
    cmake \                 # Система сборки
    build-essential \       # Компиляторы gcc/g++
    libsqlite3-dev \        # База данных SQLite
    && rm -rf /var/lib/apt/lists/*  # Очистка кэша

# Создаём рабочую директорию внутри контейнера
WORKDIR /app

# Копируем файлы сборки
COPY CMakeLists.txt .

# Копируем исходники сервера
COPY main.cpp .
COPY server.cpp server.h .
COPY database.cpp database.h .
COPY functionsserver.cpp functionsserver.h .
COPY clienthandler.cpp clienthandler.h .

# Собираем сервер (cmake + make)
RUN cmake . && make

# Открываем порт 12345 (сервер слушает этот порт)
EXPOSE 12345

# Команда запуска сервера при старте контейнера
CMD ["./Server"]
