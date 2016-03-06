#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <string>
#include <map>

namespace T3E
{

class ConfigFile {
public:

	ConfigFile();
	ConfigFile( std::string filename ) { load( filename ); }
	~ConfigFile();

	// Loads the configuratin file with the given filename
	// and stores key, value pairs
	bool load( std::string filename );

	bool getFloat( std::string key, float* value );

	bool getInt( std::string key, int* value );

private:

	std::map< std::string, std::string > data_;
};

}

#endif