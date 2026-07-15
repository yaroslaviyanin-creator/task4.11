// логика базового списка

#ifndef _LIST_H_
#define _LIST_H_

// Вычисляем смещение
#define OFFSET_OF(TypeName, entryName)  \
    (unsigned char*)&((TypeName*)0)->entryName  
    // Представляем что структура находится в памяти ровно по адресу 0
    // и обращаемся к нужному полю внутри этой воображаемой структуры
    // и берем адрес этого поля.

// Вычитаем смещение из реального адреса
#define GET_CONTENT_RECORD(TypeName, entryName, addr)    \
    ((TypeName*)((unsigned char*)addr - OFFSET_OF(TypeName, entryName)))    
    // addr — реальный адрес узелка ListEntry
    // временно превращаем этот указатель в указатель на байты
    // берем реальный адрес узелка и отступаем назад ровно на то количество байт, которое мы вычислили в OFFSET_OF
    // мы оказываемся ровно в начале структуры StringListEntry
    
typedef struct _ListEntry {
    struct _ListEntry *next;
    struct _ListEntry *prev;
} ListEntry;

void list_init(ListEntry *head); // инициализация нового элемента списка / головы нового списка
ListEntry *list_add(ListEntry *entry, ListEntry *new_entry); // добавление нового элемента после указанного
ListEntry *list_remove(ListEntry *entry); // исключение элемента из списка
void list_swap(ListEntry *entry1, ListEntry *entry2); // перестановка элементов
ListEntry *list_next(ListEntry *entry);
ListEntry *list_prev(ListEntry *entry);

#endif  // _LIST_H_

