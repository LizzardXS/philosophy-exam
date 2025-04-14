#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

// ==== Прототипы ====
int open_db(const char *filename, sqlite3 **db);
void create_table_if_not_exists(sqlite3 *db);
void create_photo_table_if_not_exists(sqlite3 *db);
void insert_patient(sqlite3 *db);
void insert_with_transaction(sqlite3 *db);
void select_by_id(sqlite3 *db);
void search_by_lastname(sqlite3 *db);
void filter_by_department(sqlite3 *db);
void export_photo(sqlite3 *db);
void insert_photo(sqlite3 *db);
void menu(sqlite3 *db);

// ==== Открытие базы данных ====
int open_db(const char *filename, sqlite3 **db) {
    int rc = sqlite3_open(filename, db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Ошибка открытия базы: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    return SQLITE_OK;
}

// ==== Создание таблиц ====
void create_table_if_not_exists(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS Patience ("
        "id INTEGER PRIMARY KEY, фамилия TEXT, имя TEXT, отчество TEXT, пол TEXT, "
        "национальность TEXT, рост REAL, вес REAL, \"дата рождения\" INTEGER, "
        "телефон TEXT, адрес TEXT, \"номер медкарты\" TEXT, диагноз TEXT, "
        "\"группа крови\" TEXT, отделение TEXT, hospital_id INTEGER);";
    char *errmsg = 0;
    sqlite3_exec(db, sql, 0, 0, &errmsg);
}

void create_photo_table_if_not_exists(sqlite3 *db) {
    const char *sql = "CREATE TABLE IF NOT EXISTS Photo ("
                      "patient_id INTEGER PRIMARY KEY, "
                      "photo BLOB, "
                      "FOREIGN KEY(patient_id) REFERENCES Patience(id));";
    char *errmsg = 0;
    sqlite3_exec(db, sql, 0, 0, &errmsg);
}

// ==== Вставка пациента ====
void insert_patient(sqlite3 *db) {
    const char *sql = "INSERT INTO Patience (id, фамилия, имя, отчество, пол, национальность, рост, вес, \"дата рождения\", телефон, адрес, \"номер медкарты\", диагноз, \"группа крови\", отделение, hospital_id) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    int id, birth_date, hospital_id;
    float height, weight;
    char last_name[50], first_name[50], patronymic[50], gender[10], nationality[50];
    char phone[20], address[100], card_number[20], diagnosis[100], blood_group[10], department[50];

    while (getchar() != '\n');
    printf("ID: "); scanf("%d", &id); while (getchar() != '\n');
    printf("Фамилия: "); fgets(last_name, sizeof(last_name), stdin);
    printf("Имя: "); fgets(first_name, sizeof(first_name), stdin);
    printf("Отчество: "); fgets(patronymic, sizeof(patronymic), stdin);
    printf("Пол: "); fgets(gender, sizeof(gender), stdin);
    printf("Национальность: "); fgets(nationality, sizeof(nationality), stdin);
    printf("Рост: "); scanf("%f", &height);
    printf("Вес: "); scanf("%f", &weight);
    printf("Дата рождения (ГГГГММДД): "); scanf("%d", &birth_date); while (getchar() != '\n');
    printf("Телефон: "); fgets(phone, sizeof(phone), stdin);
    printf("Адрес: "); fgets(address, sizeof(address), stdin);
    printf("Номер медкарты: "); fgets(card_number, sizeof(card_number), stdin);
    printf("Диагноз: "); fgets(diagnosis, sizeof(diagnosis), stdin);
    printf("Группа крови: "); fgets(blood_group, sizeof(blood_group), stdin);
    printf("Отделение: "); fgets(department, sizeof(department), stdin);
    printf("Hospital ID: "); scanf("%d", &hospital_id);

    #define TRIM(s) s[strcspn(s, "\n")] = 0
    TRIM(last_name); TRIM(first_name); TRIM(patronymic); TRIM(gender); TRIM(nationality);
    TRIM(phone); TRIM(address); TRIM(card_number); TRIM(diagnosis);
    TRIM(blood_group); TRIM(department);


    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, last_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, first_name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, patronymic, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, gender, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, nationality, -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 7, height);
    sqlite3_bind_double(stmt, 8, weight);
    sqlite3_bind_int(stmt, 9, birth_date);
    sqlite3_bind_text(stmt, 10, phone, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 11, address, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 12, card_number, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 13, diagnosis, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 14, blood_group, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 15, department, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 16, hospital_id);

    if (sqlite3_step(stmt) == SQLITE_DONE)
        printf("Пациент добавлен.\n");
    else
        fprintf(stderr, "Ошибка вставки: %s\n", sqlite3_errmsg(db));

    sqlite3_finalize(stmt);
}

// ==== Транзакция ====
void insert_with_transaction(sqlite3 *db) {
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    insert_patient(db);
    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
}

// ==== Поиск по ID ====
void select_by_id(sqlite3 *db) {
    int id;
    printf("Введите ID пациента: ");
    scanf("%d", &id);

    const char *sql = "SELECT id, фамилия, имя, отделение FROM Patience WHERE id = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("ID: %d | ФИО: %s %s | Отделение: %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2),
               sqlite3_column_text(stmt, 3));
    } else {
        printf("Пациент не найден.\n");
    }

    sqlite3_finalize(stmt);
}

