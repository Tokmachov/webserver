#include "PathComponents.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

PathComponents::PathComponents
(
	const std::string &req_target, 
	const std::string &root, 
	const std::string &index_page_name, 
	size_t location_len
) :
	_index_page_name(index_page_name)
{
	std::string req_target_with_no_location = _subtractLocationFromReqTarget(req_target, location_len);
	_path = _formPath(root, req_target_with_no_location);
	_pathType = _getPathType(_path);
}

PathComponents::PathComponents()
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

PathComponents::~PathComponents()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/
std::string PathComponents::getCurrentPath()
{
	return _path;
}

std::string PathComponents::getPathToIndexFile()
{
	if (_pathType == NON_EXISTING_FILE || _pathType == PATH_TO_FILE)
	{
		size_t last_slash_pos = _path.rfind("/");
		size_t file_name_len = _path.length() - last_slash_pos;
		_path.replace(last_slash_pos, file_name_len, "");
	}
	if (_path[_path.length() - 1] == '/')
		_path.erase(_path.length() - 1, 1);
	if (_index_page_name[0] == '/')
		_index_page_name.erase(0, 1);
	return _path + std::string("/") + _index_page_name;
}

PathComponents::PathType PathComponents::getPathType()
{
	return _pathType;
}


/*
** --------------------------------- PTIVATE_METHODS ---------------------------
*/
std::string PathComponents::_subtractLocationFromReqTarget
(
		const std::string &req_target,
		size_t location_len
) const
{
	std::string req_target_cpy = req_target;
	return req_target_cpy.replace(0, location_len, "");
}

std::string PathComponents::_formPath(std::string root, std::string req_target_with_no_location) const
{
	if (root[root.length() - 1] == '/')
		root.erase(root.length() - 1, 1);
	if (req_target_with_no_location[0] == '/')
		req_target_with_no_location.erase(0, 1);
	return root + std::string("/") + req_target_with_no_location;
}

PathComponents::PathType PathComponents::_getPathType(const std::string &path)
{
	int result;
	struct stat path_stat;

	result = stat(path.c_str(), &path_stat);
	if (result != 0)
	{
		if (path[path.length() - 1] == '/')
			return NON_EXISTING_DIR;	
		else
			return NON_EXISTING_FILE;
	}
	else
	{
		if (S_ISREG(path_stat.st_mode))
			return PATH_TO_FILE;
		else
			return DIR;
	}
}



/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */