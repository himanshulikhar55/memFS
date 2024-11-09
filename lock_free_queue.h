#include "headers.h"

class lock_free_queue {
public:
    struct Node {
        thread_data* data;
        std::atomic<Node*> next;
        
        Node(thread_data* data): data(data), next(NULL) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;


    lock_free_queue() {
        Node* dummy = new Node(NULL);
        head.store(dummy);
        tail.store(dummy);
    }

    ~lock_free_queue() {
        while (Node* node = head.load()) {
            head.store(node->next);
            delete node;
        }
    }

    void enqueue(thread_data* data) {
        Node* newNode = new Node(data);
        Node* oldTail = tail.exchange(newNode);
        oldTail->next.store(newNode);
    }

    bool dequeue(thread_data*& result) {
        Node* oldHead = head.load();
        Node* next = oldHead->next.load();

        if (next == NULL) {
            return false;
        }

        result = next->data;
        head.store(next);
        delete oldHead;
        return true;
    }
};
