/*
 * db_ops.c
 * ========
 * Implementazione di tutte le operazioni sul database SQLite tramite C API:
 *   utility, metadati, DDL, CRUD, prepared statements,
 *   transazioni, gestione BLOB, funzioni utente custom (SQL scalar), Backup API.
 */

#include "db_ops.h"

/* ================================================================== */
/*  UTILITY                                                           */
/* ================================================================== */

void sep(const char *titolo) {
    printf("\n");
    printf("+------------------------------------------+\n");
    printf("|  %s\n", titolo);
    printf("+------------------------------------------+\n");
}

/* Esegue una query semplice o termina con errore */
void query_or_die(sqlite3 *db, const char *sql) {
    char *errmsg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &errmsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERRORE SQL] %s\n", errmsg ? errmsg : "sconosciuto");
        sqlite3_free(errmsg);
        sqlite3_close(db);
        exit(EXIT_FAILURE);
    }
}

/* ================================================================== */
/*  METADATI CONNESSIONE                                              */
/* ================================================================== */

void mostra_metadati(sqlite3 *db) {
    sep("METADATI CONNESSIONE / SQLITE");
    printf("  SQLite Library Version : %s\n", sqlite3_libversion());
    printf("  SQLite Source ID       : %s\n", sqlite3_sourceid());
    printf("  SQLite Version Number  : %d\n", sqlite3_libversion_number());
    
    sqlite3_int64 total, highwater;
    sqlite3_status64(SQLITE_STATUS_MEMORY_USED, &total, &highwater, 0);
    printf("  Memory Used (Bytes)    : %lld\n", (long long)total);
}

/* ================================================================== */
/*  2. DDL – CREATE TABLE                                             */
/* ================================================================== */

void step02_ddl(sqlite3 *db) {
    sep("2. DDL – CREATE TABLE");

    const char *sql = 
        "CREATE TABLE IF NOT EXISTS " TBL " ("
        "  id     INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  nome   TEXT    NOT NULL,"
        "  eta    INTEGER NOT NULL,"
        "  email  TEXT,"
        "  avatar BLOB"
        ");";

    query_or_die(db, sql);
    printf("Tabella `" TBL "` creata (o già esistente).\n");
}

/* ================================================================== */
/*  3. INSERT con sqlite3_exec / escaping nativo                      */
/* ================================================================== */

void step03_insert_manual(sqlite3 *db) {
    sep("3. INSERT con sqlite3_mprintf (string escaping nativo)");

    const char *nomi[]  = { "Mario Rossi", "Luisa O'Brien", "Zhāng Wéi" };
    int         eta[]   = { 30, 25, 42 };
    const char *email[] = { "mario@example.com",
                            "luisa@example.com",
                            "zhang@example.com" };
    int n = 3;

    for (int i = 0; i < n; i++) {
        /* sqlite3_mprintf gestisce correttamente il quoting e l'escape dei singoli apici */
        char *sql = sqlite3_mprintf(
            "INSERT INTO " TBL " (nome, eta, email) VALUES (%Q, %d, %Q);",
            nomi[i], eta[i], email[i]
        );

        if (!sql) {
            fprintf(stderr, "[ERRORE] Out of memory in sqlite3_mprintf\n");
            exit(EXIT_FAILURE);
        }

        query_or_die(db, sql);
        sqlite3_free(sql);

        printf("  INSERT id=%lld  nome='%s'\n",
               (long long)sqlite3_last_insert_rowid(db), nomi[i]);
    }
}

/* ================================================================== */
/*  4. SELECT con sqlite3_prepare_v2 / step                           */
/* ================================================================== */

void step04_select(sqlite3 *db) {
    sep("4. SELECT con sqlite3_prepare_v2 e sqlite3_step");

    const char *sql = "SELECT id, nome, eta, email FROM " TBL " ORDER BY id;";
    sqlite3_stmt *stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
        DIE_STMT(stmt, "sqlite3_prepare_v2", db);

    printf("  %-4s  %-20s  %-4s  %s\n", "ID", "Nome", "Età", "Email");
    printf("  %-4s  %-20s  %-4s  %s\n", "----", "--------------------", "----", "-----------------------------");

    int row_count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        row_count++;
        int id          = sqlite3_column_int(stmt, 0);
        const unsigned char *nome  = sqlite3_column_text(stmt, 1);
        int eta         = sqlite3_column_int(stmt, 2);
        const unsigned char *email = sqlite3_column_text(stmt, 3);

        printf("  %-4d  %-20s  %-4d  %s\n",
               id,
               nome ? (const char *)nome : "NULL",
               eta,
               email ? (const char *)email : "NULL");
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "[ERRORE] sqlite3_step ha restituito codice %d\n", rc);
    }

    printf("\nRighe recuperate: %d\n", row_count);
    sqlite3_finalize(stmt);
}

