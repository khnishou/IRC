
#include "../include/irc.hpp"

// int main(void) {
//     try {
//         Client validClient("nickname", "hostRealName", "hostUsername", "connectedServer");
//         std::cout << "Valid client created with nickname: " << validClient.getNickname() << std::endl;

//         Client invalidClient("$invalid", "hostRealName", "hostUsername", "connectedServer");

//         std::cout << "This line should not be printed." << std::endl;
//     } catch (const Client::InvalidNickname& e) {
//         std::cerr << RED << "Error: " << DEFAULT << e.what() << std::endl;
//     } catch (const std::exception& e) {
//         std::cerr << RED << "Unhandled exception: " << DEFAULT << e.what() << std::endl;
//     }
//     return 0;
// }