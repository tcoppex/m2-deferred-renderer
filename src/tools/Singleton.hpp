/**
 * 
 *    \file Singleton.hpp
 * 
 *    Abstract class used to define a Singleton object.
 *    Not thread safe.
 * 
 */

#pragma once

#ifndef TOOLS_SINGLETON_HPP
#define TOOLS_SINGLETON_HPP


template <class T>
class Singleton
{
  private:
    static T* sm_Instance;    
      
  public:
    inline
    static T& GetInstance()
    {
      if (!sm_Instance) {
        sm_Instance = new T;
      }
      
      return *sm_Instance;
    }
    
    inline
    static void DeleteInstance()
    {
      if (sm_Instance)
      {
        delete sm_Instance;
        sm_Instance = 0;
      }
    }
    
  protected:
    Singleton() {}
    
  private:
    Singleton(Singleton&);
};


template<class T> T* Singleton<T>::sm_Instance = 0;


#endif //TOOLS_SINGLETON_HPP
