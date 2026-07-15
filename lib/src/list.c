// логика базового списка

#include "list.h"

// инициализация нового элемента списка / головы нового списка
void list_init(ListEntry *entry) {
    entry -> next = entry; // закольцованный список
    entry -> prev = entry; // закольцованный список
}

// добавление нового элемента после указанного
ListEntry *list_add(ListEntry *entry, ListEntry *new_entry) {
    // NULL не проверяем, так как список закольцован
    new_entry -> next = entry -> next;      // Новый элемент указывает вперед на того, кто был после entry
    new_entry -> prev = entry;              // Новый элемент указывает назад на entry
    entry -> next = new_entry;              // entry теперь указывает вперед на новый элемент
    new_entry -> next -> prev = new_entry;  // Тот, кто стоит после нового, теперь указывает назад на новый
    return new_entry;
}

/*
// добавление нового элемента в конец списка
void InsertTailList (ListEntry *head, ListEntry *newEntry) {
}
// добавление нового элемента в начало списка
void InsertHead (ListEntry *head, ListEntry *newEntry) {
}
*/

// Удаление элемента из цепочки
ListEntry *list_remove(ListEntry *entry) {
    // NULL не проверяем, так как список закольцован
    entry -> next -> prev = entry -> prev;  // Элемент справа теперь указывает назад на элемент слева
    entry -> prev -> next = entry -> next;  // Элемент слева теперь указывает вперед на элемент справа
    return entry;
}

/*
void FreeList (ListEntry *head) {
}
*/

// перестановка элементов
void list_swap(ListEntry *entry1, ListEntry *entry2) {
    ListEntry *entry1_next, *entry1_prev;
    if (entry1 == entry2) return; // сам с собою элемент не переставляем
    if (entry1 -> next == entry2) {
        // перстановка соседних элементов, entry1 слева от entry2
        // Аккуратно перевязываем 6 указателей (2 у соседей снаружи, 4 у самих элементов)
        entry1 -> prev -> next = entry2;
        entry2 -> next -> prev = entry1;
        entry2 -> prev = entry1 -> prev;
        entry1 -> next = entry2 -> next;
        entry1 -> prev = entry2;
        entry2 -> next = entry1;

    } else if (entry2 -> next == entry1) {
        // перстановка соседних элементов, entry1 справа от entry2
        list_swap(entry2, entry1);  // Вызываем эту же функцию, поменяв аргументы местами

        //??? третий вариант - список из всего двух переставляемых элементов - не существует, так как у нас список с головой
    } else {
        // элементы не граничат друг с другом
        // Сохраняем старых соседей первого элемента
        entry1_next = entry1 -> next;
        entry1_prev = entry1 -> prev;
        // Перевязываем соседей так, чтобы они указывали на новые места
        entry1_prev -> next = entry2;
        entry1_next -> prev = entry2;
        entry2 -> prev -> next = entry1;
        entry2 -> next -> prev = entry1;
        // Меняем указатели самих элементов
        entry1 -> next = entry2 -> next;
        entry1 -> prev = entry2 -> prev;
        entry2 -> next = entry1_next;
        entry2 -> prev = entry1_prev;
    }
}

ListEntry *list_next(ListEntry *entry) {
    return entry -> next;
}


ListEntry *list_prev(ListEntry *entry) {
    return entry -> prev;
}
