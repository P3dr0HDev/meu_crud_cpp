#include "crow.h"
#include <nlohmann/json.hpp>
#include "models/Usuario.hpp"
#include "repositories/UsuarioRepository.hpp"

using json = nlohmann::json;

const std::string CONN_STRING =
    "host=localhost port=5432 dbname=meubanco user=meuuser password=minhasenha";

int main() {
    crow::SimpleApp app;
    UsuarioRepository repo(CONN_STRING);

    CROW_ROUTE(app, "/")([]() {
        return "API C++ no ar!\n";
    });

    CROW_ROUTE(app, "/usuarios").methods("GET"_method)([&repo]() {
        try {
            auto usuarios = repo.findAll();
            json resposta = json::array();
            for (const auto &u : usuarios) {
                resposta.push_back(u.toJson());
            }
            return crow::response(200, resposta.dump());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/usuarios/<int>").methods("GET"_method)([&repo](int id) {
        try {
            auto usuario = repo.findById(id);
            if (!usuario) {
                return crow::response(404, "Usuário não encontrado");
            }
            return crow::response(200, usuario->toJson().dump());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/usuarios").methods("POST"_method)([&repo](const crow::request &req) {
        try {
            Usuario u = Usuario::fromJson(json::parse(req.body));
            Usuario criado = repo.save(u);
            return crow::response(201, criado.toJson().dump());
        } catch (const json::exception &e) {
            return crow::response(400, std::string("JSON inválido: ") + e.what());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/usuarios/<int>").methods("PUT"_method)([&repo](const crow::request &req, int id) {
        try {
            Usuario u = Usuario::fromJson(json::parse(req.body));
            auto atualizado = repo.update(id, u);
            if (!atualizado) {
                return crow::response(404, "Usuário não encontrado");
            }
            return crow::response(200, atualizado->toJson().dump());
        } catch (const json::exception &e) {
            return crow::response(400, std::string("JSON inválido: ") + e.what());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    CROW_ROUTE(app, "/usuarios/<int>").methods("DELETE"_method)([&repo](int id) {
        try {
            bool removido = repo.remove(id);
            if (!removido) {
                return crow::response(404, "Usuário não encontrado");
            }
            return crow::response(200, "Usuário removido com sucesso");
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    app.port(8080).multithreaded().run();
}