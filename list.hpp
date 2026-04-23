#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
template<typename T>
class list {
protected:
    class node {
    public:
        T *data;
        node *prev;
        node *next;

        node() : data(nullptr), prev(nullptr), next(nullptr) {}

        node(const T &value) : data(new T(value)), prev(nullptr), next(nullptr) {}

        ~node() {
            delete data;
        }
    };

protected:
    node *head;
    node *tail;
    size_t len;

    node *insert(node *pos, node *cur) {
        cur->prev = pos->prev;
        cur->next = pos;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }

    node *erase(node *pos) {
        node *ret = pos->next;
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        return ret;
    }

public:
    class const_iterator;
    class iterator {
    private:
        list *container;
        node *current;

    public:
        iterator() : container(nullptr), current(nullptr) {}

        iterator(list *c, node *n) : container(c), current(n) {}

        iterator operator++(int) {
            if (current == container->tail) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            current = current->next;
            return tmp;
        }

        iterator & operator++() {
            if (current == container->tail) {
                throw invalid_iterator();
            }
            current = current->next;
            return *this;
        }

        iterator operator--(int) {
            if (current == container->head->next) {
                throw invalid_iterator();
            }
            iterator tmp = *this;
            current = current->prev;
            return tmp;
        }

        iterator & operator--() {
            if (current == container->head->next) {
                throw invalid_iterator();
            }
            current = current->prev;
            return *this;
        }

        T & operator *() const {
            if (current == container->head || current == container->tail) {
                throw invalid_iterator();
            }
            return *(current->data);
        }

        T * operator ->() const {
            if (current == container->head || current == container->tail) {
                throw invalid_iterator();
            }
            return current->data;
        }

        bool operator==(const iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }

        bool operator==(const const_iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }

        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }

        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }

        friend class list;
        friend class const_iterator;
    };

    class const_iterator {
    private:
        const list *container;
        node *current;

    public:
        const_iterator() : container(nullptr), current(nullptr) {}

        const_iterator(const list *c, node *n) : container(c), current(n) {}

        const_iterator(const iterator &other) : container(other.container), current(other.current) {}

        const_iterator operator++(int) {
            if (current == container->tail) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            current = current->next;
            return tmp;
        }

        const_iterator & operator++() {
            if (current == container->tail) {
                throw invalid_iterator();
            }
            current = current->next;
            return *this;
        }

        const_iterator operator--(int) {
            if (current == container->head->next) {
                throw invalid_iterator();
            }
            const_iterator tmp = *this;
            current = current->prev;
            return tmp;
        }

        const_iterator & operator--() {
            if (current == container->head->next) {
                throw invalid_iterator();
            }
            current = current->prev;
            return *this;
        }

        const T & operator *() const {
            if (current == container->head || current == container->tail) {
                throw invalid_iterator();
            }
            return *(current->data);
        }

        const T * operator ->() const {
            if (current == container->head || current == container->tail) {
                throw invalid_iterator();
            }
            return current->data;
        }

        bool operator==(const iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }

        bool operator==(const const_iterator &rhs) const {
            return container == rhs.container && current == rhs.current;
        }

        bool operator!=(const iterator &rhs) const {
            return !(*this == rhs);
        }

        bool operator!=(const const_iterator &rhs) const {
            return !(*this == rhs);
        }

        friend class list;
    };

    list() {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        len = 0;
    }

    list(const list &other) {
        head = new node();
        tail = new node();
        head->next = tail;
        tail->prev = head;
        len = 0;

        node *p = other.head->next;
        while (p != other.tail) {
            push_back(*(p->data));
            p = p->next;
        }
    }

    virtual ~list() {
        clear();
        delete head;
        delete tail;
    }

    list &operator=(const list &other) {
        if (this == &other) return *this;

        clear();
        node *p = other.head->next;
        while (p != other.tail) {
            push_back(*(p->data));
            p = p->next;
        }
        return *this;
    }

    const T & front() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(head->next->data);
    }

    const T & back() const {
        if (empty()) {
            throw container_is_empty();
        }
        return *(tail->prev->data);
    }

    iterator begin() {
        return iterator(this, head->next);
    }

    const_iterator cbegin() const {
        return const_iterator(this, head->next);
    }

    iterator end() {
        return iterator(this, tail);
    }

    const_iterator cend() const {
        return const_iterator(this, tail);
    }

    virtual bool empty() const {
        return len == 0;
    }

    virtual size_t size() const {
        return len;
    }

    virtual void clear() {
        node *p = head->next;
        while (p != tail) {
            node *tmp = p;
            p = p->next;
            delete tmp;
        }
        head->next = tail;
        tail->prev = head;
        len = 0;
    }

    virtual iterator insert(iterator pos, const T &value) {
        if (pos.container != this) {
            throw invalid_iterator();
        }
        node *newNode = new node(value);
        insert(pos.current, newNode);
        len++;
        return iterator(this, newNode);
    }

    virtual iterator erase(iterator pos) {
        if (pos.container != this) {
            throw invalid_iterator();
        }
        if (pos.current == head || pos.current == tail) {
            throw invalid_iterator();
        }
        if (empty()) {
            throw container_is_empty();
        }
        node *nextNode = erase(pos.current);
        delete pos.current;
        len--;
        return iterator(this, nextNode);
    }

    void push_back(const T &value) {
        insert(end(), value);
    }

    void pop_back() {
        if (empty()) {
            throw container_is_empty();
        }
        erase(--end());
    }

    void push_front(const T &value) {
        insert(begin(), value);
    }

    void pop_front() {
        if (empty()) {
            throw container_is_empty();
        }
        erase(begin());
    }

    void sort() {
        if (len <= 1) return;

        node **arr = new node*[len];
        node *p = head->next;
        for (size_t i = 0; i < len; i++) {
            arr[i] = p;
            p = p->next;
        }

        sjtu::sort<node*>(arr, arr + len, [](const node *a, const node *b) {
            return *(a->data) < *(b->data);
        });

        head->next = arr[0];
        arr[0]->prev = head;
        for (size_t i = 0; i < len - 1; i++) {
            arr[i]->next = arr[i + 1];
            arr[i + 1]->prev = arr[i];
        }
        arr[len - 1]->next = tail;
        tail->prev = arr[len - 1];

        delete[] arr;
    }

    void merge(list &other) {
        if (this == &other) return;

        node *p1 = head->next;
        node *p2 = other.head->next;

        while (p1 != tail && p2 != other.tail) {
            if (*(p2->data) < *(p1->data)) {
                node *next2 = p2->next;
                other.erase(p2);
                insert(p1, p2);
                len++;
                p2 = next2;
            } else {
                p1 = p1->next;
            }
        }

        while (p2 != other.tail) {
            node *next2 = p2->next;
            other.erase(p2);
            insert(tail, p2);
            len++;
            p2 = next2;
        }
    }

    void reverse() {
        node *p = head;
        while (p != nullptr) {
            node *tmp = p->next;
            p->next = p->prev;
            p->prev = tmp;
            p = tmp;
        }
        std::swap(head, tail);
    }

    void unique() {
        if (len <= 1) return;

        node *p = head->next;
        while (p != tail && p->next != tail) {
            if (*(p->data) == *(p->next->data)) {
                node *toDelete = p->next;
                erase(toDelete);
                delete toDelete;
                len--;
            } else {
                p = p->next;
            }
        }
    }
};

}

#endif //SJTU_LIST_HPP
