#include <iostream>
#include "allocator.h"
#include "dynamic_array.h"

using namespace std;

int main() {
    try {
        CustomMemoryResource custom_memory_resource;
        
        cout << "\nadd array" << endl;
        DynamicArray<int> int_array(2, pmr::polymorphic_allocator<int>(&custom_memory_resource));
        
        cout << "\nadd elements" << endl;
        int_array.push_back(100);
        int_array.push_back(200); 
        int_array.push_back(300);
        
        cout << "\nget elements" << endl;
        cout << "int_array[0] = " << int_array[0] << endl;
        cout << "int_array[1] = " << int_array[1] << endl;
        cout << "int_array[2] = " << int_array[2] << endl;
        
        cout << "\nget elements through iterators" << endl;
        for (const auto& val : int_array) {
            cout << val << " ";
        }
        cout << endl;
        
    } catch (const exception& e) {
        cout << "error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
