#include <Tools/ShaderPreprocessor.h>

#include <regex>

#define ENABLE_PROFILING 1
#define ENABLE_CACHING 0

#if ENABLE_PROFILING
//#include "cinder/Timer.h"
#endif

/*
using namespace std;

namespace {
	const regex sIncludeRegex = regex("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
} // anonymous namespace

ShaderPreprocessor::ShaderPreprocessor()
{
	init();
}

void ShaderPreprocessor::init()
{
	mSearchPaths.push_back(app::getAssetPath(""));
}

string ShaderPreprocessor::parse(const fs::path &path)
{
	set<fs::path> includeTree;

#if ENABLE_PROFILING
	Timer timer(true);
	string result = parseRecursive(path, fs::path(), includeTree);
	CI_LOG_I("parse " << path << " complete, ms: " << timer.getSeconds() * 1000);
	return result;
#else
	return parseRecursive(path, fs::path(), includeTree);
#endif
}

string ShaderPreprocessor::parseRecursive(const fs::path &path, const ci::fs::path &parentPath, set<fs::path> &includeTree)
{
	if (includeTree.count(path))
		throw ShaderPreprocessorExc("circular include found, path: " + path.string());

	includeTree.insert(path);

	const fs::path fullPath = findFullPath(path, parentPath);

#if ENABLE_CACHING
	const time_t timeLastWrite = fs::last_write_time(fullPath);

	auto cachedIt = mCachedSources.find(path);
	if (cachedIt != mCachedSources.end()) {
		if (cachedIt->second.mTimeLastWrite >= timeLastWrite) {
			return cachedIt->second.mString;
		}
	}
#endif

	stringstream output;

	ifstream input(fullPath.c_str());
	if (!input.is_open())
		throw ShaderPreprocessorExc("Failed to open file at path: " + fullPath.string());

	// go through each line and process includes

	string line;
	smatch matches;

	size_t lineNumber = 1;

	while (getline(input, line)) {
		if (regex_search(line, matches, sIncludeRegex)) {
			output << parseRecursive(matches[1].str(), fullPath.parent_path(), includeTree);
			output << "#line " << lineNumber << endl;
		}
		else
			output << line;

		output << endl;
		lineNumber++;
	}

	input.close();

#if ENABLE_CACHING
	Source &source = mCachedSources[path];
	source.mTimeLastWrite = timeLastWrite;
	source.mString = output.str();

	return source.mString;
#else
	return output.str();
#endif
}

fs::path ShaderPreprocessor::findFullPath(const fs::path &path, const ci::fs::path &parentPath)
{
	auto fullPath = parentPath / path;
	if (fs::exists(fullPath))
		return fullPath;

	for (const auto &searchPath : mSearchPaths) {
		fullPath = searchPath / path;
		if (fs::exists(fullPath))
			return fullPath;
	}

	throw ShaderPreprocessorExc("could not find shader with include path: " + path.string());
}

*/