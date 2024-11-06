#include <iostream>
#include <functional>


// 定义一个C函数指针类型
typedef void (*CFunctionPtr)(int);

void highFunc(CFunctionPtr ptr)
{
    ptr(42);
}
// 定义一个C++函数对象类型
struct MyFunctionObject {
    void operator()(int value) {
        std::cout << "Value: " << value << std::endl;
    }
};

int main() {
    // 创建一个std::function对象，并绑定一个函数对象
    std::function<void(int)> funcObj = MyFunctionObject();

    // 将std::function对象赋值给C函数指针
    CFunctionPtr cFuncPtr = funcObj.target<void(int)>();

    highFunc(cFuncPtr);

    return 0;
}
