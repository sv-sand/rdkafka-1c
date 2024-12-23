﻿
#Область ОбработчикиСобытийФормы

&НаСервере
Процедура ПриСозданииНаСервере(Отказ, СтандартнаяОбработка)
	
	ОбновитьЭлементыФормы();
	
КонецПроцедуры

#КонецОбласти

#Область ОбрботчикиСобытийЭлементовФормы

&НаКлиенте
Процедура ПротоколБезопасностиПриИзменении(Элемент)
	ОбновитьЭлементыФормы();
КонецПроцедуры

#КонецОбласти

#Область СлужебныеПроцедурыИФункции

&НаСервере
Процедура ОбновитьЭлементыФормы()

	Если Объект.ПротоколБезопасности = "plaintext" Тогда
		Элементы.ГруппаSASL.Видимость = Ложь;
		Элементы.ГруппаSSL.Видимость = Ложь;
		
	ИначеЕсли Объект.ПротоколБезопасности = "ssl" Тогда
	    Элементы.ГруппаSASL.Видимость = Ложь;
		Элементы.ГруппаSSL.Видимость = Истина;
		
	ИначеЕсли Объект.ПротоколБезопасности = "sasl_plaintext" Тогда
	    Элементы.ГруппаSASL.Видимость = Истина;
		Элементы.ГруппаSSL.Видимость = Ложь;
		
	ИначеЕсли Объект.ПротоколБезопасности = "sasl_ssl" Тогда
	    Элементы.ГруппаSASL.Видимость = Истина;
		Элементы.ГруппаSSL.Видимость = Истина;
		
	Иначе
		Элементы.ГруппаSASL.Видимость = Ложь;
		Элементы.ГруппаSSL.Видимость = Ложь;
		
	КонецЕсли;
	
КонецПроцедуры

#КонецОбласти
