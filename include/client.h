#ifndef CLIENT_H
#define CLIENT_H

#include "server.h"
#include <vector>
class Server;

class Client
{
public:
	Client(std::string id, const Server& server);
	std::string get_id() const;
	std::string get_publickey() const;
	double get_wallet();
	std::string sign(std::string txt) const;
	bool transfer_money(std::string receiver, double value);
	size_t generate_nonce();
private:
	Server const* const server; // const* means it can not change the pointer that points to, means it can not points to different server, then const means it can not change the server itself.
	const std::string id;
	std::string public_key;
	std::string private_key;
};

#endif //CLIENT_H