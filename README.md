# Transport Catalogue

C++ бэкенд для системы сбора, обработки и визуализации данных об общественном транспорте.

## Функциональность

- **Хранение данных**: Эффективное хранение информации об остановках и маршрутах общественного транспорта
- **Статистика**: Получение детальной статистики по маршрутам и остановкам
- **Визуализация**: Генерация SVG-карт маршрутов с настраиваемым дизайном
- **Маршрутизация**: Поиск оптимальных маршрутов между остановками с учётом времени ожидания и движения транспорта
- **JSON API**: Полноценный JSON-интерфейс для ввода данных и получения результатов

## Технологии

- **Язык**: C++17 (modern features: std::string_view, std::optional, structured bindings)
- **Архитектура**: Модульная design с разделением ответственности
- **Сериализация**: Собственная реализация JSON парсера и билдера
- **Алгоритмы**: Графовые алгоритмы для маршрутизации (Дейкстры/Флойда-Уоршелла)
- **Визуализация**: Генерация SVG через собственный движок
- **Система сборки**: CMake

## Структура проекта

```
cpp-transport-catalogue/
├── CMakeLists.txt
├── README.md
├── opentest/                    # Тестовые данные
│   ├── s12_final_opentest_1.json
│   ├── s12_final_opentest_1_answer.json
│   ├── s12_final_opentest_2.json
│   ├── s12_final_opentest_2_answer.json
│   ├── s12_final_opentest_3.json
│   ├── s12_final_opentest_3_answer.json
│   └── test.json
└── transport-catalogue/
    ├── domain.h
    ├── domain.cpp
    ├── geo.h
    ├── geo.cpp
    ├── graph.h
    ├── json.h
    ├── json.cpp
    ├── json_builder.h
    ├── json_builder.cpp
    ├── json_reader.h
    ├── json_reader.cpp
    ├── main.cpp
    ├── map_renderer.h
    ├── map_renderer.cpp
    ├── ranges.h
    ├── request_handler.h
    ├── request_handler.cpp
    ├── router.h
    ├── svg.h
    ├── svg.cpp
    ├── transport_catalogue.h
    ├── transport_catalogue.cpp
    ├── transport_router.h
    └── transport_router.cpp
```

## Сборка и запуск

### Требования
- Компилятор C++17 (GCC 7+, Clang 5+ или MSVC 2017+)
- CMake 3.2+

### Инструкция по сборке

```bash
# Клонирование репозитория
git clone <repository-url>
cd transport-catalogue

# Создание директории для сборки
mkdir build && cd build

# Генерация проектных файлов
cmake .. -DCMAKE_BUILD_TYPE=Release

# Сборка проекта
cmake --build .

# Запуск приложения (разные варианты)
./myExe # Чтение из stdin, вывод в stdout
./myExe input.json # Чтение из файла, вывод в stdout
./myExe input.json output.json # Чтение из файла, JSON вывод в файл
./myExe input.json output.json map.svg # Чтение из файла, JSON вывод в файл, SVG в файл
./myExe input.json - map.svg # Чтение из файла, JSON вывод в stdout, SVG в файл
```

### Тестирование

В папке `opentest` содержатся тестовые данные:
```bash
# Пример запуска на тестовых данных
./myExe opentest/s12_final_opentest_1.json output.json map.svg
```

## Формат данных

### Входные данные (JSON)

Программа принимает JSON-объект со следующими полями:
- `base_requests` - массив запросов на добавление остановок и маршрутов
- `stat_requests` - массив запросов на получение информации
- `render_settings` - настройки визуализации карты
- `routing_settings` - настройки маршрутизации

### Пример входного файла
```json
{
  "base_requests": [
    {
      "type": "Stop",
      "name": "Улица Докукина",
      "latitude": 55.595884,
      "longitude": 37.601808,
      "road_distances": {
        "Улица Старокачаловская": 1000
      }
    },
    {
      "type": "Bus",
      "name": "829",
      "stops": [
        "Улица Докукина",
        "Улица Старокачаловская"
      ],
      "is_roundtrip": true
    }
  ],
  "stat_requests": [
    {
      "type": "Bus",
      "name": "829",
      "id": 1
    }
  ]
}
```

### Выходные данные (JSON)

Программа возвращает массив ответов на stat_requests:

```json
[
  {
    "curvature": 1.355,
    "request_id": 1,
    "route_length": 2760,
    "stop_count": 4,
    "unique_stop_count": 3
  }
]
```

## Примеры использования

1. **Получение информации о маршруте**:
   ```json
   {"type": "Bus", "name": "14", "id": 1}
   ```

2. **Получение информации об остановке**:
   ```json
   {"type": "Stop", "name": "Улица Докукина", "id": 2}
   ```

3. **Визуализация карты**:
   ```json
   {"type": "Map", "id": 3}
   ```

4. **Построение маршрута**:
   ```json
   {"type": "Route", "from": "Улица Докукина", "to": "Улица Старокачаловская", "id": 4}
   ```

## Ценность проекта

- Демонстрирует современные практики C++ разработки
- Показывает умение работать с сложными алгоритмами и структурами данных
- Реализует полный цикл обработки данных от ввода до визуализации
- Модульная архитектура позволяет легко расширять функциональность
- Не зависит от внешних библиотек, весь функционал реализован самостоятельно