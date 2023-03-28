# Thread pool

## Definition

Пул потоків - це сукупність робочих потоків, які створюються і підтримуються програмою, зазвичай для одночасного
виконання великої кількості короткотривалих завдань. Замість того, щоб створювати новий потік для кожного завдання,
програма надсилає завдання до пулу потоків, який потім призначає його доступному робочому потоку. Це дозволяє уникнути
накладних витрат на створення та знищення потоків для кожної задачі, а також підвищити продуктивність та масштабованість
програми.

Основна ідея пулу потоків полягає в тому, щоб мати фіксовану кількість робочих потоків, які створюються під час запуску
програми, а потім повторно використовуються для обробки черги завдань. Коли завдання надходить до пулу, воно додається
до черги завдань, і робочому потоку призначається обробка наступного доступного завдання в черзі. Якщо всі робочі потоки
в даний момент зайняті, завдання утримується в черзі до тих пір, поки не звільниться робочий потік.

Пули потоків зазвичай використовуються у серверних програмах та інших програмах, яким потрібно обробляти велику
кількість одночасних запитів або завдань. Вони також можуть бути використані для розпаралелювання інтенсивних робочих
навантажень на декількох ядрах або процесорах. Використовуючи пул потоків, програма може скористатися можливостями
паралельної обробки сучасного обладнання, при цьому керуючи кількістю потоків і уникаючи накладних витрат на створення і
знищення потоків.

## Implementation

Клас `StaticThreadPool` ви можете побачити в папці `include`.

Для створення Thread Pool нам знадобиться клас `ThreadSafeQueue`.

* Головна функція, яка нам буде потрібна - це `bool TryDeque(T& value)`, де T - це якись шаблоний тип данних.

    ```C++
    bool TryDeque(T& val)
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_buffer.empty())
            {
                return false;
            }
            val = std::move(m_buffer.back());
            m_buffer.pop_back();
            return true;
        }
    ```

  Ідея цієї функції дуже проста, ми намагаємось взяти елемент з нашої черги, якщо його там немає то ми просто повертаємо
  `false`.

Також нам буде потрібний клас `JoinThreads`.

* Задача цього класу дуже проста, коли викликатися конструктор класу `JoinThreads`
  ми додаємо наш потік до вектора, а потім при виклику деструктора всі наші потоки джойняться. По факту цей клас
  потрібний
  тільки для того, щоб не навантажувати код.

```C++
explicit JoinThreads(std::vector<std::thread>& t_threads)
    :
m_threads(t_threads)
{}

~JoinThreads()
{
    for (auto &thread : m_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}
```

В загалом наш клас `StaticThreadPool` має такі приватні поля:

```C++
std::atomic_bool m_done;
ThreadSafeQueue<FunctionWrapper> m_work_queue;
std::vector<std::thread> m_threads;
JoinThreads m_joiner;
```

> **Note**
>
> Зверніть увагу, що порядок оголошення членів є важливим:
> і прапорець `m_done`, і `m_worker_queue` мають бути оголошені перед вектором `m_threads`,
> який, у свою чергу, має бути оголошений перед `m_joiner`. Це гарантує, що члени будуть знищені у правильному порядку.
> Ви не можете безпечно знищити чергу, доки всі потоки не зупиняться.

Тепер перейдемо до того як працює наш `thread pool`.

1. В нас є проста функція `void WorkerThread()`. Її задача доволі примітивна. Все, що вона робить - це сидить у циклі і
   чекає поки наш прапорець `m_done` не буде встановлений в `true`. Поки цього не має, вона намагається взяти таску з
   нашої
   черги та виконати її. Якщо ж таски немає, то функція викликає `std::thread::yield()`.

   > **Note**
   >
   > Посилаючись на <a href="https://en.cppreference.com/w/cpp/thread/yield">CPP reference</a>. Все що робить
   > функція `yield()` - це перенесення виконання потоків на інший час, щоб дозволити іншим потокам працювати.

   Цю функцію ми закидуємо в наш потік під час ініціалізації класу, якось так:

    ```C++
   StaticThreadPool()
            :
    m_done(false), m_joiner(m_threads)
    {
        const uint16_t threadCount = std::thread::hardware_concurrency();
        try
        {
            for (uint16_t i = 0; i < threadCount; ++i)
            {
                m_threads.emplace_back(&StaticThreadPool::WorkerThread, this);
            }
        }
        catch (...)
        {
            m_done = true;
            throw;
        }
    }
    ```

   Сама ж функція виглядає так:

    ```C++
    void WorkerThread()
    {
        while(!m_done)
        {
            FunctionWrapper task;
            if (m_work_queue.TryDeque(task))
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }
    ```

