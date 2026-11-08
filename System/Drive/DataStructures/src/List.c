/**
 * @file List.c
 * @brief CN：双向链表实现文件，提供节点初始化、头插、尾插、正/反向遍历与按值
 *        删除。节点通过 prev/next 双向串接，头尾两端都以 NULL 作为边界标记。
 *        -- EN：Doubly linked list implementation providing node initialization,
 *        push-front, push-back, forward/backward traversal and delete-by-value.
 *        Nodes are linked in both directions and NULL marks both ends.
 */

#include "List.h"


void (*List_Iint_pointer)(List_Struct* prev,int num)               = List_Iint;
/* CN：List_PrintForward / List_PrintBackward 由下面的 POSITIVE 宏二选一编译，
 *     所以这里指向它们的函数指针也必须用同一个条件包起来，否则会引用到未编译的
 *     那个函数，链接时报 undefined reference。
 * EN：Only one of List_PrintForward / List_PrintBackward is compiled, chosen by
 *     the POSITIVE macro, so these pointers must use the same condition;
 *     otherwise they reference the function that was not compiled and the link
 *     fails with "undefined reference". */
#if POSITIVE
    void (*List_PrintForward_pointer)(List_Struct* head)               = List_PrintForward;
#else
    void (*List_PrintBackward_pointer)(List_Struct* head)              = List_PrintBackward;
#endif
List_Struct* (*List_PushFront_pointer)(List_Struct* head, int num) = List_PushFront;
List_Struct* (*List_PushBack_pointer)(List_Struct* head, int num)  = List_PushBack;
List_Struct* (*List_Delete_pointer2)(List_Struct* head, int num)   = List_Delete;


/**
 * @brief CN：初始化一个已存在的链表节点：写入数据域，并把前驱、后继指针清空。
 *        函数先按 LOG_SWITCH 决定是否打印调试信息，再检查节点指针是否为空，最后
 *        依次写 data、prev、next；本函数不负责申请节点内存。
 *        -- EN：Initialize an existing list node by storing the value and
 *        clearing both links. A LOG_SWITCH branch optionally prints a trace,
 *        the node pointer is checked against NULL, then data, prev and next are
 *        written in order; no memory is allocated here.
 *
 * @param prev CN：输入，待初始化节点的指针；函数通过它就地写入数据域与两个链接
 *             指针，不修改指针本身。
 *             -- EN：Input pointer to the node being initialized; the data
 *             field and both links are written through it, the pointer itself
 *             is not modified.
 *
 * @param num CN：输入，写入数据域的初始数值，int 类型，按值传递。
 *            -- EN：Input initial value stored into the data field, int passed
 *            by value.
 *
 * @return CN：无返回值；初始化结果通过指针参数就地体现。
 *         -- EN：No return value; the initialized state is exposed through the
 *         pointer parameter.
 */
void List_Iint(List_Struct* node ,int num)
{
#if LOG_SWITCH
    printf("List_Iint()函数,节点地址%p,初始值%d\n",(void*)node,num);  // CN：输出节点地址与初始值用于调试；%p 配指针、%d 配 int，宏为 0 时整段不参与编译。-- EN：Dump the node address and initial value for debugging; %p takes the pointer and %d the int, and the whole block is dropped when the macro is 0.
#endif

    if(node == NULL)              // CN：检查节点指针是否为空，为空说明没有可写入的节点，直接报错返回。-- EN：Check the node pointer against NULL; if it is empty there is nothing to write, so report and return.
    {
        printf("内存分配失败!\n");      // CN：打印内存分配失败提示，告知调用方节点不可用。-- EN：Print an allocation-failure message telling the caller the node is unusable.
        return ;                 // CN：提前结束函数，避免继续向非法地址写入。-- EN：Exit early to avoid writing through an invalid address.
    }

    node -> data = num;       // CN：把参数 num 写入数据域，作为节点的初始值。-- EN：Store the num parameter into the data field as the node value.
    node -> prev = NULL;   // CN：清空前驱指针，使该节点可以作为链表头。-- EN：Clear the prev link so the node can act as the head.
    node -> next = NULL;   // CN：清空后继指针，表示当前还没有下一个节点。-- EN：Clear the next link, meaning no successor exists yet.
}

