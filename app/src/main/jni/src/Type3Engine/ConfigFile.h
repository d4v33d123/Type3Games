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
	/**
	* Loads the config file
	*
	* @param [in] The string contatining the file path
	* @return [out] if loading succeeds then true, if it fails return false
	*/
	bool load( std::string filename );

	/**
	* Loads the config file
	*
	* @param [in] The string contatining "key" for the value wanted
	* @param [in] The float to be set from the key
	* @return [out] if it succeeds then true, if it fails return false
	*/
	bool getFloat( std::string key, float* value );

	/**
	* Loads the config file
	*
	* @param [in] The string contatining "key" for the value wanted
	* @param [in] The float to be set from the key
	* @param [in] The defualt value if the "key" leads to no value
	*/
	void getInt( std::string key, int* value, int default_value );

	/**
	* Loads the config file
	*
	* @param [in] The string contatining "key" for the value wanted
	* @param [in] The float to be set from the key
	* @return [out] if it succeeds then true, if it fails return false
	*/
	bool getString( std::string key, std::string* value );

private:

	std::string trimWhitespace( const std::string& str );

	std::map< std::string, std::string > data_;

	std::string name;
};

}

#endif