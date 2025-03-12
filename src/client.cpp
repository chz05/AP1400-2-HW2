#include "client.h"
#include "server.h"
#include "crypto.h"

#include<random>

Client::Client(std::string client_id, const Server& client_server) : id(client_id), server(&client_server) {
    crypto::generate_key(public_key, private_key);
}

std::string Client::get_id() const{
    return id;
}

std::string Client::get_publickey() const{
    return public_key;
}

double Client::get_wallet(){
    return server->get_wallet(id);
}

std::string Client::sign(std::string txt) const{
    return crypto::signMessage(private_key, txt);
}

bool Client::transfer_money(std::string receiver, double value){
    std::string transaction = id + "-" + receiver + "-" + std::to_string(value);
    std::string signature = sign(transaction);
    return server->add_pending_trx(transaction, signature);
}

size_t Client::generate_nonce(){
    std::random_device rd;  // Obtain a random number from hardware
    std::default_random_engine re(rd());  // Seed the random engine with the hardware-generated random number
    std::uniform_int_distribution<size_t> dist;  // No min and max specified, so it uses the default range

    return dist(re);  // Generate and return the random number
}



