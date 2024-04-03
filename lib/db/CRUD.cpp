//
// Created by katussska on 4/1/24.
//
#include "CRUD.h"
//TODO: pretty ugly, mby more "helper" functions for code reduction?

///CREATE
void createSchema() {
    std::ifstream schemaFile("../lib/db/schema.sql");
    if (!schemaFile.is_open()) {
        std::cerr << "Error opening schema file." << std::endl;
        throw std::runtime_error("Error opening schema file");
    }

    std::string schemaSql((std::istreambuf_iterator<char>(schemaFile)),
                          std::istreambuf_iterator<char>());

    try {
        db.exec(schemaSql);
        std::cout << "Schema created successfully." << std::endl;
    } catch (SQLite::Exception &e) {
        std::cerr << "Error executing SQL commands: " << e.what() << std::endl;
        throw std::runtime_error("Error executing SQL commands");
    }
}

void createCategory(const Category &category) {
    SQLite::Statement checkQuery(db, "SELECT COUNT(*) FROM Category WHERE name = ?");
    checkQuery.bind(1, category.getName());
    checkQuery.executeStep();

    if (checkQuery.getColumn(0).getInt() > 0) {
        std::cerr << "Category with the same name already exists." << std::endl;
        return;
    }

    SQLite::Statement insertQuery(db, "INSERT INTO Category (name, budget, budgetSet) VALUES (?, ?, ?)");
    insertQuery.bind(1, category.getName());
    insertQuery.bind(2, category.getBudget());
    insertQuery.bind(3, category.isBudgetSet());
    insertQuery.exec();
}

void createAccount(const Account &account) {
    SQLite::Statement checkQuery(db, "SELECT COUNT(*) FROM Account WHERE name = ?");
    checkQuery.bind(1, account.getName());
    checkQuery.executeStep();

    if (checkQuery.getColumn(0).getInt() > 0) {
        std::cerr << "Account with the same name already exists." << std::endl;
        return;
    }

    SQLite::Statement insertQuery(db, "INSERT INTO Account (name, balance) VALUES (?, ?)");
    insertQuery.bind(1, account.getName());
    insertQuery.bind(2, account.getBalance());
    insertQuery.exec();
}

//TODO: mby rework like update/edit?
void createTransaction(const std::string &type, int accountID, const Transaction &income) {
    SQLite::Statement query(db,
                            "INSERT INTO 'Transaction' (type, amount, description, category_id, account_id, date) VALUES (?, ?, ?, ?, ?, ?)");

    query.bind(1, type);
    query.bind(2, income.getAmount());
    query.bind(3, income.getDescription());
    query.bind(4, nullptr);
    query.bind(5, accountID);
    query.bind(6, income.getDate());

    if (query.exec())
        std::cout << "Transaction with name '" << income.getDescription() << "' was added." << std::endl;
    else
        std::cerr << "Failed to add transaction '" << income.getDescription() << "'." << std::endl;
}

void createTransaction(const std::string &type, int accountID, int categoryId, const Transaction &expense) {
    SQLite::Statement query(db,
                            "INSERT INTO 'Transaction' (type, amount, description, category_id, account_id, date) VALUES (?, ?, ?, ?, ?, ?)");
    query.bind(1, type);
    query.bind(2, expense.getAmount());
    query.bind(3, expense.getDescription());
    query.bind(4, categoryId);
    query.bind(5, accountID);
    query.bind(6, expense.getDate());

    if (query.exec())
        std::cout << "Transaction with name '" << expense.getDescription() << "' was added." << std::endl;
    else
        std::cerr << "Failed to add transaction '" << expense.getDescription() << "'." << std::endl;
}

///UPDATE
//TODO: err if amount overstep budget
void updateCategory(int id, const std::string &name, double budget) {
    if (!name.empty()) {
        SQLite::Statement nameQuery(db, "UPDATE Category SET name = ? WHERE id = ?");
        nameQuery.bind(1, name);
        nameQuery.bind(2, id);
        if (nameQuery.exec())
            std::cout << "Edited category with id '" << id << "'. (name)" << std::endl;
        else
            std::cerr << "Editing category with id '" << id << "' unsuccessful.(name)" << std::endl;
    }

    if (budget != 0.0) {
        SQLite::Statement query(db, "SELECT SUM(amount) FROM \"Transaction\" WHERE category_id = ?");
        query.bind(1, id);
        query.exec();
        double sum = query.getColumn(0).getDouble();
        //todo: finish budget err
        SQLite::Statement budgetQuery(db, "UPDATE Category SET budget = ?, budgetSet = 1 WHERE id = ?");
        budgetQuery.bind(1, budget);
        budgetQuery.bind(2, id);
        if (budgetQuery.exec())
            std::cout << "Edited category with id '" << id << "'. (budget)" << std::endl;
        else
            std::cerr << "Editing category with id '" << id << "' unsuccessful.(budget)" << std::endl;
    }
}

