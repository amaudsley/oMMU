//=========================================================================
// The following class defines a hash function for string objects.
// See http://stackoverflow.com/a/15811185/2347831
//=========================================================================
#include <string>
class stringhasher
{
public:
	// Returns hashcode for the supplied string
	size_t operator() (const std::string *key) const
	{
		size_t hash = 0;
		for (size_t i = 0; i < (*key).size(); i++)
		{
			hash += (71 * hash + (*key)[i]) % 5;
		}
		return hash;
	}

	// Compares two string objects for equality; returns true if string match
	bool operator() (const std::string *s1, const std::string *s2) const
	{
		return ((*s1).compare(*s2) == 0);
	}
};