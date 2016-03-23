#include "ConfigFile.h"
#include <sstream>
#include <string>
#include <limits>
#include <cstdlib>
#include "IOManager.h"
#include "SDL.h"

namespace T3E
{

ConfigFile::ConfigFile()
{
}

ConfigFile::~ConfigFile()
{
}

bool ConfigFile::load( std::string filename )
{
	std::stringstream file;

	std::vector<char> v;

	// Return false if the file is not valid
	if( !T3E::IOManager::readTextToBuffer( filename, v ) )
	{
		SDL_Log("Could not get config file '%s'", filename.c_str());
		return false;
	}
	else 
	{
		SDL_Log("Opened config file '%s'", filename.c_str());

		// Convert the vector of chars to a stringstream
		for( auto c : v )
			file << c;
	}

	while( file )
	{
		std::string line;
		std::getline( file, line );

		if( line.empty() || line[0] == '#' || line[0] == '\n' )
		{
			continue;			
		}

		std::size_t split_point = line.find(':');
		if( split_point != std::string::npos )
		{
			// Check the split point isn't the first or last char
			if( split_point == 0 || split_point == line.size() - 2 ) continue;

			std::string key, value;

			key = line.substr(0, split_point);
			value = trimWhitespace( line.substr(split_point + 1) );

			SDL_Log("Paired '%s': '%s'", key.c_str(), value.c_str() );

			data_[key] = value;
		}
	}

	return true;
}

bool ConfigFile::getFloat( std::string key, float* value )
{
	auto it = data_.find(key);

	// if the iterator points to the end, the key was not found
	if( it == data_.end() ) return false;

	// For some reason the modern c++11 std::stof functions are not avalible in the ndk
	// So have to use the <cstdlib> atof functions
	*value = atof(it->second.c_str());

	return true;
}

bool ConfigFile::getInt( std::string key, int* value )
{
	auto it = data_.find(key);

	// If the iterator points to the end, the key was not found
	if( it == data_.end() ) return false;

	*value = atoi(it->second.c_str());

	return true;
}

bool ConfigFile::getString( std::string key, std::string* value )
{
	auto it = data_.find(key);

	// If the iterator points to the end, the key was not found
	if( it == data_.end() ) return false;

	*value = it->second;
	//SDL_Log("Retrived %s as '%s'", key.c_str(), value->c_str() );

	return true;
}

std::string ConfigFile::trimWhitespace( const std::string& str )
{
	const size_t strBegin = str.find_first_not_of(" \t");
    if (strBegin == std::string::npos)
        return ""; // no content

    const size_t strEnd = str.find_last_not_of(" \t");
    const size_t strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

}