/* ================================================================== */
/*  5. UPDATE                                                         */
/* ================================================================== */

void step05_update(sqlite3 *db) {
    sep("5. UPDATE");

    const char *sql = "UPDATE " TBL " SET eta = eta + 1 WHERE nome = 'Mario Rossi';";
    query_or_die(db, sql);

    printf("Righe modificate: %d\n", sqlite3_changes(db));
}

/* ================================================================== */
/*  6. DELETE                                                         */
/* ================================================================== */

void step06_delete(sqlite3 *db) {
    sep("6. DELETE");

    const char *sql = "DELETE FROM " TBL " WHERE email = 'zhang@example.com';";
    query_or_die(db, sql);

    printf("Righe eliminate: %d\n", sqlite3_changes(db));
}

/* ================================================================== */
/*  7. Prepared statement – INSERT parametrizzato                     */
/* ================================================================== */

void step07_prepared_insert(sqlite3 *db) {
    sep("7. Prepared Statement – INSERT parametrizzato");

    const char *sql = "INSERT INTO " TBL " (nome, eta, email) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        DIE_STMT(stmt, "sqlite3_prepare_v2", db);

    const char *nome  = "Giovanni Bianchi";
    int         eta    = 35;
    const char *email = "giovanni@example.com";

    /* Binding dei parametri (gli indici partono da 1) */
    sqlite3_bind_text(stmt, 1, nome, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt,  2, eta);
    sqlite3_bind_text(stmt, 3, email, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE)
        DIE_STMT(stmt, "sqlite3_step INSERT", db);

    printf("  INSERT (prepared) id=%lld  nome='%s'\n",
           (long long)sqlite3_last_insert_rowid(db), nome);

    sqlite3_finalize(stmt);
}

/* ================================================================== */
/*  8. Prepared statement – SELECT con parametri e bind               */
/* ================================================================== */

void step08_prepared_select(sqlite3 *db) {
    sep("8. Prepared Statement – SELECT con filtri parametrici");

    const char *sql = "SELECT id, nome, eta, email FROM " TBL " WHERE eta >= ? ORDER BY id;";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        DIE_STMT(stmt, "sqlite3_prepare_v2", db);

    int soglia_eta = 28;
    sqlite3_bind_int(stmt, 1, soglia_eta);

    printf("  (Filtro: età >= %d)\n\n", soglia_eta);
    printf("  %-4s  %-20s  %-4s  %s\n", "ID", "Nome", "Età", "Email");
    printf("  %-4s  %-20s  %-4s  %s\n", "----", "--------------------", "----", "-----------------------------");

    int rc;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id          = sqlite3_column_int(stmt, 0);
        const unsigned char *nome  = sqlite3_column_text(stmt, 1);
        int eta         = sqlite3_column_int(stmt, 2);
        const unsigned char *email = sqlite3_column_text(stmt, 3);

        printf("  %-4d  %-20s  %-4d  %s\n",
               id,
               nome ? (const char *)nome : "NULL",
               eta,
               email ? (const char *)email : "NULL");
    }

    sqlite3_finalize(stmt);
}

/* ================================================================== */
/*  9. Transazione – BEGIN / COMMIT / ROLLBACK                        */
/* ================================================================== */

void step09_transazione(sqlite3 *db) {
    sep("9. TRANSAZIONE (BEGIN, COMMIT, ROLLBACK)");

    /* Operazione confermata con COMMIT */
    query_or_die(db, "BEGIN TRANSACTION;");
    query_or_die(db, "UPDATE " TBL " SET email='mario.new@example.com' WHERE nome='Mario Rossi';");
    query_or_die(db, "COMMIT;");
    printf("  Transazione 1: COMMIT OK (Email Mario aggiornata)\n");

    /* Simulazione di ROLLBACK */
    query_or_die(db, "BEGIN TRANSACTION;");
    query_or_die(db, "UPDATE " TBL " SET nome='NOME_TEMPORANEO' WHERE id=1;");
    printf("  UPDATE eseguito in transazione (sarà annullato)...\n");
    query_or_die(db, "ROLLBACK;");
    printf("  Transazione 2: ROLLBACK OK – modifica annullata con successo.\n");
}

/* ================================================================== */
/*  10. Gestione BLOB (Streaming / Bind binario)                      */
/* ================================================================== */

