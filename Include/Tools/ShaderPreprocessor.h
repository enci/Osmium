/*
Copyright (c) 2017 Bojan Endrovski, - All rights reserved.
Portions have been adapted from code written by Paul Houx, Simon Geilfus, Richard Eakin 
Check: https://gist.github.com/richardeakin/f67a696cfd1f4ef3a816
*/

#pragma once

#include <map>
#include <set>
#include <vector>
#include <time.h>

namespace Osm
{

class ShaderPreprocessor
{
public:
	ShaderPreprocessor();

	std::string		Read(const std::string& path);

private:
	std::string		ParseRecursive(	const std::string& path,
									const std::string& parentPath,
									std::set<std::string>& includeTree);

	std::string		GetParentPath(const std::string&path);

	std::vector<std::string>			_searchPaths;
};

}

