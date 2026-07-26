/*
 * db_ops.h
 * ========
 * Header condiviso per SQLite C API Demo: parametri, macro di errore,
 * e dichiarazioni di tutte le funzioni di db_ops.c (DDL, CRUD, Prepared Statements,
 * Transazioni, Backup, Custom Functions, ecc.)
 */

#ifndef DB_OPS_H
#define DB_OPS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

/* Nome del database SQLite utilizzato nella demo (file locale) */
#define DB_NAME     "demo_sqlite.db"

/* Nome tabella utilizzata nel demo */
#define TBL "demo_persone"

/* Macro per stampare errore e uscire */
#define DIE(db, msg)                                                \
    do {                                                            \
        fprintf(stderr, "[ERRORE] %s: %s\n",                        \
                (msg), sqlite3_errmsg(db));                         \
        sqlite3_close(db);                                          \
        exit(EXIT_FAILURE);                                         \
    } while (0)

#define DIE_STMT(stmt, msg, db)                                     \
    do {                                                            \
        fprintf(stderr, "[ERRORE STMT] %s: %s\n",                   \
                (msg), sqlite3_errmsg(db));                         \
        sqlite3_finalize(stmt);                                     \
        sqlite3_close(db);                                          \
        exit(EXIT_FAILURE);                                         \
    } while (0)

/* ------------------------------------------------------------------ */
/*  Dichiarazioni – utility                                           */
/* ------------------------------------------------------------------ */
void sep(const char *titolo);
void query_or_die(sqlite3 *db, const char *sql);

/* ------------------------------------------------------------------ */
/*  Dichiarazioni – step                                              */
/* ------------------------------------------------------------------ */
void mostra_metadati(sqlite3 *db);

void step02_ddl(sqlite3 *db);
void step03_insert_manual(sqlite3 *db);
void step04_select(sqlite3 *db);
void step05_update(sqlite3 *db);
void step06_delete(sqlite3 *db);
void step07_prepared_insert(sqlite3 *db);
void step08_prepared_select(sqlite3 *db);
void step09_transazione(sqlite3 *db);
void step10_blob_handling(sqlite3 *db);
void step11_custom_function(sqlite3 *db);
void step12_backup_api(sqlite3 *db);
void step13_cleanup(sqlite3 *db);

#endif /* DB_OPS_H */
