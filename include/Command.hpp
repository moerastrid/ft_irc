/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Command.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 13:29:42 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 13:29:43 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <sstream>
using std::stringstream;

#include <algorithm>
using std::find_if;

#include <cctype>
using std::isspace;

#include "Msg.hpp"

class Command {
	private:
		string			command;
		vector<string>	args;

		void	addArg(string arg);
		void	setCmd(string cmd);
		
	public:
		Command();
		~Command();
		Command(const Command& other);
		Command(const string& client_message_string);
		const Command& operator=(const Command& other);

		const string			getCommand() const;
		const vector<string>	getArgs() const;

		void					combine_reason();
};

bool find_reason(const string& str);
