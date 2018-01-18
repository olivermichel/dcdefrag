
#include "util.h"

#include <dirent.h>

std::vector<std::string> util::filenames_in_directory(const std::string& directory_name)
throw(std::runtime_error)
{
	DIR* dir;
	struct dirent* dir_entry;
	std::vector<std::string> file_names;

	if ((dir = opendir(directory_name.c_str())) == NULL)
		throw std::runtime_error("can't open directory " + directory_name);

	while ((dir_entry = readdir(dir)) != NULL)
		if (dir_entry->d_type == DT_REG)
			file_names.push_back(std::string(dir_entry->d_name));

	return file_names;
}
