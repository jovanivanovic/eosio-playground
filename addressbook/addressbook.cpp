#include <eosio/eosio.hpp>
#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("addressbook")]] addressbook : public eosio::contract {
    public:
        addressbook(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds) {}

        [[eosio::action]]
        void upsert(name user, string first_name, string last_name, string street, string city, string state) {
            require_auth(user);

            address_index addresses(get_self(), get_first_receiver().value);

            auto iterator = addresses.find(user.value);

            if (iterator == addresses.end()) {
                addresses.emplace(user, [&](auto& row) {
                    row.key = user;
                    row.first_name = first_name;
                    row.last_name = last_name;
                    row.street = street;
                    row.city = city;
                    row.state = state;
                });
            } else {
                addresses.modify(iterator, user, [&](auto& row) {
                    row.key = user;
                    row.first_name = first_name;
                    row.last_name = last_name;
                    row.street = street;
                    row.city = city;
                    row.state = state;
                });
            }
        }

        [[eosio::action]]
        void erase(name user) {
            require_auth(user);

            address_index addresses(get_self(), get_first_receiver().value);

            auto iterator = addresses.find(user.value);

            check(iterator != addresses.end(), "Record does not exist!");

            addresses.erase(iterator);
        }

    private:
        struct [[eosio::table]] person {
            name key;
            string first_name;
            string last_name;
            string street;
            string city;
            string state;

            uint64_t primary_key() const { return key.value; }
        };

        typedef eosio::multi_index<"people"_n, person> address_index;
};