/**
 * @brief CN：头插法。先为新节点申请堆内存，写入数值后把 next 指向原头节点，再把
 *        原头节点的 prev 回指新节点，最终由新节点充当表头。空链表时新节点直接
 *        成为唯一节点。分配失败则原样返回旧头指针，链表不变。
 *        -- EN：Push-front. Allocate a node, store the value, point its next at
 *        the old head, then fix the old head's prev back to the new node so the
 *        new node becomes the head. For an empty list the new node is the only
 *        node. On allocation failure the old head is returned unchanged.
 *
 * @param head CN：输入，当前链表头指针；为 NULL 表示空链表。
 *             -- EN：Input current head pointer; NULL means an empty list.
 *
 * @param num CN：输入，写入新节点数据域的数值。
 *            -- EN：Input value stored into the new node's data field.
 *
 * @return CN：返回插入后的新头指针；分配失败时返回原头指针，链表保持不变。
 *         -- EN：Return the new head after insertion, or the original head on
 *         allocation failure with the list left unchanged.
 */
List_Struct* List_PushFront(List_Struct* head, int num)
{
#if LOG_SWITCH
    printf("List_PushFront()函数,传入头节点%p,插入的值%d\n",(void*)head,num);  // CN：输出传入的头指针与插入值用于调试；%p 配指针、%d 配 int，num 不能解引用。-- EN：Dump the incoming head pointer and the inserted value; %p takes the pointer and %d the int, and num must not be dereferenced.
#endif

    List_Struct* node = (List_Struct*)malloc(sizeof(List_Struct));  // CN：为新节点申请一块与 List_Struct 等长的堆内存，并转换为节点指针类型。-- EN：Allocate a heap block the size of List_Struct for the new node and cast it to the node pointer type.

    if(node == NULL)            // CN：检查内存申请是否成功，失败时不改动原链表。-- EN：Check the allocation result; the original list is left untouched on failure.
    {
        printf("分配头部内存空间失败！");  // CN：打印分配失败提示，便于定位内存不足问题。-- EN：Print an allocation-failure message to help locate out-of-memory issues.
        return head;            // CN：返回原头指针，保证链表结构不被破坏。-- EN：Return the original head so the list stays intact.
    }

    node -> data = num;         // CN：把传入数值写入新节点的数据域。-- EN：Store the value into the new node's data field.
    node -> prev = NULL;        // CN：新节点成为表头，前驱指针置空。-- EN：The new node becomes the head, so its prev link is cleared.
    node -> next = head;        // CN：新节点的后继指向原头节点，完成与旧链表的衔接。-- EN：Point the new node's next link at the old head to splice it in.

    if(head != NULL)            // CN：链表非空时，需要把原头节点的前驱回指新节点，保证双向链接完整。-- EN：For a non-empty list, fix the old head's prev so the doubly linked structure stays consistent.
    {
        head -> prev = node;    // CN：原头节点的 prev 指向新节点，形成 head<->node 双向关系。-- EN：Point the old head's prev at the new node, forming a two-way head<->node link.
    }

    return node;                // CN：返回新节点，它已成为链表头；返回 0 会让调用方丢失整个链表。-- EN：Return the new node, now the head; returning 0 would make the caller lose the whole list.
}

/**
 * @brief CN：尾插法。申请新节点并清空其两个链接，若原链表为空则新节点直接作为
 *        头节点返回；否则用游标从头走到尾，把尾节点的 next 指向新节点，并把新
 *        节点的 prev 指回原尾节点。头指针仅在空链表插入时才会改变。
 *        -- EN：Push-back. Allocate a node with cleared links; if the list is
 *        empty the new node is returned as the head; otherwise a cursor walks
 *        to the tail, the tail's next is set to the new node, and the new
 *        node's prev points back at the old tail. The head only changes when
 *        inserting into an empty list.
 *
 * @param head CN：输入，当前链表头指针；为 NULL 表示空链表。
 *             -- EN：Input current head pointer; NULL means an empty list.
 *
 * @param num CN：输入，写入新节点数据域的数值。
 *            -- EN：Input value stored into the new node's data field.
 *
 * @return CN：返回链表头指针；空链表插入时返回新节点，非空时返回原 head。
 *         -- EN：Return the head pointer; the new node for an empty list, the
 *         original head otherwise.
 */
