# Внешняя компонента для обмена 1С с Kafka

Версия 0.3.1-beta

**!!! Проект в разработке !!!**

[Сборка внешней компоненты](./doc/build.md)

Структура помпоненты

```plantuml
@startuml

rectangle 1CPlatform as "Платформа 1С"

package "RdKafka1C.dll" {
    rectangle AddInNative
    rectangle RdKafka1C [
        <b>RdKafka1C</b>
        Loger
        Config
        ...
    ]
    rectangle LibRdKafka as "librdkafka"
}

note bottom of AddInNative
    Интерфейс
    компоненты
end note

note bottom of RdKafka1C
    Основная
    логика
    компоененты
end note

note bottom of LibRdKafka
    Библиотека 
    confluentinc/librdkafka
end note

rectangle Kafka as "Kafka"

1CPlatform <-> AddInNative
AddInNative <-> RdKafka1C
RdKafka1C <-> LibRdKafka
LibRdKafka <-> Kafka

@enduml
```

## Использование

## Пример обработки 1С

В каталоге /epf размещен пример обработки для демонстрации возможностей внешней компоненты.

## Ссылки

- [librdkafka](https://github.com/confluentinc/librdkafka)
- [googletest](https://github.com/google/googletest)
- [vcpkg](https://github.com/microsoft/vcpkg)