#include <gtest/gtest.h>
#include "../include/allocator.h"
#include "../include/dynamic_array.h"

using namespace std;

// === DynamicArray tests ===

TEST(test_da1, basic_int_operations) 
{
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<int> allocator(&memory_resource);
    
    DynamicArray<int> array(2, allocator);
    
    array.push_back(10);
    array.push_back(20);
    array.push_back(30);
    
    EXPECT_EQ(array.size(), 3);
    EXPECT_GE(array.capacity(), 3);
    
    EXPECT_EQ(array[0], 10);
    EXPECT_EQ(array[1], 20);
    EXPECT_EQ(array[2], 30);
}

TEST(test_da2, iterator_operations) 
{
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<int> allocator(&memory_resource);
    
    DynamicArray<int> array(2, allocator);
    
    array.push_back(1);
    array.push_back(2);
    array.push_back(3);
    
    int sum = 0;
    for (const auto& item : array) {
        sum += item;
    }
    EXPECT_EQ(sum, 6);
    
    int count = 0;
    for (auto it = array.begin(); it != array.end(); ++it) {
        EXPECT_EQ(*it, ++count);
    }
}

TEST(test_da3, pop_back_operations) 
{
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<int> allocator(&memory_resource);
    
    DynamicArray<int> array(allocator);
    
    EXPECT_TRUE(array.empty());
    EXPECT_EQ(array.size(), 0);
    
    array.push_back(42);
    EXPECT_FALSE(array.empty());
    EXPECT_EQ(array.size(), 1);
    EXPECT_EQ(array[0], 42);
    
    array.pop_back();
    EXPECT_TRUE(array.empty());
}

// === Struct operations tests ===

TEST(test_da4, struct_operations) 
{
    struct Point {
        int x, y;
        Point(int x = 0, int y = 0) : x(x), y(y) {}
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<Point> allocator(&memory_resource);
    
    DynamicArray<Point> points(2, allocator);
    
    points.push_back(Point(1, 2));
    points.push_back(Point(3, 4));
    points.push_back(Point(5, 6));
    
    EXPECT_EQ(points.size(), 3);
    EXPECT_EQ(points[0], Point(1, 2));
    EXPECT_EQ(points[1], Point(3, 4));
    EXPECT_EQ(points[2], Point(5, 6));
    
    int total_x = 0;
    for (const auto& point : points) {
        total_x += point.x;
    }
    EXPECT_EQ(total_x, 9);
}

TEST(test_da5, complex_struct_operations) 
{
    struct Employee {
        int id;
        string name;
        double salary;
        
        Employee(int id = 0, string name = "", double salary = 0.0)
            : id(id), name(name), salary(salary) {}
    };
    
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<Employee> allocator(&memory_resource);
    
    DynamicArray<Employee> employees(2, allocator);
    
    employees.push_back(Employee(1, "Alice", 50000.0));
    employees.push_back(Employee(2, "Bob", 60000.0));
    
    EXPECT_EQ(employees.size(), 2);
    EXPECT_EQ(employees[0].id, 1);
    EXPECT_EQ(employees[0].name, "Alice");
    EXPECT_EQ(employees[1].name, "Bob");
    EXPECT_EQ(employees[1].salary, 60000.0);
}

// === Exception safety tests ===

TEST(test_da6, out_of_range_exceptions) 
{
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<int> allocator(&memory_resource);
    
    DynamicArray<int> array(2, allocator);
    array.push_back(1);
    array.push_back(2);
    
    EXPECT_THROW(array[5], out_of_range);
    EXPECT_THROW(array[100], out_of_range);
    
    const DynamicArray<int>& const_array = array;
    EXPECT_THROW(const_array[5], out_of_range);
}

// === MemoryResource tests ===

TEST(test_mr1, basic_allocation_operations) 
{
    CustomMemoryResource memory_resource;
    
    void* ptr1 = memory_resource.allocate(16, 8);
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = memory_resource.allocate(32, 8);
    EXPECT_NE(ptr2, nullptr);
    
    memory_resource.deallocate(ptr1, 16, 8);
    memory_resource.deallocate(ptr2, 32, 8);
}

TEST(test_mr2, memory_reuse_operations) 
{
    CustomMemoryResource memory_resource;
    
    void* ptr1 = memory_resource.allocate(16, 8);
    void* ptr2 = memory_resource.allocate(32, 8);
    
    memory_resource.deallocate(ptr1, 16, 8);
    memory_resource.deallocate(ptr2, 32, 8);
    
    void* ptr3 = memory_resource.allocate(16, 8);
    void* ptr4 = memory_resource.allocate(32, 8);
    
    EXPECT_NE(ptr3, nullptr);
    EXPECT_NE(ptr4, nullptr);
}

TEST(test_mr3, resource_comparison_operations) 
{
    CustomMemoryResource resource1;
    CustomMemoryResource resource2;
    
    EXPECT_TRUE(resource1.is_equal(resource1));
    EXPECT_FALSE(resource1.is_equal(resource2));
}

// === Integration tests ===

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        int_array = make_unique<DynamicArray<int>>(2, int_alloc);
        for (int i = 0; i < 5; ++i) {
            int_array->push_back(i * 10);
        }
    }

    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<int> int_alloc{&memory_resource};
    unique_ptr<DynamicArray<int>> int_array;
};

TEST_F(IntegrationTest, integration_operations) 
{
    ASSERT_EQ(int_array->size(), 5);
    EXPECT_EQ((*int_array)[0], 0);
    EXPECT_EQ((*int_array)[1], 10);
    EXPECT_EQ((*int_array)[4], 40);
    
    int sum = 0;
    for (const auto& item : *int_array) {
        sum += item;
    }
    EXPECT_EQ(sum, 100);
}

TEST_F(IntegrationTest, large_data_handling) 
{
    DynamicArray<int> large_array(int_alloc);
    
    const size_t LARGE_SIZE = 100;
    for (size_t i = 0; i < LARGE_SIZE; ++i) {
        large_array.push_back(static_cast<int>(i));
    }
    
    EXPECT_EQ(large_array.size(), LARGE_SIZE);
    
    for (size_t i = 0; i < LARGE_SIZE; ++i) {
        EXPECT_EQ(large_array[i], static_cast<int>(i));
    }
}

// === Edge cases tests ===

TEST(test_edge1, empty_array_operations) 
{
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<int> allocator(&memory_resource);
    
    DynamicArray<int> empty_array(allocator);
    
    EXPECT_TRUE(empty_array.empty());
    EXPECT_EQ(empty_array.size(), 0);
    EXPECT_EQ(empty_array.capacity(), 0);
}

TEST(test_edge2, single_element_operations) 
{
    CustomMemoryResource memory_resource;
    pmr::polymorphic_allocator<int> allocator(&memory_resource);
    
    DynamicArray<int> single_array(allocator);
    single_array.push_back(999);
    
    EXPECT_FALSE(single_array.empty());
    EXPECT_EQ(single_array.size(), 1);
    EXPECT_EQ(single_array[0], 999);
    
    single_array.pop_back();
    EXPECT_TRUE(single_array.empty());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
