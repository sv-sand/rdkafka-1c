# Программный интерфейс

## Свойства компоненты

| Имя на русском языке | Имя на английском языке | Чтение | Установка | Описание                                                                                                                                                                                                  |
| ----------------------------- | -------------------------------- | ------ | --------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ВерсияКомпоненты              | ComponentVersion                 | +      |           | Содержит текущую версию компоненты                                                                                                                                                                        |
| ВерсияRdKafka                 | RdKafkaVersion                   | +      |           | Содержит версию библиотеки libRdKafka которую использует                                                                                                                                                  |
| Локаль                        | Locale                           | +      | +         | Системная локаль на машине, где запущена компонента, должна соответствовать отправляемым данным. Внимание! Если локаль en_EN, а вы пытаетесь отправить русский текст, он будет уходить в виде иероглифов. |
| ЛогФайл                       | LogFile                          | +      |           | Имя файла в который пишутся логи.                                                                                                                                                                         |
| УровеньЛогирования            | LogLevel                         | +      | +         | Текущий уровень логирования.                                                                                                                                                                              |
| ТаймаутОпераций               | OperationTimeout                 | +      | +         | Время в миллисекундах выделенное для выполнения операций, если операция выполняется дольше, библиотека RdKafka прерывает её.                                                                              |
| Отказ                         | Error                            | +      |           | Флаг отказа. Сбрасывается при начале каждой операции, чтобы после выполнения операции можно было посмотреть её ошибки. Если ошибка произошла, будет заполнено свойство ОписаниеОшибки (ErrorDescription). |
| ОписаниеОшибки                | ErrorDescription                 | +      |           | Описание ошибки, если флаг Error был взведен. 

## Методы компоненты

| Имя на русском языке              | Имя на английском языке  | Описание                                                                                                                                         |
| --------------------------------- | ------------------------ | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| НачатьЛогирование                 | StartLogging             | Запускает логирование операций компоненты и библиотеки libRrKafka.                                                                               |
| ОстановитьЛогирование             | StopLogging              | Останавливает логирование компоненты и библиотеки libRdKafka.                                                                                    |
| УстановитьПараметр                | SetConfigProperty        | Устанавливает свойство конфигурации.                                                                                                             |
| ИнициализироватьПродюсера         | InitProducer             | Инициализирует продюсера.                                                                                                                        |
| НачатьОтправку                    | StartProduce             | Подготавливает новый массив сообщений к отправке. На данном этапе происходит очистка коллекции статусов сообщений отправленных в предыдущий раз. |
| Отправить                         | Produce                  | Добавляет сообщение в асинхронную очередь отправки.                                                                                              |
| Слить                             | Flush                    | Отправляет сообщения, которые еще не отправлены в Кафку и ожидает отчетов об отправке, таже заполняет коллекцию статусов сообщений.              |
| ОстановитьПродюсера               | StopProducer             | Останавливает продюсера.                                                                                                                         |
| ОчередьСообщенийПродюсера         | ProducerQueueLen         | Возвращает длину очереди продюсера, в которой могут быть сообщения к отправке и технические сообщения.                                           |
| КоличествоНеДоставленныхСообщений | CountUndeliveredMessages | Возвращает количество не доставленных сообщений с момента вызова метода НачатьОтправку (StartProduce).                                           |
| СтатусСообщения                   | MessageStatus            | Возвращает статус сообщения по идентификатору.                                                                                                   |
| ИнициализироватьКонсюмера         | InitConsumer             | Инициализирует консюмера.                                                                                                                        |
| Прочитать                         | Consume                  | Читает сообщение из Кафки в компоненту.                                                                                                          |
| ДанныеСообщения                   | MessageData              | Возвращает данные сообщения.                                                                                                                     |
| МетаданныеСообщения               | MessageMetadata          | Возвращает метаданные сообщения.                                                                                                                 |
| ОстановитьКонсюмера               | StopConsumer             | Останавливает консюмера.                                                                                                                         |
| УстановитьПартицию                | AssignPartition          | Устанавливает партицию из которой будут читаться сообщения.                                                                                      |
| ЗафиксироватьОффсет               | CommitOffset             | Фиксирует оффсет у сообщений, которые были прочитаны с момента инициализации консюмера или последней фиксации.                                   |
| ИзменитьОффсет                    | ChangeOffset             | Изменяет оффсет у произвольного топика и партиции. Позволяет смещать оффсет назад и вперед.                                                      |
| ПолучитьОффсет                    | CommittedOffset          | Возвращает последний зафиксированный оффсет.                                                                                                     |
| ОчередьСообщенийКонсюмера         | ConsumerQueueLen         | Возвращает длину очереди консюмера, в которой могут быть сообщения фиксации оффсета, подписки на топик и технические сообщения.                  |
| Подписаться                       | Subscribe                | Подписывает консюмера на топик.                                                                                                                  |
| Отписаться                        | Unsubscribe              | Отписывает консюмера от всех топиков.                                                                                                            |
| Подписки                          | Subscription             | Возвращает список топиков на которые подписан консюмер, разделеннх точкой-запятой.
