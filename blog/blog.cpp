#include <eosio/eosio.hpp>
#include <string>

using namespace eosio;
using namespace std;

class [[eosio::contract("blog")]] blog : public eosio::contract {
    public:
        blog(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds) {}

        [[eosio::action]]
        void create(name author, string title, string content) {
            require_auth(author);

            post_index posts(get_self(), get_first_receiver().value);

            posts.emplace(author, [&](auto& row) {
                row.index = posts.available_primary_key();
                row.author = author;
                row.title = title;
                row.content = content;
            });
        }

        [[eosio::action]]
        void erase(uint64_t index) {
            post_index posts(get_self(), get_first_receiver().value);

            auto iterator = posts.find(index);

            check(iterator != posts.end(), "Record does not exist!");

            require_auth(iterator->author);

            posts.erase(iterator);
        }

        [[eosio::action]]
        void edit(uint64_t index, string title, string content) {
            post_index posts(get_self(), get_first_receiver().value);

            auto iterator = posts.find(index);

            check(iterator != posts.end(), "Record does not exist!");

            require_auth(iterator->author);

            posts.modify(iterator, iterator->author, [&](auto& row) {
                row.title = title;
                row.content = content;
            });
        }

    private:
        struct [[eosio::table]] post {
            uint64_t index;
            name author;
            string title;
            string content;

            uint64_t primary_key() const { return index; }
        };

        typedef eosio::multi_index<"posts"_n, post> post_index;
};