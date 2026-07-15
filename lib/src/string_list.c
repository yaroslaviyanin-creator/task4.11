/*
string_list.c - логика списка со строками, поддерживающего разные варианты связей
Янин Ярослав Иванович
Группа МК-101
*/

#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "string_list.h"

// генерация случайно строки их печатных символов
char *string_list_rnd_str(char *s, int len) {
  int i, l; // i - счетчик для цикла, l - случайная длина будущей строки
  char *c; // Объявляем указатель c, который будет бегать по символам строки
  l = rand() % (len - 1);
  for (c = s, i = 0; i < l; i++, c++) *c = 65 + (rand() % 26); // только заглавные буквы, чтобы глазками удобнее тестовые примеры проверять  // 33 + random(94); // а это куча печатных смволов
  *c = 0; // \0
  return s;
}

// создание нового элемента списка / головы нового списка
StringListEntry *string_list_new(char *str) {
    // Запрашиваем память под "коробку" элемента у нашего кастомного менеджера памяти
    StringListEntry *entry = mm_alloc();
    if (entry == NULL) return NULL; 
    // Если в функцию передали пустой указатель вместо строки
    if (str == NULL) entry -> value = NULL; // Записываем в поле значения NULL, так как текста нет (создаём голову)
    else {
      // Выделяем стандартную память под текст
      entry -> value = (char *)malloc(strlen(str) + 1); 
      if (!entry -> value) {
          mm_free(entry);
          return NULL;
      }
      strcpy(entry -> value, str);  // Если всё хорошо, копируем текст переданной строки в выделенную память внутри элемента
    }
    // Вызываем функцию инициализации указателей для этого элемента и возвращаем готовый элемент
    return string_list_init(entry);
}

// инициализация элемента списка entry
StringListEntry *string_list_init(StringListEntry *entry) {
    // Для каждого из 6 внедренных списков вызываем list_init, которая сделает их "кольцевыми"
    list_init(&entry -> initOrderEntry);
    list_init(&entry -> reverseOrderEntry);
    list_init(&entry -> sortByLen);
    list_init(&entry -> reverseSortByLen);
    list_init(&entry -> quickSort);
    list_init(&entry -> reverseQuickSort);
    return entry;
}

// создание нового элемента и его добавление в конец списка
StringListEntry *string_list_add(StringListEntry **list, char *str) {
    // Вызываем string_list_new
    StringListEntry *new_entry = string_list_new(str); // Выделяем память через mm_alloc и копируем строку
    if (new_entry == NULL) return NULL;
    // добавляем новый элемент в конец
    // Вставляем новый элемент сразу в КОНЕЦ всех 6 списков[cite: 13].
    // Конструкция (&(*list) -> ... ) -> prev означает элемент, стоящий ПЕРЕД головой, то есть конец кольца
    list_add((&(*list) -> initOrderEntry   ) -> prev, &new_entry -> initOrderEntry   );
    list_add((&(*list) -> reverseOrderEntry) -> prev, &new_entry -> reverseOrderEntry);
    list_add((&(*list) -> sortByLen        ) -> prev, &new_entry -> sortByLen        );
    list_add((&(*list) -> reverseSortByLen ) -> prev, &new_entry -> reverseSortByLen );
    list_add((&(*list) -> quickSort        ) -> prev, &new_entry -> quickSort        );
    list_add((&(*list) -> reverseQuickSort ) -> prev, &new_entry -> reverseQuickSort );
    return new_entry;
}

// освобождение памяти, занятой элементом списка
void string_list_free(StringListEntry *entry) {
    free(entry -> value);
    mm_free(entry);
}

// удаление элемента из списка с освобождением памяти
void string_list_remove(StringListEntry *entry) {
    list_remove(&entry -> initOrderEntry);
    list_remove(&entry -> reverseOrderEntry);
    list_remove(&entry -> sortByLen);
    list_remove(&entry -> reverseSortByLen);
    list_remove(&entry -> quickSort);
    list_remove(&entry -> reverseQuickSort);
    string_list_free(entry);
}

