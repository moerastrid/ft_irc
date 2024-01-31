/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ircserver.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/01/31 14:56:22 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 14:56:23 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
	using std::string;
#include <vector>
	using std::vector;
#include <algorithm>
	using std::reverse;

#include "Msg.hpp"
#include "Server.hpp"
#include "Executor.hpp"
#include "Env.hpp"
#include "SignalHandler.hpp"

class Executor; // Forward declaration

int				parse(int argc, char **argv);

bool			is_channel(const string& name);
bool			verify_name(const string& arg);
//bool			verify_realname(const string& arg);
vector<string>	split_args(const string& args);
bool			compare_lowercase(const string& a, const string& b);