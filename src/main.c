/*
 * main.c
 * ======================================================
 * Esempio d'uso COMPLETO e avanzato di SQLite C API (libsqlite3)
 *
 * Copertura:
 *   1.  Connessione e Apertura Database (file locale o in-memory)
 *   2.  DDL        – CREATE TABLE
 *   3.  INSERT     – sqlite3_mprintf con escaping nativo
 *   4.  SELECT     – sqlite3_prepare_v2 / sqlite3_step
 *   5.  UPDATE
 *   6.  DELETE
 *   7.  Prepared statement – INSERT parametrizzato
 *   8.  Prepared statement – SELECT con parametri
 *   9.  Transazione – BEGIN / COMMIT / ROLLBACK
 *  10.  Gestione BLOB binari e streaming di byte
 *  11.  Custom SQL Scalar Function (sqlite3_create_function)
 *  12.  Backup API (Copia a caldo di un database)
 *  13.  Cleanup   – DROP TABLE e chiusura risorse
 * ======================================================
 */

#include "db_ops.h"

/* ================================================================== */
/*  1. CONNESSIONE                                                    */
/* ================================================================== */

static sqlite3 *step01_connetti(void) {
    sep("1. CONNESSIONE E APERTURA DATABASE");

    sqlite3 *db = NULL;
    int rc = sqlite3_open(DB_NAME, &db);
    
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERRORE] Impossibile aprire il database SQLite: %s\n", sqlite3_errmsg(db));
        if (db) sqlite3_close(db);
        exit(EXIT_FAILURE);
    }

    printf("Connesso/Creato database SQLite: %s\n", DB_NAME);

    /* Abilitazione delle foreign keys e PRAGMA utili per performance/robustezza */
    char *errmsg = NULL;
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", 0, 0, &errmsg);
    sqlite3_exec(db, "PRAGMA journal_mode = WAL;", 0, 0, &errmsg); /* Write-Ahead Logging */
    if (errmsg) {
        sqlite3_free(errmsg);
    }

    return db;
}

/* ================================================================== */
/*  ENTRY POINT                                                       */
/* ================================================================== */

int main(void) {
    printf("\n");
    printf("+------------------------------------------+\n");
    printf("|  SQLite C API - Demo Completa Avanzata   |\n");
    printf("+------------------------------------------+\n");

    sqlite3 *db = step01_connetti();
    
    mostra_metadati(db);

    step02_ddl(db);
    step03_insert_manual(db);
    step04_select(db);
    step05_update(db);
    step06_delete(db);
    step07_prepared_insert(db);
    step08_prepared_select(db);
    step09_transazione(db);
    step10_blob_handling(db);
    step11_custom_function(db);
    step12_backup_api(db);
    step13_cleanup(db);

    printf("\n");
    printf("+------------------------------------------+\n");
    printf("|  Demo SQLite completato con successo.    |\n");
    printf("+------------------------------------------+\n");

    sqlite3_close(db);
    return EXIT_SUCCESS;
}
