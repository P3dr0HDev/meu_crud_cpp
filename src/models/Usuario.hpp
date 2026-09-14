//
// Created by pedro on 13/09/2026.
//
#pragma once
#include <string>
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Usuario {
    public:
    int id;
    std::string nome;
    std::string email;

    Usuario() = default;

    Usuario(int id, std::string nome, std::string email)
        : id(id), nome(std::move(nome)), email(std::move(email)) {}

    static Usuario fromRow(const pqxx::row &row) {
        return Usuario(
            row["id"].as<int>(),
            row["nome"].as<std::string>(),
            row["email"].as<std::string>()
        );
    }

    json toJson() const {
        return json{
            {"id", id},
            {"nome", nome},
            {"email", email}
        };
    }

    static Usuario fromJson(const json &j) {
        Usuario u;
        u.nome = j.at("nome").get<std::string>();
        u.email = j.at("email").get<std::string>();
        return u;
    }
};