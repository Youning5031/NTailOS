#pragma once

#include "osdef.h"

/**
 * @brief 获取链表节点所在的链表项
 * @param node 链表节点
 * @param type 链表项的类型
 * @param member 链表节点在链表项的成员名
 */
#define entryof(node, type, member) containerof(node, type, member)

// 双向循环链表
typedef struct s_list_node ListNode;

struct s_list_node
{
    ListNode *prev, *next;
};

/**
 * @brief 初始化节点
 * @param node 目标节点
 */
#define INIT_LIST_NODE(node) \
    (ListNode)               \
    {                        \
        node, node           \
    }

/**
 * @brief 定义新节点
 * @param node 节点名
 */
#define DEFINE_LIST_NODE(node) ListNode node = INIT_LIST_NODE(node)

/**
 * @brief 循环遍历链表
 * @param pos 循环变量，自动初始化
 * @param head 链表头地址
 */
#define list_for_each(pos, head) for (ListNode *pos = (head)->next; pos != (head); pos = pos->next)

/**
 * @brief 安全循环遍历链表
 * @param pos 循环变量，自动初始化
 * @param head 链表头地址
 */
#define list_for_each_s(pos, head) \
    for (ListNode *pos = (head)->next, *_sn = pos->next; pos != (head); pos = _sn, _sn = pos->next)

/**
 * @brief 反向循环遍历链表
 * @param pos 循环变量，自动初始化
 * @param head 链表头地址
 */
#define list_for_each_prev(pos, head) for (ListNode *pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * @brief 安全反向循环遍历链表
 * @param pos 循环变量，自动初始化
 * @param head 链表头地址
 */
#define list_for_each_prev_s(pos, head) \
    for (ListNode *pos = (head)->prev, *_sp = pos->prev; pos != (head); pos = _sp, _sp = pos->prev)

/**
 * @brief 循环遍历链表条目
 * @param type 链表项的类型
 * @param entry 循环变量，自动初始化
 * @param head 链表头地址
 * @param member 链表节点在链表项的成员名
 */
#define list_for_each_entry(type, entry, head, member)                              \
    for (type *entry = entryof((head)->next, type, member); &entry->member != head; \
         entry = entryof(entry->member.next, type, member))

/**
 * @brief 安全循环遍历链表条目
 * @param type 链表项的类型
 * @param entry 循环变量，自动初始化
 * @param head 链表头地址
 * @param member 链表节点在链表项的成员名
 */
#define list_for_each_entry_s(type, entry, head, member)                  \
    ListNode *_pos = (head)->next, *_sn = _pos->next;                     \
    for (type *entry = entryof(_pos->next, type, member); _pos != (head); \
         _pos = _sn, _sn = _pos->next, entry = entryof(_pos->next, type, member))

/**
 * @brief 将`new`插入`prev`和`next`之间
 * @param new 新的节点
 * @param prev 插入后的前驱
 * @param next 插入后的后继
 */
INLINE void list_insert(ListNode *new, ListNode *prev, ListNode *next)
{
    new->prev = prev;
    new->next = next;
    prev->next = new;
    next->prev = new;
}

/**
 * @brief 在`target`前插入`new`
 * @param new 新的节点
 * @param target 插入位置
 */
INLINE void list_insert_prev(ListNode *new, ListNode *target)
{
    list_insert(new, target->prev, target);
}

/**
 * @brief 在`target`后插入`new`
 * @param new 新的节点
 * @param target 插入位置
 */
INLINE void list_insert_next(ListNode *new, ListNode *target)
{
    list_insert(new, target, target->next);
}

/**
 * @brief 连接两个节点
 * @param prev 前一个节点
 * @param next 后一个节点
 */
INLINE void list_connect(ListNode *prev, ListNode *next)
{
    prev->next = next;
    next->prev = prev;
}

/**
 * @brief 删除节点
 * @param node 需要删除的节点
 */
INLINE void list_delete(ListNode *node)
{
    list_connect(node->prev, node->next);
}

/**
 * @brief 删除并重置节点
 * @param node 需要删除并重置的节点
 */
INLINE void list_delete_and_init(ListNode *node)
{
    list_connect(node->prev, node->next);
    *node = INIT_LIST_NODE(node);
}

/**
 * @brief 压入节点到第一个
 * @param list 链表
 * @param node 节点
 */
INLINE void list_push(ListNode *list, ListNode *node)
{
    list_insert_next(node, list);
}

/**
 * @brief 弹出第一个节点
 * @param list 链表
 * @return 第一个节点
 */
INLINE ListNode *list_pop(ListNode *list)
{
    ListNode *node = list->next;
    list_delete_and_init(node);
    return node;
}

/**
 * @brief 将`old`替换为`new`
 * @param old 旧的节点
 * @param new 新的节点
 */
INLINE void list_replace(ListNode *old, ListNode *new)
{
    list_insert(new, old->prev, old->next);
}

/**
 * @brief 将`old`替换为`new`，并重置`old`
 * @param old 旧的节点
 * @param new 新的节点
 */
INLINE void list_replace_and_init(ListNode *old, ListNode *new)
{
    list_insert(new, old->prev, old->next);
    *old = INIT_LIST_NODE(old);
}

/**
 * @brief 分割链表，范围为`[start, end)`，并将新链表连接到`new_list`
 * @param start 分割的起始，包含该节点
 * @param end 分割的结束，不包含该节点
 * @param new_list 哨兵节点，接受新链表的链表头
 */
INLINE void list_split(ListNode *start, ListNode *end, ListNode *new_list)
{
    if (start == end) return;
    ListNode *end_prev = end->prev;
    list_connect(start->prev, end);
    list_insert(new_list, end_prev, start);
}

/**
 * @brief 判断链表是否为空
 * @param list 链表
 * @return `true`为空/`false`非空
 */
INLINE bool list_is_empty(const ListNode *list)
{
    return list->next == list;
}

/**
 * @brief 判断节点是否为链表最后一个节点
 * @param list 链表
 * @param node 需要判断的节点
 * @return `true`为最后一个/`false`不是最后一个
 */
INLINE bool list_is_last(const ListNode *list, const ListNode *node)
{
    return list->prev == node;
}