void step10_blob_handling(sqlite3 *db) {
    sep("10. GESTIONE BLOB (Dati binari)");

    const char *sql = "UPDATE " TBL " SET avatar = ? WHERE id = 1;";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        DIE_STMT(stmt, "sqlite3_prepare_v2 BLOB", db);

    /* Dati binari fittizi (es. header immagine o byte grezzi) */
    unsigned char fake_image_bytes[] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x01, 0x02 };
    
    /* SQLITE_TRANSIENT dice a SQLite di fare una copia locale dei dati binari */
    sqlite3_bind_blob(stmt, 1, fake_image_bytes, sizeof(fake_image_bytes), SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE)
        DIE_STMT(stmt, "sqlite3_step BLOB", db);

    printf("  BLOB binario (%zu bytes) associato con successo all'utente ID=1.\n", sizeof(fake_image_bytes));
    sqlite3_finalize(stmt);

    /* Lettura e verifica del BLOB */
    const char *sel_sql = "SELECT avatar FROM " TBL " WHERE id = 1;";
    if (sqlite3_prepare_v2(db, sel_sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const void *blob_ptr = sqlite3_column_blob(stmt, 0);
            int blob_bytes = sqlite3_column_bytes(stmt, 0);
            printf("  Verifica lettura BLOB: recuperati %d bytes dalla colonna avatar.\n", blob_bytes);
            if (blob_bytes > 0 && blob_ptr) {
                printf("  Prime 4 bytes (hex): 0x%02X 0x%02X 0x%02X 0x%02X\n",
                       ((unsigned char*)blob_ptr)[0],
                       ((unsigned char*)blob_ptr)[1],
                       ((unsigned char*)blob_ptr)[2],
                       ((unsigned char*)blob_ptr)[3]);
            }
        }
        sqlite3_finalize(stmt);
    }
}

/* ================================================================== */
/*  11. Funzione SQL Custom personalizzata (scalar function)          */
/* ================================================================== */

/* Funzione C che implementa la logica personalizzata richiamabile via SQL: pw_len(testo) */
static void sql_string_length_func(sqlite3_context *context, int argc, sqlite3_value **argv) {
    if (argc < 1) return;
    const unsigned char *text = sqlite3_value_text(argv[0]);
    if (!text) {
        sqlite3_result_int(context, 0);
        return;
    }
    int len = (int)strlen((const char *)text);
    sqlite3_result_int(context, len);
}

void step11_custom_function(sqlite3 *db) {
    sep("11. FUNZIONE SQL CUSTOM (sqlite3_create_function)");

    /* Registrazione della funzione SQL "STRLEN_CUSTOM" */
    int rc = sqlite3_create_function(
        db,
        "STRLEN_CUSTOM",
        1, /* 1 argomento */
        SQLITE_UTF8,
        NULL,
        sql_string_length_func,
        NULL,
        NULL
    );

    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERRORE] Impossibile registrare la funzione custom\n");
        return;
    }

    printf("  Funzione custom `STRLEN_CUSTOM(string)` registrata con successo.\n");
    printf("  Esecuzione query SQL con funzione custom...\n\n");

    const char *sql = "SELECT nome, STRLEN_CUSTOM(nome) AS len_nome FROM " TBL " ORDER BY id;";
    sqlite3_stmt *stmt = NULL;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        printf("  %-20s  %s\n", "Nome", "Lunghezza (Custom)");
        printf("  %-20s  %s\n", "--------------------", "------------------");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *nome = (const char *)sqlite3_column_text(stmt, 0);
            int len = sqlite3_column_int(stmt, 1);
            printf("  %-20s  %d\n", nome ? nome : "NULL", len);
        }
        sqlite3_finalize(stmt);
    }
}

/* ================================================================== */
/*  12. SQLite Backup API                                             */
/* ================================================================== */

void step12_backup_api(sqlite3 *db) {
    sep("12. BACKUP API (Backup a caldo su file di riserva)");

    sqlite3 *pDest = NULL;
    int rc = sqlite3_open("demo_sqlite_backup.db", &pDest);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "[ERRORE] Apertura database di backup fallita: %s\n", sqlite3_errmsg(pDest));
        if (pDest) sqlite3_close(pDest);
        return;
    }

    /* Inizializzazione della Backup API */
    sqlite3_backup *pBackup = sqlite3_backup_init(pDest, "main", db, "main");
    if (!pBackup) {
        fprintf(stderr, "[ERRORE] Inizializzazione SQLite Backup API fallita\n");
        sqlite3_close(pDest);
        return;
    }

    /* Esecuzione del backup in un unico blocco (o a passi) */
    rc = sqlite3_backup_step(pBackup, -1);
    if (rc == SQLITE_DONE) {
        printf("  Backup completato con successo su `demo_sqlite_backup.db`.\n");
    } else {
        fprintf(stderr, "[ERRORE] Backup fallito durante lo step: codice %d\n", rc);
    }

    sqlite3_backup_finish(pBackup);
    sqlite3_close(pDest);
}

/* ================================================================== */
/*  13. CLEANUP – DROP TABLE                                          */
/* ================================================================== */

void step13_cleanup(sqlite3 *db) {
    sep("13. CLEANUP – DROP TABLE");

    query_or_die(db, "DROP TABLE IF EXISTS " TBL ";");
    printf("  Tabella `" TBL "` eliminata correttamente.\n");
}
