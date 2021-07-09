#include "utils.hpp"

std::vector<std::string>	split(std::string req, std::string delimetr)
{
	std::vector<std::string>	v;
	std::string					word;
	std::size_t					begin = 0;
	std::size_t					end = 0;
	std::size_t					found = 0;

	while ( (found = req.find(delimetr, begin)) != std::string::npos)
	{
		end = found;
		word = req.substr(begin, end - begin);
		if (word != "")
			v.push_back(word);
		begin = found + delimetr.length();
	}
	return (v);
}

void						printStringVector(std::vector<std::string>	v)
{
	std::vector<std::string>::iterator	it;

	for (it = v.begin(); it != v.end(); it++)
	{
		std::cout << *it << std::endl;
	}
}