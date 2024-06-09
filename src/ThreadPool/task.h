using callback = void(*)(void*);
template<typename T>
struct Task {
    Task(){
        function = nullptr;
        arg = nullptr;
    }
    Task(callback f, void *arg){
        function = f;
        arg = arg;
    }
    callback function;
    T* arg;
};