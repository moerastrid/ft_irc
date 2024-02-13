/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   ircserver.hpp                                      :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 14:56:22 by ageels        #+#    #+#                 */
/*   Updated: 2024/02/01 15:46:02 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
	using std::string;
#include <vector>
	using std::vector;
#include <algorithm>
	using std::reverse;
#include <iostream>
	using std::cout;
	using std::cerr;
#include <unistd.h>

#include "Msg.hpp"
#include "Server.hpp"
#include "Executor.hpp"
#include "Env.hpp"
#include "SignalHandler.hpp"

class Executor; // Forward declaration

int				parse(int argc, char **argv);

bool			is_channel(const string& name);
bool 			is_name(const string &name);
vector<string>	split_args(const string& args);
bool			compare_lowercase(const string& a, const string& b);
bool			channel_is_empty(const Channel &value);