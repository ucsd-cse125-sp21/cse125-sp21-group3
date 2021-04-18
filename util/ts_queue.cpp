#include <queue>

template<class T>
class ThreadSafeQueue {
    private:
        std::queue<T*> q;
        std::mutex m;

    public:
        ThreadSafeQueue(){}

        void push(T item){
            m.lock();
            q.push(item);
            m.unlock();
        }

        T pop(){
            m.lock();
            if(q.empty())
                return NULL;
            T popped = q.front();
            q.pop();
            return popped;
        }
};