//TODO: err if balance negative
void updateAccount(int id, const std::string &name, double balance) {
    if (!name.empty()) {
        SQLite::Statement nameQuery(db, "UPDATE Account SET name = ? WHERE id = ?");
        nameQuery.bind(1, name);
        nameQuery.bind(2, id);
        if (nameQuery.exec())
            std::cout << "Edited account with id '" << id << "'. (name)" << std::endl;
        else
            std::cerr << "Editing account with id '" << id << "' unsuccessful.(name)" << std::endl;
    }

    if (balance != 0.0) {
        SQLite::Statement balanceQuery(db, "UPDATE Account SET balance = ? WHERE id = ?");
        balanceQuery.bind(1, balance);
        balanceQuery.bind(2, id);
        if (balanceQuery.exec())
            std::cout << "Edited category with id '" << id << "'. (balance)" << std::endl;
        else
            std::cerr << "Editing category with id '" << id << "' unsuccessful.(balance)" << std::endl;
    }
}

//TODO: when transaction added mby category or account will be updated
void
updateTransaction(int id, int accountID, int categoryId, double amount, const std::string &description,
                  const std::string &date) {
    if (accountID != 0) {
        SQLite::Statement accountQuery(db, "UPDATE Transaction SET account_id = ? WHERE id = ?");
        accountQuery.bind(1, accountID);
        accountQuery.bind(2, id);
        if (accountQuery.exec())
            std::cout << "Edited transaction with id '" << id << "'. (account_id)" << std::endl;
        else
            std::cerr << "Editing transaction with id '" << id << "' unsuccessful.(account_id)" << std::endl;
    }

    if (categoryId != 0) {
        SQLite::Statement categoryQuery(db, "UPDATE Transaction SET category_id = ? WHERE id = ?");
        categoryQuery.bind(1, categoryId);
        categoryQuery.bind(2, id);
        if (categoryQuery.exec())
            std::cout << "Edited transaction with id '" << id << "'. (category_id)" << std::endl;
        else
            std::cerr << "Editing transaction with id '" << id << "' unsuccessful.(category_id)" << std::endl;
    }

    if (amount != 0.0) {
        SQLite::Statement amountQuery(db, "UPDATE Transaction SET amount = ? WHERE id = ?");
        amountQuery.bind(1, amount);
        amountQuery.bind(2, id);
        if (amountQuery.exec())
            std::cout << "Edited transaction with id '" << id << "'. (amount)" << std::endl;
        else
            std::cerr << "Editing transaction with id '" << id << "' unsuccessful.(amount)" << std::endl;
    }

    if (!description.empty()) {
        SQLite::Statement descQuery(db, "UPDATE Transaction SET description = ? WHERE id = ?");
        descQuery.bind(1, description);
        descQuery.bind(2, id);
        if (descQuery.exec())
            std::cout << "Edited transaction with id '" << id << "'. (description)" << std::endl;
        else
            std::cerr << "Editing transaction with id '" << id << "' unsuccessful.(description)" << std::endl;
    }

    if (!date.empty()) {
        SQLite::Statement dateQuery(db, "UPDATE Transaction SET date = ? WHERE id = ?");
        dateQuery.bind(1, date);
        dateQuery.bind(2, id);
        if (dateQuery.exec())
            std::cout << "Edited transaction with id '" << id << "'. (date)" << std::endl;
        else
            std::cerr << "Editing transaction with id '" << id << "' unsuccessful.(date)" << std::endl;
    }
}

///READ

//TODO: mby wrong id error?
///DELETE
void destroyCategory(int id) {
    SQLite::Statement query(db, "DELETE FROM Category WHERE id = ?");
    query.bind(1, id);

    if (query.exec())
        std::cout << "Category with ID '" << id << "' deleted successfully." << std::endl;
    else
        std::cerr << "Failed to delete category with ID '" << id << "'." << std::endl;
}

void destroyAccount(int id) {
    SQLite::Statement query(db, "DELETE FROM Account WHERE id = ?");
    query.bind(1, id);

    if (query.exec())
        std::cout << "Account with ID '" << id << "' deleted successfully." << std::endl;
    else
        std::cerr << "Failed to delete account with ID '" << id << "'." << std::endl;
}

void destroyTransaction(int id) {
    SQLite::Statement query(db, "DELETE FROM Transaction WHERE id = ?");
    query.bind(1, id);

    if (query.exec())
        std::cout << "Transaction with ID '" << id << "' deleted successfully." << std::endl;
    else
        std::cerr << "Failed to delete transaction with ID '" << id << "'." << std::endl;
}