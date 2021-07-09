#ifndef PATHCOMPONENTS_HPP
# define PATHCOMPONENTS_HPP

# include <iostream>
# include <string>
# include <vector>
# include <sys/types.h> // stat call
# include <sys/stat.h> // stat call

struct PathComponents
{
	enum PathType
	{
		PATH_TO_FILE,
		DIR,
		NON_EXISTING_FILE,
		NON_EXISTING_DIR
	};
	PathComponents
	(
		const std::string &req_target, 
		const std::string &root, 
		const std::string &index_page_name, 
		size_t location_len
	);
	PathComponents();
	~PathComponents();
	//methods
	std::string getCurrentPath();
	std::string getPathToIndexFile();
	PathType getPathType();
	private:
		// vars
		std::string _path;
		std::string _index_page_name;
		PathType _pathType;
		//method
		std::string _subtractLocationFromReqTarget
		(
				const std::string &req_target,
			 	size_t location_len
		) const;
		std::string _formPath(std::string root,std::string wreq_target_with_no_location) const;
		PathType _getPathType(const std::string &path);
};

#endif /* ************************************************** PATHCOMPONENTS_H */