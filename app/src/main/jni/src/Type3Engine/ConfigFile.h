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

	void getInt( std::string key, int* value, int default_value );

	bool getString( std::string key, std::string* value );

private:

	std::string trimWhitespace( const std::string& str );

	std::map< std::string, std::string > data_;

	std::string name;
};

}

#endif