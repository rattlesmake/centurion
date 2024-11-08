#include <iostream>
#include <string>
#include <vector>


#include <pybind11/pybind11.h>
#include <pybind11/embed.h>
#include <pybind11/eval.h>
#include <pybind11/stl.h>

// main function

class Dog;

class Animal
{
	friend class Dog;
public:
	Animal(const Dog& other) = delete;
	Animal& operator=(const Animal& other) = delete;
	//Animal() {};
	void Test() { std::cout << "test" << std::endl; }
protected:
	Animal() {};
};

class Dog : public Animal
{
public:
	Dog& operator=(const Dog& other) = delete;
	Dog() {};
	void Test2() { std::cout << "test2" << std::endl; }
};

namespace py = pybind11;



PYBIND11_EMBEDDED_MODULE(admin, m)
{
	py::class_<Animal, std::shared_ptr<Animal>>(m, "Animal")
		.def("Test", &Animal::Test)
		;

	py::class_<Dog, Animal, std::shared_ptr<Dog>>(m, "Dog")
		.def("Test2", &Dog::Test2)
		;
}

int main(int numArgs, char* args[])
{
	try
	{
		auto guard = new py::scoped_interpreter();
		py::exec("from admin import *");
		
		py::module_ main = py::module_::import("__main__");

		std::shared_ptr<Dog> dog = std::shared_ptr<Dog>(new Dog());
		main.attr("x") = std::static_pointer_cast<Animal>(dog);
		
		//py::exec("x = GetDog()");
		py::exec("print(x)");
		return 0;
	}
	catch (const std::exception& ex)
	{
		//TODO
		std::cout << "[DEBUG] " << ex.what() << std::endl;
	}
}
