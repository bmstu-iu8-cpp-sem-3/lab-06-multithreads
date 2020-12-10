# Лабораторная работа №6

## Задание

Использовать `std::thread` или `std::async`/`std::future` для параллельного вычисления значений хеш-функции [SHA-256](https://ru.wikipedia.org/wiki/SHA-2) определённого вида.

Для отображения значения хеш-функций часто используют запись в шестнадцатиричном (**HEX**) формате. Результатом работы функции **SHA-256** является массив из 32 байт (`32 * 8 = 256`), который может быть представлен в виде 64-x символьной **HEX**-строки:
```
SHA256("test") = "9f86d081884c7d659a2feaa0c55ad015a3bf4f1b2b0b822cd15d6c15b0f00a08";
SHA256("1234") = "03ac674216f3e15c761ee1a5e255f067953623c8b388b4459e13f978d7c846f4";
```

Особенностью современных криптографических хеш-функций является вычислительная сложность получения:
- коллизий (разных сообщений, для которых значение хеш-функции совпадает);
- прообразов (исходных сообщений из имеющегося значения хеш-функции).

На текущий момент самым эффективным решением данных задач является метод полного перебора.

#### Требуется:
1. Организовать перебор случайных входных данных (прообразов) для функции **SHA-256** с целью получения значения хеш-функции, удовлетворяющего следующему виду:
`XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0000`, где `X`- произвольный **HEX**-символ, а `0` - ноль, т.е. значение хеш-функции должно заканчиваться на `N=4`нулей, а остальные символы могут быть произвольными.
1. Вычисления проводить в `M` потоках, где `M` - максильное число одновременно выполняемых потоков. <br/>Задается аргументом командной строки. Если аргумент не задан, то использовать значение по умолчанию равное `std::thread::hardware_concurrency`.
1. Все случайные входные данные, значения хеш-функции, а так же идентификатор потока, который обрабатывает данные вывести в лог `Log` (консоль+файл); Уровень логирования для **trace**.
1. При обнаружении значения хеш-функции требуемого вида вывести его в `Log` и продолжить вычисления; <br/>Уровень логирования для данного события **info**.
1. Для логгирования использовать библиотеку `boost::log` (документацию с примерами можно найти [здесь](https://www.boost.org/doc/libs/1_63_0/libs/log/doc/html/log/tutorial.html#log.tutorial.trivial)).
1. Обеспечить ротацию лог-файлов по размеру.
1. Для формирования случайных входных данных использовать псевдослучайный генератор `std::rand`.
1. Корректно обработать завершение программы с помощью нажатия на клавиши **Ctrl+C**.
1. При завершении работы программы создать файл с данными в виде json-массива. Имя файла задается аргументом командной строки. При отсутствии аргумента, завершить программу. <br/>Json-массив хранит объекты следующего вида:
```js
{
  "timestamp": 1566777600, // время нахождение подходящего хеша
  "hash": "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0000", // hex-представление полученного хеша
  "data": "AABCDA32F" // hex-представление прообраза хеш-функции
}
```


## Рекомендации

Подключение библиотеки `boost::log` осуществить с помощью пакетного менеджера **Hunter** (см. [пример](https://docs.hunter.sh/en/latest/packages/pkg/Boost.html)).

Для вычислений значений хеш-функции использовать библиотеку [picosha2](https://github.com/okdshin/PicoSHA2).

Количество одновременно выполняемых потоков для конкретной аппаратной конфигурации может быть получено с помощью функции `std::thread::hardware_concurrency`.

Пример получения значения хеш-функции в **HEX**-виде из последовательности байт:
```cpp
#include <picosha2.h>

const std::vector<unsigned char> data_vector{...}; // вектор
const std::string hash = picosha2::hash256_hex_string(data_vector);

const std::array<unsigned char, 4> data_array{...}; // массив
const std::string hash = picosha2::hash256_hex_string(data_array);

const unsigned char data_c_array[N] = {...}; // c-массив
const std::string hash = picosha2::hash256_hex_string(data_c_array, data_c_array + N);
```

Пример тривиального логирования с помощью библиотеки `boost::log`:
```cpp
#include <boost/log/trivial.hpp>

int main(int, char*[])
{
    BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
    BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
    BOOST_LOG_TRIVIAL(info) << "An informational severity message";
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    BOOST_LOG_TRIVIAL(error) << "An error severity message";
    BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";
}
```

Пример настройки логгирования в файл:
```cpp
boost::shared_ptr< logging::core > core = logging::core::get();

boost::shared_ptr< sinks::text_file_backend > backend =
    boost::make_shared< sinks::text_file_backend >(
        keywords::file_name = "file_%5N.log",
        keywords::rotation_size = 5 * 1024 * 1024,
        keywords::format = "[%TimeStamp%]: %Message%",
        keywords::time_based_rotation =
            sinks::file::rotation_at_time_point(12, 0, 0));

typedef sinks::synchronous_sink< sinks::text_file_backend > sink_t;
boost::shared_ptr< sink_t > sink(new sink_t(backend));
// sink ->set_filter(logging::trivial::severity >= logging::trivial::info);
core->add_sink(sink);
```

Для работы с Json используйте знакомые вам библиотеки. Стоит сделать один json-массив, доступ к которому предоставить из всех запущенных потоков. Чтобы избежать гонки за данными, синхронизируйте доступ до этого объекта.

Итоговый json должен выглядеть следующим образом (`X`- произвольный **HEX**-символ):
```js
[
    {
        "timestamp": 1566777600,
        "hash": "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0000",
        "data": "XXXXXXXXXXXXXXX"
    },
    {
        "timestamp": 1566777640,
        "hash": "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0000",
        "data": "XXXXXXXXX"
    },
    {
        "timestamp": 1566776621,
        "hash": "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX0000",
        "data": "XXXXXXXXX"
    },
    // ...
]
```
