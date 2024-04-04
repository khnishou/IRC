/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ykerdel <ykerdel@student.42heilbronn.de    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 14:44:42 by smallem           #+#    #+#             */
/*   Updated: 2024/04/04 19:26:45 by ykerdel          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Users.hpp"
#include "../include/Channel.hpp"

int main() {
	Server serv(6667, "zebi");

	serv.start();
}
