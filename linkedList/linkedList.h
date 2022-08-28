#if !defined(LINKED_LIST_M)
#define LINKED_LIST_M

typedef struct ListNode{
    struct ListNode *prev;
    void* value;
    struct ListNode *next;
} ListNode;

typedef ListNode List;

typedef struct {
    void (*pushItem)(List* list, void* value);
    int (*length)(List* list);
    void* (*itemAt)(List* list, int index);
    List* (*map)(List* list, void*(*mapper)(void* value, int index));
    List* (*createList)();
    void (*forEach)(List* list, void (*callback)(void* value, int index));
    void (*clear)(List* list);
    List* (*leftPush)(List* list, void* value);
    List* (*shift)(List* list);
} LinkedListModuleType;

LinkedListModuleType LinkedList;

#endif // LINKED_LIST_M
