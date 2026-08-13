#ifndef REFLECTCLASS_H
#define REFLECTCLASS_H
#include <QByteArray>
#include <QMetaObject>
#include <QHash>
#include <QObject>

template<typename L>  //用于反射传父类参�?
class CReflectClass
{
public:
    //首先要注册创建的�?
    template<typename T>
    static void registerClass()
    {
        //&constructorHelper<T>其实是获取创建的函数指针,核心功能�?
        constructors().insert(T::staticMetaObject.className(), &constructorHelper<T>);
    }

    static QObject *createObject(const QByteArray& className,L* parent = nullptr)
    {
        Constructor constructor = constructors().value(className);
        if (constructor == nullptr ){
            return nullptr;
        }
        return (*constructor)(parent);//执行new T函数，创建对应实�?
    }
private:
    typedef QObject *(*Constructor)(L* parent);
    template<typename T>
    static QObject *constructorHelper(L* parent)
    {
        if(parent)
            return new T(parent);
        else {
            return new T();
        }
    }
    static QHash<QByteArray, Constructor> &constructors()
    {
        static QHash<QByteArray, Constructor> instance;
        return instance;
    }
};
#endif // REFLECTCLASS_H
