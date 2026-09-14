#include "crow.h"
#include <pqxx/pqxx>
#include <nlohmann/json.hpp>
#include "models/Usuario.hpp"

using json = nlohmann::json;

const std::string CONN_STRING =
    "host=localhost port=5432 dbname=meubanco user=meuuser password=minhasenha";

int main() {
    crow::SimpleApp app;

    // ---------- ROTA RAIZ ----------
    CROW_ROUTE(app, "/")([]() {
        return "API C++ no ar!";
    });

    // ---------- GET /usuarios (listar todos) ----------
    CROW_ROUTE(app, "/usuarios").methods("GET"_method)([]() {
        try {
            pqxx::connection conn(CONN_STRING);
            pqxx::work txn(conn);
            pqxx::result r = txn.exec("SELECT id, nome, email FROM usuarios ORDER BY id");

            json resposta = json::array();
            for (auto row : r) {
                Usuario u = Usuario::fromRow(row);
                resposta.push_back(u.toJson());
            }
            return crow::response(200, resposta.dump());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    // ---------- GET /usuarios/<id> (buscar por id) ----------
    CROW_ROUTE(app, "/usuarios/<int>").methods("GET"_method)([](int id) {
        try {
            pqxx::connection conn(CONN_STRING);
            pqxx::work txn(conn);
            pqxx::result r = txn.exec_params(
                "SELECT id, nome, email FROM usuarios WHERE id = $1", id
            );

            if (r.empty()) {
                return crow::response(404, "Usuário não encontrado");
            }

            Usuario u = Usuario::fromRow(r[0]);
            return crow::response(200, u.toJson().dump());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    // ---------- POST /usuarios (criar) ----------
    CROW_ROUTE(app, "/usuarios").methods("POST"_method)([](const crow::request &req) {
        try {
            Usuario u = Usuario::fromJson(json::parse(req.body));

            pqxx::connection conn(CONN_STRING);
            pqxx::work txn(conn);
            pqxx::result r = txn.exec_params(
                "INSERT INTO usuarios (nome, email) VALUES ($1, $2) RETURNING id",
                u.nome, u.email
            );
            txn.commit();

            u.id = r[0]["id"].as<int>();
            return crow::response(201, u.toJson().dump());
        } catch (const json::exception &e) {
            return crow::response(400, std::string("JSON inválido: ") + e.what());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    // ---------- PUT /usuarios/<id> (atualizar) ----------
    CROW_ROUTE(app, "/usuarios/<int>").methods("PUT"_method)([](const crow::request &req, int id) {
        try {
            Usuario u = Usuario::fromJson(json::parse(req.body));

            pqxx::connection conn(CONN_STRING);
            pqxx::work txn(conn);
            pqxx::result r = txn.exec_params(
                "UPDATE usuarios SET nome = $1, email = $2 WHERE id = $3 RETURNING id",
                u.nome, u.email, id
            );

            if (r.empty()) {
                return crow::response(404, "Usuário não encontrado");
            }
            txn.commit();

            u.id = id;
            return crow::response(200, u.toJson().dump());
        } catch (const json::exception &e) {
            return crow::response(400, std::string("JSON inválido: ") + e.what());
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    // ---------- DELETE /usuarios/<id> (remover) ----------
    CROW_ROUTE(app, "/usuarios/<int>").methods("DELETE"_method)([](int id) {
        try {
            pqxx::connection conn(CONN_STRING);
            pqxx::work txn(conn);
            pqxx::result r = txn.exec_params(
                "DELETE FROM usuarios WHERE id = $1 RETURNING id", id
            );

            if (r.empty()) {
                return crow::response(404, "Usuário não encontrado");
            }
            txn.commit();

            return crow::response(200, "Usuário removido com sucesso");
        } catch (const std::exception &e) {
            return crow::response(500, std::string("Erro: ") + e.what());
        }
    });

    app.port(8080).multithreaded().run();
}