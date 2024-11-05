# Внешняя компонента Rdkafka-1C

Версия 1.1.1

[Сборка внешней компоненты](./doc/build.md)
- [Менеджер пакетов vcpkg](./doc/vcpkg.md)
- [Настройка cmake](./doc/cmake.md)
- [Тестовый Apache Kafka и веб интерфейс](./doc/kafka.md)

[Использование](./doc/how-to-use.md)

Rdkafka-1c - это внешняя компонента разработанная на языке C++ для обмена приложений на языке 1С с брокером сообщений Kafka. Компонента использует производительную многопоточную библиотеку [librdkafka](https://github.com/confluentinc/librdkafka) и разработана в соответствии с рекомендациями фирмы 1С по разработке [внешних компонент](https://its.1c.ru/db/metod8dev/content/3221/hdoc).

![Структура компоненты](doc/res/structure.png)

## Контрибьюторам

Разработка ведется по [Gitflow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow), любой вклад в проект приветствуется. Чтобы задать вопрос или для своих предложений по развитию вы можете открывать [issue](https://github.com/sv-sand/rdkafka-1c/issues). Если вы можете сделать pull request и решить [issue](https://github.com/sv-sand/rdkafka-1c/issues) - это будет в двойне круто.

## Ссылки

- [Технология создания внешних компонент](https://its.1c.ru/db/metod8dev/content/3221/hdoc)
- [librdkafka](https://github.com/confluentinc/librdkafka)
- [googletest](https://github.com/google/googletest)
- [vcpkg](https://github.com/microsoft/vcpkg)
- [cmake](https://cmake.org)