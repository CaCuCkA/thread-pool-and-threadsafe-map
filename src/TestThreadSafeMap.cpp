#include "ThreadSafeMap.h"

#include <iostream>
#include <thread>

int main()
{
    ThreadSafeMap<std::string, int> ageMap;

    std::thread t1([&ageMap]()
   {
       ageMap.Insert("Bob", 18);
       ageMap.Insert("John", 19);
       ageMap.Insert("Alex", 100);
   });

    std::thread t2([&ageMap]()
   {
       std::cout << "Size of map: " << ageMap.Size() << std::endl;
   });

    std::thread t3([&ageMap]()
   {
       ageMap.Remove("Bob");
   });

    std::thread t4([&ageMap]()
   {
       if (ageMap.Empty())
       {
           std::cout << "Map is empty" << std::endl;
       }
       else
       {
           std::cout << "Map is not empty" << std::endl;
       }
   });

   std::thread t5([&ageMap]()
  {
      int age;
      if (ageMap.Find("Bob", age))
      {
          std::cout << "Bob`s age is " << age << std::endl;
      }
      else
      {
          std::cout << "Bob`s age is not found" << std::endl;
      }
  });

    std::thread t6([&ageMap]()
   {
       std::cout << "Get Alex age using operator []: " << ageMap["Alex"] << std::endl;
   });


    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    return 0;
}