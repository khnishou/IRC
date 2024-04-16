/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: smallem <smallem@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 14:44:42 by smallem           #+#    #+#             */
/*   Updated: 2024/04/16 15:17:26 by smallem          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Users.hpp"
#include "../include/Channel.hpp"

int main() {
	Server serv(6667, "pswd");

	while (serv.getState() == START) {
		serv.init();
		serv.start();
	}
}