// ==== Поиск по фамилии ====
void search_by_lastname(sqlite3 *db) {
    char pattern[50];
    printf("Введите часть фамилии: ");
    scanf("%s", pattern);

    char like_pattern[60];
    snprintf(like_pattern, sizeof(like_pattern), "%%%s%%", pattern);

    const char *sql = "SELECT id, фамилия, имя FROM Patience WHERE фамилия LIKE ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, like_pattern, -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%d | %s %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2));
    }

    sqlite3_finalize(stmt);
}

// ==== Фильтр по отделению ====
void filter_by_department(sqlite3 *db) {
    char dept[50];
    printf("Введите название отделения: ");
    scanf("%s", dept);

    const char *sql = "SELECT id, фамилия, имя FROM Patience WHERE отделение = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, dept, -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        printf("%d | %s %s\n",
               sqlite3_column_int(stmt, 0),
               sqlite3_column_text(stmt, 1),
               sqlite3_column_text(stmt, 2));
    }

    sqlite3_finalize(stmt);
}

// ==== Экспорт фото ====
void export_photo(sqlite3 *db) {
    int id;
    printf("Введите ID пациента для экспорта фото: ");
    scanf("%d", &id);

    const char *sql = "SELECT photo FROM Photo WHERE patient_id = ?;";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const void *blob = sqlite3_column_blob(stmt, 0);
        int size = sqlite3_column_bytes(stmt, 0);


        FILE *file = fopen("out/photo_out.png", "wb");
        if (file) {
            fwrite(blob, 1, size, file);
            fclose(file);
            printf("Фото сохранено в out/photo_out.png\n");
        } else {
            printf("Ошибка записи файла.\n");
        }
    } else {
        printf("Фото не найдено.\n");
    }

    sqlite3_finalize(stmt);
}

// ==== Вставка или обновление фото ====
void insert_photo(sqlite3 *db) {
    int patient_id;
    char path[256];
    printf("Введите ID пациента, которому нужно добавить фото: ");
    scanf("%d", &patient_id);
    printf("Введите путь к изображению (например, images/photo1.png): ");
    scanf("%s", path);

    FILE *file = fopen(path, "rb");
    if (!file) {
        printf("Ошибка: не удалось открыть файл %s\n", path);
        return;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    unsigned char *data = malloc(size);
    fread(data, 1, size, file);
    fclose(file);

    const char *sql = "INSERT OR REPLACE INTO Photo (patient_id, photo) VALUES (?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    sqlite3_bind_int(stmt, 1, patient_id);
    sqlite3_bind_blob(stmt, 2, data, size, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE)
        printf("Фото успешно добавлено/обновлено.\n");
    else
        fprintf(stderr, "Ошибка вставки фото: %s\n", sqlite3_errmsg(db));

    sqlite3_finalize(stmt);
    free(data);
}

// ==== Меню ====
void menu(sqlite3 *db) {
    int choice;
    while (1) {
        printf("\n==== МЕНЮ ====\n");
        printf("1. Вставить пациента (autocommit)\n");
        printf("2. Вставить пациента (транзакция)\n");
        printf("3. Поиск по ID\n");
        printf("4. Поиск по фамилии (LIKE)\n");
        printf("5. Фильтр по отделению\n");
        printf("6. Сохранить фото пациента\n");
        printf("7. Добавить/обновить фото пациента\n");
        printf("0. Выход\nВыбор: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: insert_patient(db); break;
            case 2: insert_with_transaction(db); break;
            case 3: select_by_id(db); break;
            case 4: search_by_lastname(db); break;
            case 5: filter_by_department(db); break;
            case 6: export_photo(db); break;
            case 7: insert_photo(db); break;
            case 0: return;
            default: printf("Неверный выбор\n");
        }
    }
}

// ==== main ====
int main() {
    sqlite3 *db;
    if (open_db("/Users/elizavetasenuk/Desktop/Patience.db", &db) != SQLITE_OK)
        return 1;

    create_table_if_not_exists(db);
    create_photo_table_if_not_exists(db);
    menu(db);
    sqlite3_close(db);
    return 0;
}

