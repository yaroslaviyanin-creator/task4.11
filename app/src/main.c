/*
main.c - приложение
Янин Ярослав Иванович
Группа МК-101
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "mm.h"
#include "string_list.h"

#define MM_BLOCKS 2 // количество элементов списка, по умолчанию размещающихся в блоке менеджера памяти
#define TEST_LIST_ENTITIES 100 
#define TEST_BUFFER_MAX_LEN 64

StringListEntry *list;

// пользовательская функция для перерасчета адресов при перевыделении памяти в менеджере памяти
// Функция вызывается из mm_alloc при переезде массива. ofs - это сдвиг в байтах (новый адрес минус старый)
void realloc_callback(int ofs) {
    ListEntry *head, *currentListEntry, *entry;
    StringListEntry *currentStringListEntry = list; // Начинаем с головы
    if (ofs) { // Если память успешно выделена
        printf("MM reallocating; update addresses...\n");
        currentListEntry = head = &currentStringListEntry -> initOrderEntry;
        do {
            // Получаем структуру данных по указателю узла
            currentStringListEntry = GET_CONTENT_RECORD(StringListEntry, initOrderEntry, currentListEntry);
            currentListEntry = list_next(currentListEntry); // Шагаем вперед

            // прибовляем смещение
            entry = &currentStringListEntry -> initOrderEntry;
            (char *)entry -> next += ofs;
            (char *)entry -> prev += ofs;
            entry = &currentStringListEntry -> reverseOrderEntry;
            (char *)entry -> next += ofs;
            (char *)entry -> prev += ofs;
            entry = &currentStringListEntry -> sortByLen;
            (char *)entry -> next += ofs;
            (char *)entry -> prev += ofs;
            entry = &currentStringListEntry -> reverseSortByLen;
            (char *)entry -> next += ofs;
            (char *)entry -> prev += ofs;
            entry = &currentStringListEntry -> quickSort;
            (char *)entry -> next += ofs;
            (char *)entry -> prev += ofs;
            entry = &currentStringListEntry -> reverseQuickSort;
            (char *)entry -> next += ofs;
            (char *)entry -> prev += ofs;
        } while (currentListEntry != head); // Идем, пока не обойдем весь список
        (char *)list += ofs;    // Сдвигаем саму глобальную переменную "голова списка"
    } else {
        printf("MM reallocating; can not reallocate memory\n");
    }
}

int main () {
    char buffer[TEST_BUFFER_MAX_LEN];   // Буфер для генерации случайных строк при тестировании
    int i, j, n;                        // Переменные для циклов тестирования менеджера памяти
    ListEntry *entry;                   // Временный указатель на узел списка для обхода

    // создание менеджера памяти
    // Вызываем функцию инициализации. Передаем размер структуры, количество блоков и функцию перевыделения памяти
    if (mm_initialize(sizeof(StringListEntry), MM_BLOCKS, &realloc_callback)) {
        // ругаемся на невозможность выделения динамической памяти
        printf("Can not allocate memory!\n");
        return -1;
    }
    // создание списка, фактически, его головы
    list = string_list_new(NULL);   // Вызываем функцию создания элемента, передавая NULL вместо строки текста
    srand((unsigned int)time(NULL));

    // тест списков
    //printf("\n---- list test ----\n");
    //string_list_add(&list, "test444");
    //string_list_add(&list, "test22222");
    //string_list_add(&list, "test6");
    //string_list_add(&list, "test3333");
    //string_list_add(&list, "test111111");
    //string_list_add(&list, "test55");
    //string_list_add(&list, "test777");
    //string_list_add(&list, "test88888");
    //string_list_make_reverse_order      (list);
    //string_list_make_sort_by_len        (list);
    //string_list_make_reverse_sort_by_len(list);
    //string_list_make_quick_sort         (list);
    //string_list_make_reverse_quick_sort (list);
    //printf("Initial order:\n");
    //string_list_init_order_print(list);
    //printf("Reverse order:\n");
    //string_list_reverse_order_print(list);
    //printf("Sorted by len:\n");
    //string_list_sort_by_len_print(list);
    //printf("Reversed sorted by len:\n");
    //string_list_reverse_sort_by_len_print(list);
    //printf("Quick sorted:\n");
    //string_list_quick_sort_print(list);
    //printf("Reversed quick sorted:\n");
    //string_list_reverse_quick_sort_print(list);
    //string_list_clear(list); 

    // тест менеджера памяти
    printf("\n---- memory manager test ----\n");
    n = TEST_LIST_ENTITIES;
    for (j = 0; j < 3; j++) {
        // Добавляем 100 случайных строк
        for (i = 0; i < n; i++) string_list_add(&list, string_list_rnd_str(buffer, TEST_BUFFER_MAX_LEN));
        string_list_make_quick_sort(list);
        for (i = n / 2; i; i--) {
            entry = list_next(&list -> quickSort);
            string_list_remove(GET_CONTENT_RECORD(StringListEntry, quickSort, entry));
        }
        n = string_list_count(list); // неэффективно, но зато протестируем функцию определения количества элементов в списке
        printf("\nQuick sorted, ineration %d, count: %d:\n", j, n);
        string_list_quick_sort_print(list);
        n *= 3;
    }

    // очистка списка и удаление головы списка
    string_list_free(string_list_clear(list));
    // удаление менеджера памяти
    mm_finalize();
    return 0;
}

