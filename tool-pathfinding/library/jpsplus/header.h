/*
* ---------------------------
* CENTURION
* [2019] - [2020] Rattlesmake
* All Rights Reserved.
* ---------------------------
*/

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <deque>
#include <regex>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <stack>
#include <array>
#include <string>
#include <iomanip>
#include <algorithm>
#include <iterator>

#include <GL/glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

using std::string;
using std::unordered_map;
using std::map;
using std::list;
using std::vector;
using std::set;
using std::stack;
using std::unordered_set;
using std::array;
using std::pair;
using std::deque;
using std::cout;
using std::endl;

struct hash_pair
{
	template <class T1, class T2>
	size_t operator()(const pair<T1, T2>& p) const
	{
		auto hash1 = std::hash<T1>{}(p.first);
		auto hash2 = std::hash<T2>{}(p.second);
		return hash1 ^ hash2;
	}
};