/*
// очистка списка с головой list; удаление всех элементов кроме головы, возвращает голову list
void string_list_clear(StringListEntry *list) {
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> initOrderEntry;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, initOrderEntry, entry);
        entry = list_next(entry);
        string_list_remove(string_entry);
    }
}*/

// размер списка
int string_list_count(StringListEntry *list) {
    ListEntry *head, *entry;
    int n = 0;
    head = &list -> initOrderEntry;
    entry = list_next(head);
    while (entry != head) {
      entry = list_next(entry);
      n++;
    }
    return n;
}

// очистка списка с головой list; удаление всех элементов кроме головы, возвращает голову list
StringListEntry *string_list_clear(StringListEntry *list) {
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> initOrderEntry;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, initOrderEntry, entry);
        entry = list_next(entry);
        string_list_free(string_entry);
    }
    return string_list_init(list);
}

/*
void GenerateRandomStringList (unsigned int size) {
}
*/

// ----------------------------------------------------------------

// упорядочивание списка reverseOrderEntry (инвертирование на месте)
void string_list_make_reverse_order(StringListEntry *list) {
    ListEntry *head, *forward, *backward, *tmp;
    head = &list -> reverseOrderEntry;
    forward  = list_next(head);
    backward = list_prev(head);
    while (forward != backward && forward != head && backward != head) {
        list_swap(forward, backward);
        if (list_prev(forward) == backward) break;
        tmp = forward;
        forward  = list_next(backward); // forward от backward'а, так как элементы поменялись местами
        backward = list_prev(tmp     ); // backward от forward'а, так как элементы поменялись местами
    }
}

// упорядочивание списка sortByLen пузырьком
void string_list_make_sort_by_len(StringListEntry *list) {
    StringListEntry *string_cur, *string_next;  // Указатели на структуры с текстом
    ListEntry *head, *entry_cur, *entry_next;   // Указатели на базовые узлы
    int swapped = 1;                            // Флаг замен для алгоритма
    head = &list -> sortByLen;                  // Берем голову списка sortByLen
    while (swapped) {
        swapped = 0;                            // Сбрасываем флаг
        entry_cur = list_next(head);            // Начинаем с первого элемента (после головы)
        // Цикл идет до тех пор, пока следующий элемент не окажется головой
        while ((entry_next = list_next(entry_cur)) != head) {
            // Макрос вычитает из адреса узла его смещение, получая адрес начала структуры StringListEntry
            string_cur  = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry_cur );
            string_next = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry_next);
            // Если длина текущего текста больше следующего, их надо поменять
            if (strlen(string_cur -> value) > strlen(string_next -> value)) {
                list_swap(entry_cur, entry_next);   // Переставляем узлы местами
                swapped = 1;
            } else entry_cur = list_next(entry_cur);// Иначе просто шагаем дальше
        }
    }
}

// упорядочивание списка reverseSortByLen (инвертирование на основе sortByLen)
void string_list_make_reverse_sort_by_len(StringListEntry *list) {
    StringListEntry *string_entry;
    ListEntry *dir_head, *rev_head, *entry;
    dir_head = &list -> sortByLen; // голова отсортированного списка, на основе которого нужно упорядочить реверсивный
    rev_head = &list -> reverseSortByLen; // голова реверсивного списка, который нужно упорядочить
    entry = list_next(dir_head); // начинаем движение по отсортированному списку с первого элемента  до конца
    while (entry != dir_head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry);
        // исключаем из реверсивного списка текущий элемент в отсортированном списке и добавляем его же справа от головы в реверсивный;
        // поскольку все элементы добавляются справа от головы, то в результате получается обратный порядок
        list_add(rev_head, list_remove(&string_entry -> reverseSortByLen));
        entry = list_next(entry);
    }
}

