#include "singleton.h"

SingletonDestroyer::~SingletonDestroyer()
{
    delete p_instance;
}

Singleton*         Singleton::p_instance = nullptr;
SingletonDestroyer Singleton::destroyer;
