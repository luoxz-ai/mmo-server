#ifndef LockfreeQueue_h__
#define LockfreeQueue_h__

#include <atomic>
#include <memory>

// Lock-free multiple-producer single-consumer
template<class T>
class MPSCQueue
{
public:
    MPSCQueue();
    ~MPSCQueue();

    template<class _T>
    void push(_T&& item)
    {
        Node* new_node = new Node(std::forward<_T>(item));
        Node* old      = head_.load(std::memory_order_relaxed);
        do
        {
            new_node->next = old;
        } while(!head_.compare_exchange_weak(old, new_node, std::memory_order_release));
    }
    bool get(T& item)
    {
        if(poll_list_)
        {
            Node* result = poll_list_;
            poll_list_   = poll_list_->next;
            item         = std::move(result->value);
            delete result;
            return true;
        }
        else
        {
            Node* head = head_.exchange(nullptr, std::memory_order_acquire);
            if(!head)
            {
                return false;
            }
            // Reverse list
            do
            {
                Node* temp = head;
                head       = head->next;
                temp->next = poll_list_;
                poll_list_ = temp;
            } while(head != nullptr);

            head       = poll_list_;
            item       = std::move(head->value);
            poll_list_ = head->next;
            delete head;
            return true;
        }
    }

private:
    struct Node
    {
        template<class _T>
        Node(_T&& val)
            : value(std::forward<_T>(val))
            , next(nullptr)
        {
        }

        Node* next;
        T     value;
    };

    std::atomic<Node*> head_;
    Node*              poll_list_; // for consumer only
};

template<class T>
MPSCQueue<T>::MPSCQueue()
    : head_(nullptr)
    , poll_list_(nullptr)
{
}

template<class T>
MPSCQueue<T>::~MPSCQueue()
{
    while(true)
    {
        while(poll_list_)
        {
            Node* result = poll_list_;
            poll_list_   = poll_list_->next;
            delete result;
        }
        Node* head = head_.exchange(nullptr, std::memory_order_acquire);
        if(!head)
        {
            break;
        }
        poll_list_ = head->next;
        delete head;
    }
}

#endif // LockfreeQueue_h__