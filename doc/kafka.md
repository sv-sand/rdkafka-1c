# Apache Kafka и веб интерфейс

## Установка контейнеров

Необходимо выполнить команды в каталоге проекта:

1. Сборка образов Docker:

```sh
docker-compose build
```

2. Запуск контейнеров

```sh
docker-compose up -d
```

Для остановки или повторного запуска контейнеров модно воспользоваться комадной строкой или UI Docker

## Настройка веб интерфейса

В качестве UI Kafka выбран [provectuslabs/kafka-ui](https://github.com/provectus/kafka-ui)

Веб интерфейс открывается в браузере, на странице [http://localhost:8082/](http://localhost:8082/). При первом входе Kefka UI предложит ввести настройки кластера, вводим:

```
Cluster name: localhost
Bootstrap Servers: PLAINTEXT://kafka
Port: 19092
```

Тестовый топик добавится при запуске тестов.