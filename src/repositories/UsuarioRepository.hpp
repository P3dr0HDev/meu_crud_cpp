//
// Created by pedro on 16/09/2026.
//

#pragma once
#include <pqxx/pqxx>
#include <vector>
#include <optional>
#include "../models/Usuario.hpp"

class UsuarioRepository {
private: std::string connString;

public:
    explicit UsuarioRepository(std::string connString)
    : connString(std::move(connString)) {}

    std::vector<Usuario> findAll() {
        pqxx::connection conn(connString);
        pqxx::work txn(conn);
        pqxx::result r = txn.exec("SELECT id, nome, email FROM usuarios ORDER BY id");

        std::vector<Usuario> usuarios;
        for (auto row : r) {
            usuarios.push_back(Usuario::fromRow(row));
        }
        return usuarios;
    }

    std::optional<Usuario> findById(int id) {
        pqxx::connection conn(connString);
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(
            "SELECT id, nome, email FROM usuarios WHERE id = $1", id
        );

        if (r.empty()) {
            return std::nullopt;
        }
        return Usuario::fromRow(r[0]);
    }

    Usuario save(const Usuario &usuario) {
        pqxx::connection conn(connString);
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(
            "INSERT INTO usuarios (nome, email) VALUES ($1, $2) RETURNING id",
            usuario.nome, usuario.email
        );
        txn.commit();

        Usuario criado = usuario;
        criado.id = r[0]["id"].as<int>();
        return criado;
    }

    std::optional<Usuario> update(int id, const Usuario &usuario) {
        pqxx::connection conn (connString);
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(
        "UPDATE usuarios SET nome = $1, email = $2 WHERE id = $3 RETURNING id",
        usuario.nome, usuario.email, id);

        if (r.empty()) {
            return std::nullopt;
        }
        txn.commit();

        Usuario atualizado = usuario;
        atualizado.id = id;
        return atualizado;
    }

    bool remove(int id) {
        pqxx::connection conn(connString);
        pqxx::work txn(conn);
        pqxx::result r = txn.exec_params(
            "DELETE FROM usuarios WHERE id = $1 RETURNING id", id
        );

        if (r.empty()) {
            return false;
        }
        txn.commit();
        return true;
    }
};