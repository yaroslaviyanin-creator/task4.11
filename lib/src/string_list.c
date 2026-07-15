/*
string_list.c - логика списка со строками, поддерживающего разные варианты связей.
Янин Ярослав Иванович
Группа МК-101
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "string_list.h"

// Функция генерирует случайную строку из заглавных латинских букв
// <s> - буфер для записи сгенерированной строки
// <len> - максимальная длина строки
char* string_list_rnd_str(char* s, int len) {
    int i, l;
    char* c;
    l = rand() % (len - 1);
    for (c = s, i = 0; i < l; i++, c++) {
        *c = 65 + (rand() % 26); // только заглавные буквы, чтобы визуально было проще проверять
    }
    *c = 0; // нуль-терминатор в конце строки
    return s;
}

// Функция инициализирует все внутренние связи элемента списка
// <entry> - указатель на элемент списка
StringListEntry* string_list_init(StringListEntry* entry) {
    // Для каждого из 6 внедренных списков вызываем list_init, которая сделает их "кольцевыми"
    list_init(&entry->initOrderEntry);
    list_init(&entry->reverseOrderEntry);
    list_init(&entry->sortByLen);
    list_init(&entry->reverseSortByLen);
    list_init(&entry->quickSort);
    list_init(&entry->reverseQuickSort);
    return entry;
}

// Функция создает новый элемент списка или голову нового списка
// <str> - строка для копирования в элемент (или NULL для создания головы)
StringListEntry* string_list_new(char* str) {
    // Запрашиваем память под "коробку" элемента у нашего кастомного менеджера памяти
    StringListEntry* entry = mm_alloc();
    if (entry == NULL) {
        fprintf(stderr, "StringList Error: Cannot allocate memory for entry\n");
        return NULL;
    }

    // Если передали NULL, значит мы создаем пустую "голову" списка
    if (str == NULL) {
        entry->value = NULL;
    }
    else {
        // Выделяем стандартную память под сам текст (длина строки + 1 байт на нуль-терминатор)
        entry->value = (char*)malloc(strlen(str) + 1);
        if (entry->value == NULL) {
            fprintf(stderr, "StringList Error: Cannot allocate memory for string\n");
            mm_free(entry);
            return NULL;
        }
        strcpy(entry->value, str);
    }

    return string_list_init(entry);
}

// Функция создает новый элемент и добавляет его в конец списка
// <list> - двойной указатель на голову списка
// <str> - строка для добавления
StringListEntry* string_list_add(StringListEntry** list, char* str) {
    StringListEntry* new_entry = string_list_new(str);
    if (new_entry == NULL) return NULL;

    // Вставляем новый элемент сразу в КОНЕЦ всех 6 списков.
    // Конструкция (&(*list) -> ... ) -> prev означает элемент, стоящий ПЕРЕД головой, то есть конец кольца
    list_add((&(*list)->initOrderEntry)->prev, &new_entry->initOrderEntry);
    list_add((&(*list)->reverseOrderEntry)->prev, &new_entry->reverseOrderEntry);
    list_add((&(*list)->sortByLen)->prev, &new_entry->sortByLen);
    list_add((&(*list)->reverseSortByLen)->prev, &new_entry->reverseSortByLen);
    list_add((&(*list)->quickSort)->prev, &new_entry->quickSort);
    list_add((&(*list)->reverseQuickSort)->prev, &new_entry->reverseQuickSort);

    return new_entry;
}

// Функция освобождает память, занятую элементом списка и его строкой
// <entry> - удаляемый элемент
void string_list_free(StringListEntry* entry) {
    if (entry->value != NULL) {
        free(entry->value);
    }
    mm_free(entry);
}

// Функция удаляет элемент из всех списков с освобождением памяти
// <entry> - удаляемый элемент
void string_list_remove(StringListEntry* entry) {
    list_remove(&entry->initOrderEntry);
    list_remove(&entry->reverseOrderEntry);
    list_remove(&entry->sortByLen);
    list_remove(&entry->reverseSortByLen);
    list_remove(&entry->quickSort);
    list_remove(&entry->reverseQuickSort);
    string_list_free(entry);
}

// Функция возвращает размер списка
// <list> - голова списка
int string_list_count(StringListEntry* list) {
    ListEntry* head, * entry;
    int n = 0;
    head = &list->initOrderEntry;
    entry = list_next(head);
    while (entry != head) {
        entry = list_next(entry);
        n++;
    }
    return n;
}

// Функция очищает список, удаляя все элементы кроме головы, возвращает голову
// <list> - голова списка
StringListEntry* string_list_clear(StringListEntry* list) {
    StringListEntry* string_entry;
    ListEntry* head, * entry;
    head = &list->initOrderEntry;
    entry = list_next(head);

    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, initOrderEntry, entry);
        entry = list_next(entry);
        string_list_free(string_entry);
    }
    return string_list_init(list);
}