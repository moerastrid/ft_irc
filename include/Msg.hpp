/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Msg.hpp                                            :+:    :+:            */
/*                                                     +:+                    */
/*   By: ageels <ageels@student.codam.nl>             +#+                     */
/*       tnuyten <tnuyten@student.codam.nl>			 +#+                      */
/*   Created: 2024/01/31 14:57:37 by ageels        #+#    #+#                 */
/*   Updated: 2024/01/31 19:26:48 by ageels        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
	using std::cout;
	using std::endl;
#include <string>
	using std::string;
	using std::next;
	using std::find;
#include <algorithm>
	using std::replace;

class Msg {
	private	:
		void	structure(const string text);
		void	customStream(const string text);
		void	debug(const string text);
		void	info(const string text);
		void	warning(const string text);
		void	error(const string text);

		Msg();								
		Msg(const Msg &src);				
		Msg	&operator=(const Msg &src);		
		typedef void (Msg::*Msgptr)(string);

	public	:
		~Msg();								
		Msg(const string text, const string level);
};
