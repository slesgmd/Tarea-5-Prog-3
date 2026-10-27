#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>

class Account {
    std::string id;
    double balance;
public:
    Account(std::string id, double balance) : id(id), balance(balance) {}

    std::string getId() const { return id; }
    double getBalance() const { return balance; }
    void setBalance(double b) { balance = b; }
};

class Transaction {
    std::string type;
    Account* account;
    double amount;
public:
    Transaction(std::string type, Account* account, double amount)
        : type(type), account(account), amount(amount) {}

    bool apply() {
        if (type == "deposit") {
            account->setBalance(account->getBalance() + amount);
            return true;
        }
        // no se puede retirar mas de lo que hay
        if (account->getBalance() < amount) return false;
        account->setBalance(account->getBalance() - amount);
        return true;
    }
};

class TransactionManager {
    std::mutex mtx;
    std::vector<Transaction> history;

    TransactionManager() {}

public:
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;

    // static local, en c++11 se inicializa una sola vez aunque haya hilos
    static TransactionManager& getInstance() {
        static TransactionManager instance;
        return instance;
    }

    void deposit(Account* account, double amount) {
        std::lock_guard<std::mutex> lock(mtx);
        Transaction t("deposit", account, amount);
        if (t.apply()) history.push_back(t);
    }

    void withdraw(Account* account, double amount) {
        std::lock_guard<std::mutex> lock(mtx);
        Transaction t("withdraw", account, amount);
        if (t.apply()) history.push_back(t);
    }

    int totalTransactions() {
        std::lock_guard<std::mutex> lock(mtx);
        return history.size();
    }
};

int main() {
    auto& manager = TransactionManager::getInstance();

    Account account("A1", 1000);

    std::vector<std::thread> threads;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            manager.deposit(&account, 100);
            manager.withdraw(&account, 50);
        });
    }

    for (auto& t : threads) t.join();

    std::cout << "Balance final: " << account.getBalance()
              << std::endl;

    std::cout << "Direccion manager: "
              << &TransactionManager::getInstance()
              << std::endl;

    std::cout << "Transacciones: " << manager.totalTransactions() << std::endl;
}
