#ifndef _STRING_LIST_H_
#define _STRING_LIST_H_

#include "mm.h"
#include "list.h"

typedef struct _StringListEntry {
    char *value;
    ListEntry initOrderEntry;
    ListEntry reverseOrderEntry;
    ListEntry sortByLen;
    ListEntry reverseSortByLen;
    ListEntry quickSort;
    ListEntry reverseQuickSort;

} StringListEntry;


char *string_list_rnd_str(char *s, int len); // генерация случайно строки их печатных символов
StringListEntry *string_list_new(char *str); // создание нового элемента списка / головы нового списка
StringListEntry *string_list_init(StringListEntry *entry); // инициализация элемента списка entry
StringListEntry *string_list_add(StringListEntry **list, char *str); // создание нового элемента и его добавление в конец списка
void string_list_free(StringListEntry *entry); // освобождение памяти, занятой элементом списка
void string_list_remove(StringListEntry *entry); // удаление элемента из списка с освобождением памяти
int string_list_count(StringListEntry *list); // размер списка
StringListEntry *string_list_clear(StringListEntry *list); // очистка списка с головой list; удаление всех элементов кроме головы, возвращает голову list
// упрядочивание списков
void string_list_make_reverse_order      (StringListEntry *list); // упорядочивание списка reverseOrderEntry (инвертирование на месте)
void string_list_make_sort_by_len        (StringListEntry *list); // упорядочивание списка sortByLen пузырьком
void string_list_make_reverse_sort_by_len(StringListEntry *list); // упорядочивание списка reverseSortByLen (инвертирование на основе sortByLen)
void string_list_make_quick_sort         (StringListEntry *list); // упорядочивание списка quickSort
void string_list_make_reverse_quick_sort (StringListEntry *list); // упорядочивание списка reverseQuickSort
// вывод строк из списка list в консоль (для отладки)
int string_list_init_order_print         (StringListEntry *list);
int string_list_reverse_order_print      (StringListEntry *list);
int string_list_sort_by_len_print        (StringListEntry *list);
int string_list_reverse_sort_by_len_print(StringListEntry *list);
int string_list_quick_sort_print         (StringListEntry *list);
int string_list_reverse_quick_sort_print (StringListEntry *list);

#endif  // _STRING_LIST_H_

