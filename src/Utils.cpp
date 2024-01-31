#include "ircserver.hpp"


bool is_channel_char(char c) {
	return isalnum(c) || string("!#$%'()+,-./").find(c) != string::npos;
}

bool is_channel(const string& name) {
	for (const auto& c : name) {
		if (!is_channel_char(c))
			return false;
	}
	return !name.empty() && name.front() == '#';
}

bool is_nickname_character(const char c) {
	string valid = "_-[]\\^{}";
	if (isalnum(c) || valid.find(c) != string::npos)
		return true;
	return false;
}

bool verify_name(const string& arg) {
	for (string::const_iterator it = arg.begin(); it != arg.end(); ++it) {
		if (!is_nickname_character(*it))
			return false;
	}
	return true;
}

bool verify_realname(const string& arg) {
	for (string::const_iterator it = arg.begin(); it != arg.end(); ++it) {
		if (!is_nickname_character(*it) && !isspace(*it))
			return false;
	}
	return true;
}

// Takes a comma-separated string of arguments, gives back a vector of said arguments.
vector<string> split_args(const string& args) {
	istringstream nameStream(args);
	vector<string> res;

	string buffer;
	while (getline(nameStream, buffer, ',')) {
		res.push_back(buffer);
	}

	return res;
}

//case insensitive string comparison
bool compare_lowercase(const string& a, const string& b) {
	string ac = a;
	string bc = b;

	transform(ac.begin(), ac.end(), ac.begin(), [](unsigned char c) {
		return tolower(c);
	});

	transform(bc.begin(), bc.end(), bc.begin(), [](unsigned char c) {
		return tolower(c);
	});

	return ac == bc;
}