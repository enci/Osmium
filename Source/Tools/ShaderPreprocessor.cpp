#include <Tools/ShaderPreprocessor.h>
#include <regex>
#include <sstream>
#include <Defines.h>
#include "../../../Blast/Tools/SharedTools.h"

#define ENABLE_PROFILING 0
#define ENABLE_CACHING 0

#if ENABLE_PROFILING
#endif

using namespace Osm;
using namespace std;

namespace {
	const regex sIncludeRegex = regex("^[ ]*#[ ]*pragma include[ ]+[\"<](.*)[\">].*");
} // anonymous namespace


ShaderPreprocessor::ShaderPreprocessor()
{
	_searchPaths.push_back("");
}

string ShaderPreprocessor::Read(const string& path)
{
	set<string> includeTree;
	return ParseRecursive(path, "", includeTree);
}

string ShaderPreprocessor::ParseRecursive(
	const string& path,
	const string& parentPath,
	set<string>& includeTree)
{
	string fullPath = parentPath.empty() ? path : parentPath + "/" + path;

	if (includeTree.count(fullPath))
	{
		LOG("Circular include found! Path: %s", path.c_str());
		return string();
	}

	includeTree.insert(fullPath);	
	
#if ENABLE_CACHING
	const time_t timeLastWrite = fs::last_write_time(fullPath);

	auto cachedIt = mCachedSources.find(path);
	if (cachedIt != mCachedSources.end()) {
		if (cachedIt->second.mTimeLastWrite >= timeLastWrite) {
			return cachedIt->second.mString;
		}
	}
#endif	

	string parent = GetParentPath(fullPath);
	string inputString = ReadFile(fullPath);
	if(inputString.empty())
	{
		LOG("Included shader file found! Path: %s", fullPath.c_str());
		return string();
	}

	stringstream input(move(inputString));
	stringstream output;

	// go through each line and process includes
	string line;
	smatch matches;
	size_t lineNumber = 1;
	while (getline(input, line))
	{
		if (regex_search(line, matches, sIncludeRegex))
		{
			output << ParseRecursive(matches[1].str(), parent, includeTree);
			//output << "#line " << lineNumber << endl;
		}
		else
		{
			if(!line.empty() && line[0] != '\0') // Don't null terminate
				output << line;
		}
		output << endl;
		lineNumber++;
	}

#if ENABLE_CACHING
	Source &source = mCachedSources[path];
	source.mTimeLastWrite = timeLastWrite;
	source.mString = output.str();

	return source.mString;
#else
	return output.str();
#endif
}

string ShaderPreprocessor::GetParentPath(const string& path)
{
	// Implementation base on:
	// http://stackoverflow.com/questions/28980386/how-to-get-file-name-from-a-whole-path-by-c

	string parent = "";
	string::size_type found = path.find_last_of("/");

	// if we found one of this symbols
	if (found != string::npos)
		parent = path.substr(0, found);
	
	return parent;
}
