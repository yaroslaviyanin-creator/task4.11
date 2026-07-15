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

// Функция упорядочивает список reverseOrderEntry (инвертирование на месте)
// <list> - голова списка
void string_list_make_reverse_order(StringListEntry* list) {
    ListEntry* head, * forward, * backward, * tmp;
    head = &list->reverseOrderEntry;
    forward = list_next(head);
    backward = list_prev(head);

    // Идем с двух концов к центру
    while (forward != backward && forward != head && backward != head) {
        list_swap(forward, backward);
        if (list_prev(forward) == backward) break; // если элементы были соседними

        tmp = forward;
        // Указатели перекрестились из-за перестановки, поэтому шагаем "наоборот"
        forward = list_next(backward);
        backward = list_prev(tmp);
    }
}

// Функция упорядочивает список sortByLen методом пузырька
// <list> - голова списка
void string_list_make_sort_by_len(StringListEntry* list) {
    StringListEntry* string_cur, * string_next;
    ListEntry* head, * entry_cur, * entry_next;
    int swapped = 1; // Флаг замен для алгоритма

    head = &list->sortByLen;
    while (swapped) {
        swapped = 0;
        entry_cur = list_next(head);
        // Идем до тех пор, пока следующий элемент не окажется головой
        while ((entry_next = list_next(entry_cur)) != head) {
            string_cur = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry_cur);
            string_next = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry_next);

            // Если длина текущего текста больше следующего, меняем их местами
            if (strlen(string_cur->value) > strlen(string_next->value)) {
                list_swap(entry_cur, entry_next);
                swapped = 1;
            }
            else {
                entry_cur = list_next(entry_cur);
            }
        }
    }
}

// Функция упорядочивает список reverseSortByLen (инвертирование на основе sortByLen)
// <list> - голова списка
void string_list_make_reverse_sort_by_len(StringListEntry* list) {
    StringListEntry* string_entry;
    ListEntry* dir_head, * rev_head, * entry;

    dir_head = &list->sortByLen; // Голова уже отсортированного списка
    rev_head = &list->reverseSortByLen; // Голова реверсивного списка
    entry = list_next(dir_head);

    while (entry != dir_head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry);
        // Вырезаем узел из реверсивного списка и вставляем его строго после головы.
        // Поскольку каждый следующий элемент встает перед предыдущим, порядок переворачивается.
        list_add(rev_head, list_remove(&string_entry->reverseSortByLen));
        entry = list_next(entry);
    }
}

// Вспомогательная функция для быстрой сортировки (left не включается, right включается)
// <left> - левая граница диапазона
// <right> - правая граница диапазона
void string_list_make_rqsort(ListEntry* left, ListEntry* right) {
    StringListEntry* string_base, * string_entry;
    ListEntry* base, * entry, * next;
    int is_last;

    base = list_next(left);     // Опорным выбираем первый элемент из диапазона
    if (base == right) return;  // Если диапазон из 1 элемента, выходим

    string_base = GET_CONTENT_RECORD(StringListEntry, quickSort, base);
    entry = list_next(base);

    do {
        is_last = (entry == right);
        string_entry = GET_CONTENT_RECORD(StringListEntry, quickSort, entry);

        // Сравниваем строки лексикографически (по алфавиту)
        if (strcmp(string_entry->value, string_base->value) < 0) {
            next = list_next(entry);
            // Вырезаем элемент и вставляем его СЛЕВА от опорного
            list_add(left, list_remove(entry));
            entry = next;
            if (is_last) right = list_prev(entry);
        }
        else {
            entry = list_next(entry);
        }
    } while (!is_last);

    // Рекурсивно сортируем левую и правую части
    string_list_make_rqsort(left, base);
    if (base != right) string_list_make_rqsort(base, right);
}

// Функция упорядочивает список quickSort
// <list> - голова списка
void string_list_make_quick_sort(StringListEntry* list) {
    ListEntry* head, * last;
    head = &list->quickSort;
    last = list_prev(head);
    if (last != head) string_list_make_rqsort(head, last);
}

// Функция упорядочивает список reverseQuickSort (инвертирование на основе QuickSort)
// <list> - голова списка
void string_list_make_reverse_quick_sort(StringListEntry* list) {
    StringListEntry* string_entry;
    ListEntry* dir_head, * rev_head, * entry;

    dir_head = &list->quickSort;
    rev_head = &list->reverseQuickSort;
    entry = list_next(dir_head);

    while (entry != dir_head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, quickSort, entry);
        list_add(rev_head, list_remove(&string_entry->reverseQuickSort));
        entry = list_next(entry);
    }
}

// Функция выводит элементы списка в порядке инициализации
// <list> - голова списка
int string_list_init_order_print(StringListEntry* list) {
    int n = 0;
    StringListEntry* string_entry;
    ListEntry* head, * entry;
    head = &list->initOrderEntry;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, initOrderEntry, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry->value);
    }
    return n;
}

// Функция выводит элементы списка в обратном порядке инициализации
// <list> - голова списка
int string_list_reverse_order_print(StringListEntry* list) {
    int n = 0;
    StringListEntry* string_entry;
    ListEntry* head, * entry;
    head = &list->reverseOrderEntry;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, reverseOrderEntry, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry->value);
    }
    return n;
}

// Функция выводит элементы списка по длине
// <list> - голова списка
int string_list_sort_by_len_print(StringListEntry* list) {
    int n = 0;
    StringListEntry* string_entry;
    ListEntry* head, * entry;
    head = &list->sortByLen;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry->value);
    }
    return n;
}

// Функция выводит элементы списка в обратном порядке по длине
// <list> - голова списка
int string_list_reverse_sort_by_len_print(StringListEntry* list) {
    int n = 0;
    StringListEntry* string_entry;
    ListEntry* head, * entry;
    head = &list->reverseSortByLen;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, reverseSortByLen, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry->value);
    }
    return n;
}

// Функция выводит элементы списка после быстрой сортировки
// <list> - голова списка
int string_list_quick_sort_print(StringListEntry* list) {
    int n = 0;
    StringListEntry* string_entry;
    ListEntry* head, * entry;
    head = &list->quickSort;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, quickSort, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry->value);
    }
    return n;
}

// Функция выводит элементы списка в обратном порядке после быстрой сортировки
// <list> - голова списка
int string_list_reverse_quick_sort_print(StringListEntry* list) {
    int n = 0;
    StringListEntry* string_entry;
    ListEntry* head, * entry;
    head = &list->reverseQuickSort;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, reverseQuickSort, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry->value);
    }
    return n;
}