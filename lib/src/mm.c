/*
mm.c - менеджер памяти.
Янин Ярослав Иванович
Группа МК-101
*/

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include "mm.h"

MemoryManager* _mm = NULL; // Глобальный указатель на наш менеджер памяти

// Функция инициализирует менеджер памяти
// <size> - размер одного элемента в байтах
// <count> - количество элементов в блоке
// <callback> - указатель на функцию перерасчета адресов при реаллокации
int mm_initialize(int size, int count, void(*callback)(int ofs)) {
    if (_mm != NULL) free(_mm); // Если менеджер уже был создан, удаляем старый

    // Выделяем память под саму структуру MemoryManager
    _mm = (MemoryManager*)malloc(sizeof(MemoryManager));
    if (_mm == NULL) {
        fprintf(stderr, "MM Error: Cannot allocate memory for manager\n");
        return -1;
    }

    _mm->size = size; // Запоминаем размер одного элемента

#ifdef MM_USE
    _mm->callback = callback; // Сохраняем коллбэк для пересчета указателей
    _mm->delta = count;       // На сколько будем увеличивать массив, если память закончится
    _mm->count = count;       // Сколько элементов вмещает текущий блок

    // Выделяем один большой сплошной кусок памяти (массив байтов)
    _mm->m = calloc(_mm->size, _mm->count);
    if (_mm->m == NULL) {
        fprintf(stderr, "MM Error: Cannot allocate primary block\n");
        free(_mm);    // Удаляем саму структуру менеджера, если не хватило памяти
        _mm = NULL;
        return -1;
    }
    _mm->border_ofs = 0;  // Смещение свободной памяти равно 0 (всё свободно)
    _mm->empty = NULL;    // Стек свободных кусочков изначально пуст
#endif

    return 0;
}

// Функция очищает память, использующуюся менеджером
void mm_finalize(void) {
#ifdef MM_USE
    if (_mm != NULL) free(_mm->m); // Сначала освобождаем большой массив
#endif
    if (_mm != NULL) free(_mm);           // Затем удаляем саму структуру менеджера
    _mm = NULL;
}

// Функция выделяет память под новый элемент нужного размера
void* mm_alloc(void) {
#ifdef MM_USE
    void* p;

    // 1. Проверяем, есть ли ранее удаленные элементы в стеке
    if (_mm->empty != NULL) {
        p = _mm->empty;
        _mm->empty = _mm->empty->next; // Сдвигаем вершину стека
        return p;
    }

    // 2. Проверяем, есть ли место в нашем большом массиве
    if (_mm->border_ofs < _mm->size * _mm->count) {
        p = (char*)_mm->m + _mm->border_ofs;
        _mm->border_ofs += _mm->size;
        return p;
    }

    // 3. Место кончилось! Пробуем выделить новый массив большего размера
    if (_mm->callback != NULL) {
        p = calloc(_mm->size, _mm->count + _mm->delta);
        if (p == NULL) {
            fprintf(stderr, "MM Error: Cannot allocate memory for expansion\n");
            _mm->callback(0); // вызываем коллбэк с признаком ошибки
            return NULL;
        }

        // Вычисляем смещение (разницу адресов) и вызываем коллбэк для пересчета адресов
        _mm->callback((char*)p - (char*)_mm->m);

        // Копируем старые данные в новый массив и удаляем старый
        memcpy(p, _mm->m, _mm->size * _mm->count);
        free(_mm->m);

        _mm->m = p;
        _mm->count += _mm->delta;

        // Теперь место есть, выделяем элемент с новой границы
        p = (char*)_mm->m + _mm->border_ofs;
        _mm->border_ofs += _mm->size;
        return p;
    }

    fprintf(stderr, "MM Error: Memory exhausted and no callback provided\n");
    return NULL;
#else
    return malloc(_mm->size);
#endif
}

// Функция освобождает память из-под элемента (возвращает в стек менеджера)
// <entry> - указатель на освобождаемый блок памяти
void mm_free(void* entry) {
#ifdef MM_USE
    // Приводим указатель к структуре стека и кладем на вершину
    ((MMEmptyStack*)entry)->next = _mm->empty;
    _mm->empty = entry;
#else
    free(entry);
#endif
}