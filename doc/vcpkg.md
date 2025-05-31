# Менеджер пакетов vcpkg

Для сборки компоненты понадобятся дополнительные библиотеки, которые можно загрузить и использовать через менеджер пакетов С++ vcpkg. Вы можете [скачать](https://github.com/microsoft/vcpkg) его самостоятельно в свой каталог.

## Установка

1. Клонируем репозиторий 

```sh
git clone https://github.com/microsoft/vcpkg.git
```

2. Запускаем настройку среды 

```sh
bootstrap-vcpkg.bat
```

3. Добавляем переменную окружения:
 
```sh
VCPKG_ROOT=D:\tools\vcpkg
```

4. Добавляем в переменную PATH каталог vcpkg: 
   
```sh
D:\tools\vcpkg
```

5. Интегрируем в MS Visual Studio (для windows):

```sh
vcpkg integrate install
```

## Установка зависимостей:

Для установки зависимостей нужно запустить скрипт `vcpkg-install`. Зависимости будут установлены в каталог `vcpkg_installed`.