2. В нас також є функція, яка відправляє наші таски в `thread pool`. Це функція `Submit()`. Вона вже є трошки
   складнішою
   в імплементації. Її задача, як вже згадувалась вище - це відправлення таски в наш `thread pool` та
   повертати `std::
   future<ResultType>`, де `ResultType` - це шаблоний результат, який повертає наша функція.
    ```C++
    template<typename FunctionType>
    std::future<typename  std::result_of<FunctionType()>::type>
    Submit(FunctionType function)
    {
        typedef typename std::result_of<FunctionType()>::type resultType;
        std::packaged_task<resultType()> task(std::move(function));
        std::future<resultType> result(task.get_future());
        m_work_queue.Enque(std::move(task));
        return result;
    }
    ```
   > **Note**
   >
   >   Я думаю є сенс пояснити, що тут відбувається. Для початку треба розібратись, що повертає наша функція.
   > Посилаючись на <a href="https://en.cppreference.com/w/cpp/thread/future">CPP reference</a>, `std::future` надає
   > механізм доступу до результату асинхронної роботи
   >   * Творець асинхронної операції може використовувати різноманітні методи для запиту, очікування або вилучення
         значення з `std::future`.
   >   * Коли асинхронна операція буде готова надіслати результат творцеві, вона може зробити це, змінивши спільний
         стан (
         наприклад, `std::promise::set_value`), який пов'язано з std::future творця.
   >
   >   **Як в нас з'являється std::future?**
   >   1. Обгортання `std::packaged_task`: <br>
          Ми робимо це для того, щоб значення, що повертається, або виключення, що
          генерується, зберігалось у спільному стані, до якого можна отримати доступ через об'єкти `std::future`.
   >   2. Отримання об'єкта `std::future`: <br>
          Ми це робимо доволі легко завдяки методу `get_future()`<br>
   >
   >   Тепер наша задача запхати таску в `ThreadSafeQueue`. Оскільки `std::packaged_task` в нас `moveable` та
   не `copyable`
   > треба обгорнути нашу таску в кастомний клас обгортку, `FunctionWrapper`, який зробить нашу таску
   тільки `moveable`.<br>
   >
   > Я не буду пояснювати як імплементований клас `FunctionWrapper`, головне, що треба зрощуміти, що він робить нашу
   таску *only*
   > `moveable`.

Це все що є в нашому класі `StaticThreadPool`.

## How it works
Припустимо, що в нас є така проста
функція `void printNum(int number)`, задача якої вивести отримане число у консоль. Спробуємо це зробити декілька разів з
використанням `thread pool`.

```C++
int main()
{
    StaticThreadPool staticThreadPool;
    std::vector<std::future<void>> futures;

    for (size_t i = 0; i < 8; ++i)
    {
        futures.emplace_back(staticThreadPool.Submit([capture = static_cast<int>(i)] ()
        {
            printNum(capture);
        }));
    }

    for (auto& result : futures)
    {
        result.get();
    }
    return 0;
}
```

Послідовно розберемо як це працює:
* Створюємо екземпляр класу `StaticThreadPool` та вектор `std::futures<void>`
* Відправляємо наші таски в `thread pool`
* Забираємо результат наших тасок з використанням методу `get()`

Як ми бачимо програма доволі примітивна.

## Results
```textmate
Number: 0
Number: 2
Number: Number: 5
Number: 6
Number: 7
Number: 3
4
Number: 1
```

> **Note**
>
> Зауважте, що вивід в консоль має такий вид, адже потік виводу С++ синхронний.