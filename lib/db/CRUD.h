//
// Created by katussska on 4/2/24.
//

#ifndef PROJECT_CRUD_H
#define PROJECT_CRUD_H

#include "../include/Transaction.h"

using DatabasePtr = std::unique_ptr<SQLite::Database>;
using StatementPtr = std::unique_ptr<SQLite::Statement>;

inline DatabasePtr db = std::make_unique<SQLite::Database>("../lib/db/database.db",
                                                           SQLite::OPEN_CREATE | SQLite::OPEN_READWRITE);

///CREATE
void createSchema();

void createCategory(const Category &category);

void createAccount(const Account &account);

void createTransaction(const std::string &type, int accountID, int categoryId, const Transaction &expense);

///UPDATE
void updateCategory(int id, const std::string &name, double budget);

void updateAccount(int id, const std::string &name, double balance, double moneyChange);

void updateTransaction(int id, int accountID, int categoryId, double amount, const std::string &description,
                       const std::string &date);

///READ
void readAllCategoriesWithExpenses();

void readTransactionsByCategoryId(int id);

void readTransactionsByCategoryName(const std::string &name);

void readAllAccountsWithBalance();

void readAccountDetailsById(int id);

void readAccountDetailsByName(const std::string &name);

void readTransactionsByType(const std::string &type);

///DELETE
void destroyAccount(int id);

void destroyTransaction(int id);

void destroyCategory(int id);

#endif //PROJECT_CRUD_H