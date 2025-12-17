#ifndef GLOBAL_H
#define GLOBAL_H


// 单例模式，用于参数传递，懒汉模式
class global
{

public:
    static global& getInstance(){
        static global instance;
        return instance;
    }
    ~global(){};
    global(const global&) = delete;
    global& operator = (const global&) = delete;
private:
    global(){};

public:
    int getDlloutFlag() const;
    void setDlloutFlag(int value);

private:
    int DlloutFlag = -1;

};

#endif // GLOBAL_H