List_Struct* List_PushBack(List_Struct* head, int num)
{
#if LOG_SWITCH
    printf("List_PushBack()函数,传入头节点%p,插入的值%d\n",(void*)head,num);  // CN：输出传入的头指针与插入值用于调试；%p 配指针、%d 配 int，宏为 0 时整段不参与编译。-- EN：Dump the incoming head pointer and the inserted value; %p takes the pointer and %d the int, and the whole block is dropped when the macro is 0.
#endif
    List_Struct* node = (List_Struct*)malloc(sizeof(List_Struct));  // CN：为新节点申请堆内存，大小与 List_Struct 一致。-- EN：Allocate heap memory for the new node, sized to List_Struct.
    if (node == NULL)
    {
        return head;                  // CN：分配失败时直接返回原头指针，链表保持不变。-- EN：Return the original head on allocation failure; the list is unchanged.
    }

    node->data = num;                 // CN：写入数据域。-- EN：Store the payload.
    node->prev = NULL;                // CN：新节点暂时孤立，前驱先置空。-- EN：Clear prev since the node is still isolated.
    node->next = NULL;                // CN：后继同样置空，后续再按需要串接。-- EN：Clear next as well; links are spliced afterwards.

    if (head == NULL)
    {
        return node;                  // CN：空链表时新节点就是头节点，直接返回。-- EN：For an empty list the new node is the head, so return it directly.
    }


    List_Struct* cur = head;          // CN：以头节点为起点设置遍历游标，用于寻找尾节点。-- EN：Start the traversal cursor at the head to find the tail.
    while (cur->next != NULL)
    {
         cur = cur->next;             // CN：沿 next 一直前进，直到游标停在尾节点上。-- EN：Advance along next until the cursor rests on the tail node.
    }

    cur->next = node;                 // CN：原尾节点的后继指向新节点，新节点接在链表末尾。-- EN：Point the old tail's next at the new node, appending it.
    node->prev = cur;                 // CN：新节点的前驱指回原尾节点，补全双向链接。-- EN：Point the new node's prev back at the old tail to complete the two-way link.

    return head;                      // CN：头指针未变，原样返回。-- EN：The head is unchanged and returned as is.
}

#if POSITIVE
    /**
     * @brief CN：正向遍历并打印链表。从头节点出发沿 next 前进，逐个打印数据域，
     *        直到遇到 NULL 为止。该函数只在 POSITIVE 宏非 0 时参与编译。
     *        -- EN：Traverse forward and print the list. Starting at the head it
     *        follows next, printing each payload until NULL is reached. It is
     *        only compiled when POSITIVE is non-zero.
     *
     * @param head CN：输入，链表头指针；为 NULL 时循环体不执行，只输出空行。
     *             -- EN：Input head pointer; when NULL the loop body never runs
     *             and only the trailing newline is printed.
     *
     * @return CN：无返回值，结果直接打印到标准输出。
     *         -- EN：No return value; the result is printed to stdout.
     */
    void List_PrintForward(List_Struct* head)
    {
    #if LOG_SWITCH
        printf("进入List_PushBack()函数\n");  // CN：输出调试轨迹，字符串内容与当前函数不一致（原样保留）。-- EN：Print the debug trace; the string does not match this function (left untouched).
    #endif
        List_Struct* cur = head;          // CN：设置遍历游标，从头节点开始。-- EN：Set the traversal cursor to the head.
        printf("正向: ");                   // CN：先打印方向前缀，后续数值紧跟其后输出。-- EN：Print the direction prefix before the values.
        while (cur != NULL)
        {
            printf("%d ", cur->data);     // CN：打印当前节点的数据域，数值之间以空格分隔。-- EN：Print the current payload, separating values with a space.
            cur = cur->next;              // CN：游标后移一个节点，继续遍历。-- EN：Advance the cursor to the next node.
        }
        printf("\n");                     // CN：遍历结束后换行，避免与后续输出黏连。-- EN：Print a newline after traversal so later output starts on a new line.
    }
#else
    /**
     * @brief CN：反向遍历并打印链表。先用游标走到尾节点，再沿 prev 逐个回退并
     *        打印数据域，直到 NULL 为止。POSITIVE 为 0 时编译的就是本函数。
     *        -- EN：Traverse backward and print the list. A cursor first walks to
     *        the tail, then follows prev back to NULL while printing payloads.
     *        With POSITIVE equal to 0 this is the function that gets compiled.
     *
     * @param head CN：输入，链表头指针；为 NULL 时直接打印空链表提示后返回。
     *             -- EN：Input head pointer; when NULL an empty-list message is
     *             printed and the function returns.
     *
     * @return CN：无返回值，结果直接打印到标准输出。
     *         -- EN：No return value; the result is printed to stdout.
     */
    void List_PrintBackward(List_Struct* head)
    {
    #if LOG_SWITCH
        printf("List_PrintBackward()函数,传入头节点%p\n",(void*)head);  // CN：输出传入的头指针用于调试；%p 配指针，宏为 0 时整段不参与编译。-- EN：Dump the incoming head pointer for debugging; %p takes the pointer, and the whole block is dropped when the macro is 0.
    #endif
        if (head == NULL)
        {
            printf("反向: (空)\n");          // CN：空链表没有可遍历的节点，打印提示后结束。-- EN：An empty list has nothing to traverse, so print a hint and stop.
            return;                       // CN：提前返回，避免对空指针解引用。-- EN：Return early to avoid dereferencing a null pointer.
        }

        List_Struct* cur = head;          // CN：设置游标，先从头节点开始。-- EN：Set the cursor, starting at the head.
        while (cur->next != NULL)         // CN：先一路走到尾节点，为反向遍历做准备。-- EN：Walk to the tail first, preparing for the backward pass.
        {
            cur = cur->next;              // CN：沿 next 前进，直到 cur->next 为空。-- EN：Advance along next until cur->next is NULL.
        }

        printf("反向: ");                   // CN：打印方向前缀，随后输出各节点数值。-- EN：Print the direction prefix before the values.

        while (cur != NULL)
        {
            printf("%d ", cur->data);     // CN：打印当前节点的数据域。-- EN：Print the current payload.
            cur = cur->prev;              // CN：借助 prev 往回走，实现反向遍历。-- EN：Step back through prev to traverse backward.
        }

        printf("\n");                     // CN：输出结束换行。-- EN：End the line with a newline.
    }
