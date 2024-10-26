﻿
#Область ОбработчикиСобытийФормы

&НаСервере
Процедура ПриСозданииНаСервере(Отказ, СтандартнаяОбработка)

	
	
КонецПроцедуры

&НаКлиенте
Процедура ПриОткрытии(Отказ)
	
	ОбновитьЭлементы();	
	
КонецПроцедуры

#КонецОбласти

#Область ОбработчикиСобытийЭлементовФормы

&НаКлиенте
Процедура ПартицияСообщенияАвтоПриИзменении(Элемент)
	ОбновитьЭлементы();
КонецПроцедуры

#КонецОбласти

#Область ОбработчикиКомандФормы

&НаКлиенте
Процедура СоздатьКоннектор(Команда)	
	СоздатьКоннекторНаСервере();
КонецПроцедуры

&НаСервере
Процедура СоздатьКоннекторНаСервере()

	Если НЕ ЗначениеЗаполнено(НастройкиКафки) Тогда
		Возврат;	
	КонецЕсли;
	
	Если ЗначениеЗаполнено(АдресКоннектора)
		И ВосстановитьКоннектор() <> Неопределено
		Тогда
		ВызватьИсключение "Коннектор уже создан";
	КонецЕсли;
	
	Коннектор = Обработки.КафкаКоннектор.Создать();
	Коннектор.Инициализировать(НастройкиКафки.Идентификатор);
	
	Объект.ВерсияКомпоненты = Коннектор.ВерсияКомпоненты;
	Объект.ВерсияRdKafka = Коннектор.ВерсияRdKafka;
	
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

&НаКлиенте
Процедура ОстановитьКоннектор(Команда)
	ОстановитьКоннекторНаСервере();
КонецПроцедуры

&НаСервере
Процедура ОстановитьКоннекторНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	Коннектор.Остановить();
	Коннектор = Неопределено;
	
	АдресКоннектора = "";
	
КонецПроцедуры

#Область Продюсер

&НаКлиенте
Процедура ОтправитьСообщения(Команда)
	ОтправитьСообщенияНаСервере();	
КонецПроцедуры

&НаСервере
Процедура ОтправитьСообщенияНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	
	ДобавлятьНомерСообщения = КоличествоСообщенийКОтправке > 1 И Лев(СокрЛП(ТекстСообщенияКОтправке), 1) <> "{";
	
	ВремяНачала = ТекущаяУниверсальнаяДатаВМиллисекундах();
	
	Для НомерСообщения = 1 По КоличествоСообщенийКОтправке Цикл
		Текст = ТекстСообщенияКОтправке;
		Если ДобавлятьНомерСообщения Тогда
			Текст = Текст + " " + НомерСообщения; 	
		КонецЕсли;
		
		Если ПартицияСообщенияАвто Тогда
			Коннектор.ОтправитьСообщение(Текст, КлючСообщения, ЗаголовкиСообщения);
		Иначе
			Коннектор.ОтправитьСообщение(Текст, КлючСообщения, ЗаголовкиСообщения, ПартицияСообщения);
		КонецЕсли;
					
	КонецЦикла;
	
	ВремяОкончания = ТекущаяУниверсальнаяДатаВМиллисекундах();
	
	ТекстСообщения = СтрШаблон("Отправлено %1 сообщений за %2 секунд", 
	 	КоличествоСообщенийКОтправке,
		(ВремяОкончания - ВремяНачала) / 1000);
	ОбщегоНазначения.СообщитьПользователю(ТекстСообщения);
	
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

&НаКлиенте
Процедура ОтправитьСообщенияАсинхронно(Команда)
	ОтправитьСообщенияАсинхронноНаСервере();
КонецПроцедуры

&НаСервере
Процедура ОтправитьСообщенияАсинхронноНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	
	ДобавлятьНомерСообщения = КоличествоСообщенийКОтправке > 1 И Лев(СокрЛП(ТекстСообщенияКОтправке), 1) <> "{";
	
	МассивСообщений = Новый Массив;
	Для НомерСообщения = 1 По КоличествоСообщенийКОтправке Цикл
		Текст = ТекстСообщенияКОтправке;
		Если ДобавлятьНомерСообщения Тогда
			Текст = Текст + " " + НомерСообщения; 	
		КонецЕсли;
		
		Если ПартицияСообщенияАвто Тогда
			Сообщение = Коннектор.НовоеСообщение(Текст, КлючСообщения, ЗаголовкиСообщения);
		Иначе
			Сообщение = Коннектор.НовоеСообщение(Текст, КлючСообщения, ЗаголовкиСообщения, ПартицияСообщения);
		КонецЕсли;
		
		МассивСообщений.Добавить(Сообщение);					
	КонецЦикла;
	
	ВремяНачала = ТекущаяУниверсальнаяДатаВМиллисекундах();
	Коннектор.ОтправитьСообщения(МассивСообщений);
	ВремяОкончания = ТекущаяУниверсальнаяДатаВМиллисекундах();
	
	ТекстСообщения = СтрШаблон("Отправлено %1 сообщений за %2 секунд", 
	 	КоличествоСообщенийКОтправке,
		(ВремяОкончания - ВремяНачала) / 1000);
	ОбщегоНазначения.СообщитьПользователю(ТекстСообщения);
	
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

&НаКлиенте
Процедура ОчередьСообщенийПродюсера(Команда)
	ОчередьСообщенийПродюсераНаСервере();
КонецПроцедуры

