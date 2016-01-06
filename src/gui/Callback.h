#ifndef CALLBACK_H
#define CALLBACK_H

#include <functional>
#include <QObject>

class Callback : public QObject
{
Q_OBJECT

public:
   typedef std::function<void()> FunctionType;

   Callback(FunctionType fn, QObject *parent = nullptr) : QObject(parent), functor(std::move(fn)) {}

public slots:
    void execute() { functor(); }

private:
    FunctionType functor;
};

inline Callback* makeCallback(Callback::FunctionType fn, QObject *parent = nullptr)
{
    return new Callback(std::move(fn), parent);
}

#endif
