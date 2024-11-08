/*
* ---------------------------
* CENTURION
* [2019] - [2022] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#ifndef SFML_ENABLED
#define SFML_ENABLED 0
#endif // !SFML_ENABLED

#include <centurion_typedef.hpp>

#include <tinyxml2_utils.h> // it includes string and tinyxml2
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <deque>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <stack>
#include <array>
#include <string>
#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <optional>
#include <imgui_utils.h>
#include <duration_logger.h>

#include <variant>

#include <stl_utils.h>
#include <custom_exceptions.hpp>

//Threads
#include <thread_guard.h>
#include <condition_variable>

using std::string;
using std::unordered_map;
using std::map;
using std::list;
using std::vector;
using std::set;
using std::unordered_set;
using std::array;
using std::pair;
using std::stack;
using std::deque;
using std::cout;
using std::endl;

//Provide an hash function for a std::pair<T1,T2> object
struct hash_pair
{
	template <class T1, class T2>
	[[nodiscard]] size_t operator()(const pair<T1, T2>& p) const noexcept
	{
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ hash2;
	}
};

struct hash_weakPtr
{
	template <class T>
	[[nodiscard]] size_t operator()(const std::weak_ptr<T>& wp) const noexcept
	{
		//Beware: This may lead to undefined behaviour
		//since the hash of a key is assumed to be constant, but, int this case, it will change
		//when the weak_ptr expires.
		auto sp = wp.lock();
		return std::hash<decltype(sp)>()(sp);
	}
};

struct equalCMP_weakPtr
{
	template <class T>
	[[nodiscard]] bool operator()(const std::weak_ptr<T>& left, const std::weak_ptr<T>& right) const noexcept
	{
		if (left.expired() == false && right.expired() == false)
			return (left.lock() == right.lock());
		return !left.owner_before(right) && !right.owner_before(left);
	}
};