&НаСервере
Процедура ОчередьСообщенийПродюсераНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	ОбщегоНазначения.СообщитьПользователю("Очередь сообщений продюсера: " + Коннектор.ОчередьСообщенийПродюсера());
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

#КонецОбласти

#Область Консюмер

&НаКлиенте
Процедура ПрочитатьСообщения(Команда)
	ПрочитатьСообщениеНаСервере();
КонецПроцедуры

&НаСервере
Процедура ПрочитатьСообщениеНаСервере()
	
	Коннектор = ВосстановитьКоннектор();	
	
	Если ПартицияСообщенияАвто Тогда	
		Коннектор.УстановитьПартицию(ПартицияСообщения);		
	КонецЕсли;
	
	// Сначала вычитываем все сообщения для замера времени
	ВремяНачала = ТекущаяУниверсальнаяДатаВМиллисекундах();
	
	МассивСообщений = Новый Массив;
	Сообщение = Коннектор.ПрочитатьСообщение();	
	Пока ЗначениеЗаполнено(Сообщение) Цикл		
		Если ЧитатьМетаданныеСообщений Тогда
			Сообщение = Сообщение + Символы.ПС + "Метаданные: " + Символы.ПС + Коннектор.ПрочитатьМетаданныеСообщения();	
		КонецЕсли;
		
		МассивСообщений.Добавить(Сообщение);
		Если МассивСообщений.Количество() >= КоличествоСообщенийКЧтению Тогда
			Прервать;	
		КонецЕсли;
		
		Сообщение = Коннектор.ПрочитатьСообщение();
	КонецЦикла;
	
	ВремяОкончания = ТекущаяУниверсальнаяДатаВМиллисекундах();
	
	ТекстСообщения = СтрШаблон("Загружено %1 сообщений за %2 секунд", 
	 	МассивСообщений.Количество(),
		(ВремяОкончания - ВремяНачала) / 1000);
	ОбщегоНазначения.СообщитьПользователю(ТекстСообщения);
	
	// Теперь выводим сообщения, если количество больше 100 - это явно нагрузочный тест, нет смысла выводить их
	Если МассивСообщений.Количество() <= 100 Тогда
		Для каждого Сообщение Из МассивСообщений Цикл
			ОбщегоНазначения.СообщитьПользователю(СтрШаблон("Сообщение: %1", Сообщение));
		КонецЦикла;	
	КонецЕсли;
		    
	СохранитьКоннектор(Коннектор);	
	
КонецПроцедуры

&НаКлиенте
Процедура Подписки(Команда)
	ПодпискиНаСервере();
КонецПроцедуры

&НаСервере
Процедура ПодпискиНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	ОбщегоНазначения.СообщитьПользователю(Коннектор.Подписки());
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

&НаКлиенте
Процедура Подписаться(Команда)
	ПодписатьсяНаСервере();
КонецПроцедуры

&НаСервере
Процедура ПодписатьсяНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	Коннектор.Подписаться();
	СохранитьКоннектор(Коннектор);	
	
КонецПроцедуры

&НаКлиенте
Процедура ПолучитьОффсет(Команда)
	ПолучитьОффсетНаСервере();
КонецПроцедуры

&НаСервере
Процедура ПолучитьОффсетНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	Оффсет = Коннектор.ПолучитьОффсет(ПартицияСообщения);
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

&НаКлиенте
Процедура ЗафиксироватьОффсет(Команда)
	ЗафиксироватьОффсетНаСервере();
КонецПроцедуры

&НаСервере
Процедура ЗафиксироватьОффсетНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	Коннектор.ЗафиксироватьОффсет(ПартицияСообщения, Оффсет);
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

&НаКлиенте
Процедура ОчередьСообщенийКонсюмера(Команда)
	ОчередьСообщенийКонсюмераНаСервере();
КонецПроцедуры

&НаСервере
Процедура ОчередьСообщенийКонсюмераНаСервере()
	
	Коннектор = ВосстановитьКоннектор();
	ОбщегоНазначения.СообщитьПользователю("Очередь сообщений консюмера: " + Коннектор.ОчередьСообщенийКонсюмера());
	СохранитьКоннектор(Коннектор);
	
КонецПроцедуры

#КонецОбласти

#КонецОбласти

#Область СлужебныеПроцедурыИФункции

&НаСервере
Процедура ОбновитьЭлементы()
	
	Элементы.ПартицияОтправка.Доступность = НЕ ПартицияСообщенияАвто;	
	Элементы.ПартицияПолучение.Доступность = НЕ ПартицияСообщенияАвто;	
	
КонецПроцедуры

&НаСервере
Процедура СохранитьКоннектор(Коннектор)
	
	Структура = Новый Структура("Коннектор", Коннектор);
	АдресКоннектора = ПоместитьВоВременноеХранилище(Структура, УникальныйИдентификатор);	
	
КонецПроцедуры

&НаСервере
Функция ВосстановитьКоннектор()
	
	Если НЕ ЗначениеЗаполнено(АдресКоннектора) Тогда
		ВызватьИсключение "Коннектор не был создан";		
	КонецЕсли;
	
	Структура = ПолучитьИзВременногоХранилища(АдресКоннектора);
	
	Если Структура = Неопределено Тогда
		ВызватьИсключение "Не удалось получить коннектор из временного хранилища";	
	КонецЕсли;
	
	Возврат Структура.Коннектор;
	
КонецФункции

#КонецОбласти
