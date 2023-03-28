# Thread Safe Map

## Definition

Потокобезпечна карта -- це структура даних, яка дозволяє декільком потокам одночасно отримувати доступ до карти та
модифікувати її без будь-якої неузгодженості даних або умов гонки. Вона призначена для запобігання проблемам, які можуть
виникнути при одночасному доступі декількох потоків до не-безпечної для потоків карти.

Якщо два потоки намагаються одночасно змінити одну і ту ж пару ключ-значення, результат непередбачуваний і може
призвести до пошкодження даних. З іншого боку, безпечна для потоків карта гарантує, що тільки один потік може отримати
доступ до певної пари ключ-значення в будь-який момент часу.

## Implementation

Клас `ThreadSafeMap` ви можете побачити в папці `include`.

Для реалізації `ThreadSafeMap` нам буде потрібен `std::unordered_map` та `shared_mutex`.

> **Note**
>
> Різниця між `std::mutex` та  `std::shared_mutex` гарна описана
> в <a href="https://en.cppreference.com/w/cpp/thread/shared_mutex">CPP reference</a>. Якщо коротко, то
> `std::shared_mutex` дає змогу одночасно багатьом потокам читати одні й ті самі дані, якщо в цей момент немає потоків,
> які
> змінюють ці данні.

Наш клас буде темплейтним, адже він повинен мати змогу зберігати будь-які типи даних, та мати наступні методи:

```C++
   void Insert(const Key& key, const Value& value);
void Remove(const Key& key);
bool Find(const Key& key, Value& value) const;
Value& operator[](const Key& key);
size_t Size() const;
bool Empty();
```

Для початку розберемо наступні функції:

* `bool Find(const Key& key, Value& value) const`
* `size_t Size() const`
* `bool Empty()`

Я їх обрав не просто так, адже вони мають одну спільну рису, якщо ви подивитись на них, то помітите, що всі вона мають
один спільний рядок коду:

```C++
 std::shared_lock<std::shared_mutex> lock(m_mutex);
```

Ці функції потребують саме такої реалізації, адже `std::shared_lock` дозволяє декільком читачам утримувати спільні
блокування м'ютексу при цьому гарантуючи, що лише один потік модифікує мапу одночасно. Більш детальне пояснення можна
знайти на  <a href="https://en.cppreference.com/w/cpp/thread/shared_lock">CPP reference</a>.

### Реалізація цих функцій

1. ```C++
   bool Find(const Key& key, Value& value) const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end())
        {
            value = it->second;
            return true;
        }
        return false;
    }
    ```
   Функція захватує м'ютекс, шукає елемент в `std::unordered_map`, якщо такий існує, то присвоює його посилальному
   аргументу функції та повертає `true`. В інакшому -- `false`.

2. ```C++
    size_t Size() const
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_map.size();
    }
    ```
   Функція захватує м'ютекс для того, щоб інший потік не зміг додати елемент в `std::unordered_map` та повертає
   розмір `std::unordered_map`.

3. ```C++
    bool Empty()
    {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_map.empty();
    }
   ```
   Функція захватує м'ютекс для того, щоб інший потік не зміг додати елемент в `std::unordered_map`, перевіряє чи
   дорівнює розмір `std::unordered_map`, якщо так, то повертає `true` інакше -- `false`.

Тепер розглянемо функції які залишилися:

* `void Insert(const Key& key, const Value& value)`;
* `void Remove(const Key& key)`;
* ` Value& operator[](const Key& key)`;

Якщо ми подивимось, то вони так само мають один спільний рядок коду:

```C++
 std::unique_lock<std::shared_mutex> lock(m_mutex);
```

Ці функції потребують саме такої реалізації, адже `std::unique_lock`, адже завдяки йому тільки один потік може змінювати
дані у `std::unordered_map`. Більше інформації ви можете знайти
на  <a href="https://en.cppreference.com/w/cpp/thread/unique_lock">CPP reference</a>.

### Реалізація цих функцій

1. ```C++
   void Insert(const Key& key, const Value& value)
   {
      std::unique_lock<std::shared_mutex> lock(m_mutex);
      m_map.insert(std::make_pair(key, value));
   }
   ```
   Функція захватує м'ютекс для того, щоб інші не могли додати дані до `std::unordered_map` та додає елемент.

2. ```C++
   void Remove(const Key& key)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_map.erase(key);
    }
   ```
   Функція захватує м'ютекс для того, щоб інші не могли видалити дані з `std::unordered_map` та видаляє елемент.

3. ```C++
   Value& operator[](const Key& key)
    {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        return m_map[key];
    }
   ```
   Функція захватує м'ютекс для того, щоб інші не могли видалити дані з `std::unordered_map` та модифікує елемент або
   просто якось використовує цей елемент.

## How it works
Насправді там не має, що пояснювати, адже здається все має бути зрозуміло.

## Results

```textmate
   Size of map: 3
   Bob`s age is not found
   Map is not empty
   Get Alex age using operator []: 100
```

Результати -- очікуванні. 