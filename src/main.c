#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include "sqlite3.h"

// 1. Registrar evento con fecha actual
void registrarEvento(sqlite3 *db, const char *descripcion) {
    time_t ahora = time(NULL);
    struct tm *tm_info = localtime(&ahora);
    char fecha[20];
    strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tm_info);

    const char *sql = "INSERT INTO eventos (fecha, descripcion) VALUES (?, ?);";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, fecha, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, descripcion, -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            printf("✅ Evento registrado: %s\n", descripcion);
        } else {
            fprintf(stderr, "❌ Error al insertar evento: %s\n", sqlite3_errmsg(db));
        }
        sqlite3_finalize(stmt);
    }
}

// 2. Mostrar todos los eventos
void mostrarEventos(sqlite3 *db) {
    const char *sql = "SELECT id, fecha, descripcion FROM eventos;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        printf("\n📋 Eventos registrados:\n");
        printf("------------------------------------------------------------\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *fecha = sqlite3_column_text(stmt, 1);
            const unsigned char *descripcion = sqlite3_column_text(stmt, 2);
            printf("%d | %s | %s\n", id, fecha, descripcion);
        }
        printf("------------------------------------------------------------\n");
        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "❌ Error al consultar eventos: %s\n", sqlite3_errmsg(db));
    }
}

// 3. Filtrar eventos por texto
void filtrarEventosPorTexto(sqlite3 *db, const char *filtro) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, fecha, descripcion FROM eventos WHERE descripcion LIKE ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        char patron[100];
        snprintf(patron, sizeof(patron), "%%%s%%", filtro);
        sqlite3_bind_text(stmt, 1, patron, -1, SQLITE_STATIC);

        printf("\n🔍 Eventos que contienen \"%s\":\n", filtro);
        printf("------------------------------------------------------------\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *fecha = sqlite3_column_text(stmt, 1);
            const unsigned char *descripcion = sqlite3_column_text(stmt, 2);
            printf("%d | %s | %s\n", id, fecha, descripcion);
        }
        printf("------------------------------------------------------------\n");
        sqlite3_finalize(stmt);
    } else {
        fprintf(stderr, "❌ Error al filtrar eventos: %s\n", sqlite3_errmsg(db));
    }
}

// 4. Exportar eventos a CSV
void exportarEventosCSV(sqlite3 *db, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "w");
    if (!archivo) {
        perror("❌ No se pudo crear el archivo CSV");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, fecha, descripcion FROM eventos;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        fprintf(archivo, "id,fecha,descripcion\n");
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char *fecha = sqlite3_column_text(stmt, 1);
            const unsigned char *descripcion = sqlite3_column_text(stmt, 2);
            fprintf(archivo, "%d,%s,\"%s\"\n", id, fecha, descripcion);
        }
        sqlite3_finalize(stmt);
        printf("📁 Eventos exportados a %s\n", nombreArchivo);
    } else {
        fprintf(stderr, "❌ Error al exportar eventos: %s\n", sqlite3_errmsg(db));
    }

    fclose(archivo);
}

// 5. Registrar evento desde la terminal
void registrarEventoInteractivo(sqlite3 *db) {
    char descripcion[256];
    printf("\n📝 Escribe la descripción del evento: ");
    fgets(descripcion, sizeof(descripcion), stdin);
    descripcion[strcspn(descripcion, "\n")] = 0;
    registrarEvento(db, descripcion);
}

// Función principal
int main() {
    setlocale(LC_ALL, "");
    sqlite3 *db;

    if (sqlite3_open("eventos.db", &db) != SQLITE_OK) {
        fprintf(stderr, "❌ No se pudo abrir la base de datos: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    const char *sqlCrearTabla =
        "CREATE TABLE IF NOT EXISTS eventos ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "fecha TEXT NOT NULL,"
        "descripcion TEXT NOT NULL);";
    sqlite3_exec(db, sqlCrearTabla, 0, 0, 0);

    // 🔁 Aquí puedes probar cada función
    registrarEventoInteractivo(db);              // 5
    mostrarEventos(db);                          // 2
    filtrarEventosPorTexto(db, "Cámara");        // 3
    exportarEventosCSV(db, "eventos.csv");       // 4

    sqlite3_close(db);
    return 0;
}