#endif

/**
 * @brief CN：按值删除第一个匹配的节点。先用游标查找目标节点，未找到则原样返回
 *        头指针；找到后分三种情况改写指针：目标不是头时让前驱的后继跳过它，
 *        目标是头时头指针后移，目标不是尾时让后继的前驱跳过它，最后释放节点。
 *        -- EN：Delete the first node whose payload matches. A cursor locates the
 *        target and the head is returned unchanged when nothing matches. When
 *        found, links are rewritten in three cases: a non-head target is
 *        skipped by its predecessor's next, a head target moves the head
 *        forward, and a non-tail target is skipped by its successor's prev;
 *        finally the node is freed.
 *
 * @param head CN：输入，链表头指针；为 NULL 时查找立即结束并原样返回。
 *             -- EN：Input head pointer; when NULL the search ends at once and
 *             it is returned unchanged.
 *
 * @param num CN：输入，待删除节点的数值，只比较第一个匹配项。
 *            -- EN：Input value to delete; only the first match is removed.
 *
 * @return CN：返回删除后的链表头指针；若被删除的是头节点则返回其后继，未找到时
 *         返回原头指针（此时链表内容不变）。
 *         -- EN：Return the head after deletion: the successor when the head was
 *         removed, or the original head when no match was found (list intact).
 */
List_Struct* List_Delete(List_Struct* head, int num)
{
#if LOG_SWITCH
    printf("List_Delete()函数,传入头节点%p,待删除的值%d\n",(void*)head,num);  // CN：输出传入的头指针与待删除值用于调试；%p 配指针、%d 配 int，宏为 0 时整段不参与编译。-- EN：Dump the incoming head pointer and the value to delete; %p takes the pointer and %d the int, and the whole block is dropped when the macro is 0.
#endif
    List_Struct* cur = head;                 // CN：从表头开始设置查找游标。-- EN：Start the search cursor at the head.

    while (cur != NULL && cur->data != num)  // CN：顺序查找第一个数据域等于 num 的节点。-- EN：Search linearly for the first node whose payload equals num.
    {
        cur = cur->next;                     // CN：未命中则游标后移，继续比较下一个节点。-- EN：On a miss advance the cursor and compare the next node.
    }

    if (cur == NULL) return head;            // CN：走到链尾仍未找到，链表保持不变，直接返回原头指针。-- EN：Reached the end without a match, so return the original head unchanged.

    if (cur->prev != NULL)                   // CN：目标不是头节点，其前驱存在，可安全改写前驱的后继。-- EN：The target is not the head and has a predecessor whose next can be rewritten.
    {
        cur->prev->next = cur->next;         // CN：前驱的后继直接指向目标的后继，把目标从链上摘除。-- EN：Point the predecessor's next at the target's successor, unlinking the target.
    }
    else
    {
        head = cur->next;                    // CN：目标是头节点，头指针后移到第二个节点。-- EN：The target is the head, so move the head to the second node.
    }


    if (cur->next != NULL)                   // CN：目标不是尾节点，其后继存在，需要补回前驱指针。-- EN：The target is not the tail, so its successor exists and its prev must be fixed.
    {
        cur->next->prev = cur->prev;         // CN：后继的前驱跳过目标，直接指向目标的前驱。-- EN：Point the successor's prev past the target to the target's predecessor.
    }


    free(cur);                               // CN：释放目标节点占用的堆内存，防止内存泄漏。-- EN：Free the target node to avoid a memory leak.

    return head;                             // CN：返回调整后的头指针。-- EN：Return the adjusted head pointer.
}
