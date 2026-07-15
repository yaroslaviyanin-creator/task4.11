/*
string_list.h - логика списка со строками, поддерживающего разные варианты связей.
Янин Ярослав Иванович
Группа МК-101
*/

#ifndef _STRING_LIST_H_
#define _STRING_LIST_H_

#include "mm.h"
#include "list.h"

typedef struct _StringListEntry {
    char* value;
    ListEntry initOrderEntry;
    ListEntry reverseOrderEntry;
    ListEntry sortByLen;
    ListEntry reverseSortByLen;
    ListEntry quickSort;
    ListEntry reverseQuickSort;
} StringListEntry;

// Функция генерирует случайную строку из печатных символов
// <s> - буфер для записи строки
// <len> - максимальная длина строки
char* string_list_rnd_str(char* s, int len);

// Функция создает новый элемент списка или голову нового списка
// <str> - строка для копирования в элемент (или NULL для головы)
StringListEntry* string_list_new(char* str);

// Функция инициализирует все внутренние связи элемента списка
// <entry> - указатель на элемент списка
StringListEntry* string_list_init(StringListEntry* entry);

// Функция создает новый элемент и добавляет его в конец списка
// <list> - двойной указатель на голову списка
// <str> - строка для добавления
StringListEntry* string_list_add(StringListEntry** list, char* str);

// Функция освобождает память, занятую элементом списка и его строкой
// <entry> - удаляемый элемент
void string_list_free(StringListEntry* entry);

// Функция удаляет элемент из всех списков с освобождением памяти
// <entry> - удаляемый элемент
void string_list_remove(StringListEntry* entry);

// Функция возвращает размер списка
// <list> - голова списка
int string_list_count(StringListEntry* list);

// Функция очищает список, удаляя все элементы кроме головы, возвращает голову
// <list> - голова списка
StringListEntry* string_list_clear(StringListEntry* list);

// Функция упорядочивает список reverseOrderEntry (инвертирование на месте)
// <list> - голова списка
void string_list_make_reverse_order(StringListEntry* list);

// Функция упорядочивает список sortByLen пузырьком
// <list> - голова списка
void string_list_make_sort_by_len(StringListEntry* list);

// Функция упорядочивает список reverseSortByLen (инвертирование на основе sortByLen)
// <list> - голова списка
void string_list_make_reverse_sort_by_len(StringListEntry* list);

// Функция упорядочивает список quickSort
// <list> - голова списка
void string_list_make_quick_sort(StringListEntry* list);

// Функция упорядочивает список reverseQuickSort
// <list> - голова списка
void string_list_make_reverse_quick_sort(StringListEntry* list);

// Функция выводит элементы списка в порядке инициализации
// <list> - голова списка
int string_list_init_order_print(StringListEntry* list);

// Функция выводит элементы списка в обратном порядке инициализации
// <list> - голова списка
int string_list_reverse_order_print(StringListEntry* list);

// Функция выводит элементы списка по длине
// <list> - голова списка
int string_list_sort_by_len_print(StringListEntry* list);

// Функция выводит элементы списка в обратном порядке по длине
// <list> - голова списка
int string_list_reverse_sort_by_len_print(StringListEntry* list);

// Функция выводит элементы списка после быстрой сортировки
// <list> - голова списка
int string_list_quick_sort_print(StringListEntry* list);

// Функция выводит элементы списка в обратном порядке после быстрой сортировки
// <list> - голова списка
int string_list_reverse_quick_sort_print(StringListEntry* list);

#endif  // _STRING_LIST_H_