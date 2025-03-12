#include "server.h"
#include "client.h"
#include <random>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>


std::vector<std::string> pending_trxs;


void show_wallets(const  Server& server)
{
	std::cout << std::string(20, '*') << std::endl;
	for(const auto& client: server.clients)
		std::cout << client.first->get_id() <<  " : "  << client.second << std::endl;
	std::cout << std::string(20, '*') << std::endl;
}


Server::Server() {}

int generateRandom4DigitNumber() {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Define the range for 4-digit numbers (1000 to 9999)
    std::uniform_int_distribution<> dis(1000, 9999);

    // Generate and return the random number
    return dis(gen);
}

std::shared_ptr<Client> Server::add_client(std::string id){
	for (const auto& c: clients){
		std::string client_id = c.first->get_id();
		if (client_id == id){
			id += std::to_string(generateRandom4DigitNumber());
		}
	}
	std::shared_ptr<Client> client = std::make_shared<Client>(id, *this);
	clients.insert({client, 5.0});
	return client;
}

std::shared_ptr<Client> Server::get_client(std::string id) const{
	for (const auto& c: clients){
		std::string client_id = c.first->get_id();
		if (client_id == id){
			return c.first;
		}
	}
	return nullptr;
}

double Server::get_wallet(std::string id) const{
	for (const auto& c: clients){
		std::string client_id = c.first->get_id();
		if (client_id == id){
			return c.second;
		}
	}
	throw std::logic_error("No client with this id");
}

bool Server::parse_trx(std::string trx, std::string& sender, std::string& receiver, double& value){
	size_t first = trx.find('-');
    size_t second = trx.find('-', first + 1);
    size_t third = trx.find('-', second + 1);
	if (first == std::string::npos || second == std::string::npos || third != std::string::npos) {
        throw std::runtime_error("Invalid format: expected a string with exactly two '-' delimiters.");
    }
	std::string part1 = trx.substr(0, first);
    std::string part2 = trx.substr(first + 1, second - first - 1);
    std::string part3 = trx.substr(second + 1);
	sender = part1;
	receiver = part2;
	value = std::stod(part3);
	return true;
}

bool Server::add_pending_trx(std::string trx, std::string signature) const{
	size_t first = trx.find('-');
    size_t second = trx.find('-', first + 1);
    size_t third = trx.find('-', second + 1);
	std::string sender = trx.substr(0, first);
    std::string receiver = trx.substr(first + 1, second - first - 1);
    std::string value = trx.substr(second + 1);
	double money = std::stod(value);
	double sender_money = get_wallet(sender);

	if (sender_money < money){
		return false;
	}
	std::shared_ptr<Client> sender_client = get_client(sender);
	bool authentic = crypto::verifySignature(sender_client->get_publickey(), trx, signature);
	if (!authentic){
		return false;
	}
	pending_trxs.push_back(trx);
	return true;
}

std::size_t Server::mine(){
	std::string transactions;
    for (size_t i = 0; i < pending_trxs.size(); ++i) {
        transactions += pending_trxs[i];
    }
	for (;;){
	for (auto& client: clients){
		std::string append_nounce_transactions = transactions;
		std::size_t random_number = client.first->generate_nonce();
		append_nounce_transactions = transactions + std::to_string(random_number);
		std::string final_string = crypto::sha256(append_nounce_transactions);
		std::string first_ten = final_string.substr(0, 10);
		if (first_ten.find("000") != std::string::npos) {
			std::cout << client.first->get_id() << std::endl;
			client.second += 6.5;
			for (int i = 0; i < pending_trxs.size(); i++){
				size_t first = pending_trxs[i].find('-');
				size_t second = pending_trxs[i].find('-', first + 1);
				size_t third = pending_trxs[i].find('-', second + 1);
				std::string sender = pending_trxs[i].substr(0, first);
				std::string receiver = pending_trxs[i].substr(first + 1, second - first - 1);
				std::string value = pending_trxs[i].substr(second + 1);
				double money = std::stod(value);
				for (auto &client_2: clients){
					if (client_2.first->get_id() == sender){
						client_2.second -= money;
					}
					if (client_2.first->get_id() == receiver){
						client_2.second += money;
					}
				}
			}
			pending_trxs.clear();
			return random_number;
		}
	}
}
}

