#include <queue>
#include <mutex>

template<class T>
class ThreadSafeQueue {
    private:
        std::queue<T> q;
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
            if(q.empty()){
                m.unlock();
                return ""; //empty string for null
            }
            T popped = q.front();
            q.pop();
            m.unlock();
            return popped;
        }
};  