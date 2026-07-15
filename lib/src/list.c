/*
list.c - логика базового списка.
Янин Ярослав Иванович
Группа МК-101
*/

#include "list.h"

// Функция инициализирует новый элемент списка или голову нового списка
// <entry> - указатель на базовый узел списка
void list_init(ListEntry* entry) {
    entry->next = entry; // закольцованный список
    entry->prev = entry; // закольцованный список
}

// Функция добавляет новый элемент после указанного
// <entry> - элемент, после которого происходит вставка
// <new_entry> - новый вставляемый элемент
ListEntry* list_add(ListEntry* entry, ListEntry* new_entry) {
    new_entry->next = entry->next;      // Новый элемент указывает вперед на того, кто был после entry
    new_entry->prev = entry;              // Новый элемент указывает назад на entry
    entry->next = new_entry;              // entry теперь указывает вперед на новый элемент
    new_entry->next->prev = new_entry;  // Тот, кто стоит после нового, теперь указывает назад на новый
    return new_entry;
}

// Функция исключает элемент из двусвязного списка, перевязывая соседей
// <entry> - элемент, который нужно удалить из цепочки
ListEntry* list_remove(ListEntry* entry) {
    entry->next->prev = entry->prev;  // Элемент справа теперь указывает назад на элемент слева
    entry->prev->next = entry->next;  // Элемент слева теперь указывает вперед на элемент справа
    return entry;
}

// Функция возвращает указатель на следующий элемент списка
// <entry> - текущий элемент
ListEntry* list_next(ListEntry* entry) {
    return entry->next;
}

// Функция возвращает указатель на предыдущий элемент списка
// <entry> - текущий элемент
ListEntry* list_prev(ListEntry* entry) {
    return entry->prev;
}