// упорядочивание части списка quickSort: left в список не включается, right - включается
void string_list_make_rqsort(ListEntry *left, ListEntry *right) {
    StringListEntry *string_base, *string_entry;
    ListEntry *base, *entry, *next;
    int is_last;
    base = list_next(left);     // опорным выбираем первый элемент из диапазона
    if (base == right) return;  // диапазон состоит из одного элемента, ничего сортировать не надо
    string_base = GET_CONTENT_RECORD(StringListEntry, quickSort, base);
    entry = list_next(base);    // Начинаем проверку со следующего элемента
    do {
        is_last = (entry == right); // Запоминаем, последний ли это элемент диапазона
        string_entry = GET_CONTENT_RECORD(StringListEntry, quickSort, entry);
        // Сравниваем строки лексикографически
        if (strcmp(string_entry -> value, string_base -> value) < 0) {
            next = list_next(entry);    // Запоминаем следующий узел
            // Вырезаем текущий элемент (list_remove) и вставляем его СЛЕВА от опорного (list_add к left)
            list_add(left, list_remove(entry));
            entry = next;   // Переходим к следующему элементу
            if (is_last) right = list_prev(entry);  // Корректируем правую границу, если перенесли последний элемент
        } else entry = list_next(entry);
    } while (!is_last);
    string_list_make_rqsort(left, base);    // Рекурсивно сортируем левую часть
    if (base != right) string_list_make_rqsort(base, right); // Рекурсивно сортируем правую часть
}
void string_list_make_quick_sort(StringListEntry *list) {
    ListEntry *head, *last;
    head = &list -> quickSort;  // Берем голову списка quickSort
    last = list_prev(head);     // Берем самый последний элемент списка
    if (last != head) string_list_make_rqsort(head, last);
}

// упорядочивание списка reverseQuickSort (инвертирование на основе QuickSort)
void string_list_make_reverse_quick_sort(StringListEntry *list) {
    StringListEntry *string_entry;
    ListEntry *dir_head, *rev_head, *entry;
    dir_head = &list -> quickSort; // голова отсортированного списка, на основе которого нужно упорядочить реверсивный
    rev_head = &list -> reverseQuickSort; // голова реверсивного списка, который нужно упорядочить
    entry = list_next(dir_head); // начинаем движение по отсортированному списку с первого элемента  до конца
    while (entry != dir_head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, quickSort, entry);
        // исключаем из реверсивного списка текущий элемент в отсортированном списке и добавляем его же справа от головы в реверсивный;
        // поскольку все элементы добавляются справа от головы, то в результате получается обратный порядок
        list_add(rev_head, list_remove(&string_entry -> reverseQuickSort));
        entry = list_next(entry);
    }
}


// ----------------------------------------------------------------

// вывод строк из списка list в консоль (для отладки)
int string_list_init_order_print(StringListEntry *list) {
    int n = 0;
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> initOrderEntry;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, initOrderEntry, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry -> value);
    }
    return n;
}

int string_list_reverse_order_print(StringListEntry *list) {
    int n = 0;
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> reverseOrderEntry;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, reverseOrderEntry, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry -> value);
    }
    return n;
}

int string_list_sort_by_len_print(StringListEntry *list) {
    int n = 0;
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> sortByLen;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, sortByLen, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry -> value);
    }
    return n;
}

int string_list_reverse_sort_by_len_print(StringListEntry *list) {
    int n = 0;
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> reverseSortByLen;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, reverseSortByLen, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry -> value);
    }
    return n;
}

int string_list_quick_sort_print(StringListEntry *list) {
    int n = 0;
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> quickSort;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, quickSort, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry -> value);
    }
    return n;
}

int string_list_reverse_quick_sort_print(StringListEntry *list) {
    int n = 0;
    StringListEntry *string_entry;
    ListEntry *head, *entry;
    head = &list -> reverseQuickSort;
    entry = list_next(head);
    while (entry != head) {
        string_entry = GET_CONTENT_RECORD(StringListEntry, reverseQuickSort, entry);
        entry = list_next(entry);
        printf("%5d. %s\n", ++n, string_entry -> value);
    }
    return n;
}

