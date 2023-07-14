#include <iostream>
#include <fstream>

using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::ifstream;

const int MAX_USERS = 1000;
const int MAX_TRANSACTIONS = 1000;
const int MAX_BLOCKS = 1000;
const char fileNameOfUsers[] = "users.dat";
const char fileNameOfBlocks[] = "blocks.dat";

unsigned computeHash(const unsigned char* memory, int length) {
    unsigned hash = 0xbeaf;
    for (int c = 0; c < length; c++) {
        hash += memory[c];
        hash += hash << 10;
        hash ^= hash >> 6;
    }
    hash += hash << 3;
    hash ^= hash >> 11;
    hash += hash << 15;
    return hash;
}   

struct User {
    unsigned id;
    char name[128];
};

struct Transaction {
    unsigned sender;
    unsigned receiver;
    double coins;
    long long time;
};

struct TransactionBlock {
    unsigned id;
    unsigned prevBlockId;
    unsigned prevBlockHash;
    int validTransactions;
    Transaction transactions[16];
};

struct SystemOfBlocks {
    TransactionBlock blocks[MAX_BLOCKS];
    int count = 0;
};

struct SystemOfUsers {
    User users[MAX_USERS];
    int count = 0;
};

void InitUser(User& user, Transaction& transaction, double investment)
{
    cout << "Enter user name: ";
    cin.ignore();
    cin.getline(user.name, 128);
    cout << "Enter amount: ";
    cin >> investment;
    if (investment <= 0)
        cout << "Error! You have to input amaunt bigger than 0.";
}

void CreateUser(ifstream& file, User& user, Transaction& transaction, SystemOfUsers& systemOfUsers, User& systemUser)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }
    unsigned prevUserId = 0;
    for (size_t i = 0; i < systemOfUsers.count; i++)
    {
        if (prevUserId < systemOfUsers.users[i].id)
            prevUserId = systemOfUsers.users[i].id;
    }
    user.id = prevUserId + 1;
}

void createTransaction(const char* command, Transaction& transaction, User& receiverUser, User& senderUser, User& systemUser, double investment, TransactionBlock& transactionBlock, SystemOfBlocks& systemOfBlocks)
{
    if (strcmp(command, "create-user") == 0)
    {
        transaction.sender = systemUser.id;
        transaction.receiver = receiverUser.id;
        transaction.coins = investment * 420;
        AddTransactionsToBlock(transactionBlock, systemOfBlocks, transaction);
    }

    if (strcmp(command, "send-coins"))
    {

        transaction.sender = senderUser
    }
}

void AddUserToSystem(User& user, SystemOfUsers& system)
{
    if (system.count <= MAX_USERS)
    {
        system.users[system.count] = user;
        system.count++;
    }
}

void AddTransactionsToBlock(TransactionBlock& transactionBlock, SystemOfBlocks& systemOfBlocks, Transaction& transaction)
{
    if (systemOfBlocks.count <= MAX_BLOCKS)
    {
        if (systemOfBlocks.count == 0)
        {
            for (size_t i = 0; i < systemOfBlocks.count; i++)
                transactionBlock.id = i + 1;
            transactionBlock.prevBlockId = transactionBlock.id;
            if (transactionBlock.validTransactions <= 16)
            {
                transactionBlock.transactions[transactionBlock.validTransactions] = transaction;
                transactionBlock.validTransactions++;
            }
            transactionBlock.prevBlockHash = computeHash((const unsigned char*)&systemOfBlocks.blocks[0], systemOfBlocks.count);
            systemOfBlocks.blocks[systemOfBlocks.count] = transactionBlock;
            systemOfBlocks.count++;
        }
        else {
            transactionBlock.prevBlockId = systemOfBlocks.blocks[systemOfBlocks.count - 1].id;
            for (size_t i = 0; i < systemOfBlocks.count; i++)
                transactionBlock.id = i + 1;
            if (transactionBlock.validTransactions <= 16)
            {
                transactionBlock.transactions[transactionBlock.validTransactions] = transaction;
                transactionBlock.validTransactions++;
            }
            transactionBlock.prevBlockHash = computeHash((const unsigned char*)&systemOfBlocks.blocks[systemOfBlocks.count - 1], systemOfBlocks.count);
            transactionBlock.prevBlockHash = systemOfBlocks.blocks[systemOfBlocks.count - 1].prevBlockHash;
            systemOfBlocks.blocks[systemOfBlocks.count] = transactionBlock;
            systemOfBlocks.count++;
        }
    }
}

void WriteUserToFile(ofstream& file, User& user, const double& investment)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }
    file.write((const char*)&user.id, sizeof(user.id));
    file.write(user.name, sizeof(user.name));
}

void createSystemUser(User& user, User& systemUser)
{
    strcpy_s(systemUser.name, "SYSTEM_USER");
    systemUser.id = 0;
}

void WriteSystemOfUsersToFile(ofstream& writeFile, SystemOfUsers& systemOfUsers, SystemOfBlocks& systemOfBlocks)
{
    if (!writeFile.is_open())
    {
        cout << "Error";
        return;
    }

    for (size_t i = 0; i < systemOfUsers.count; i++)
    {
        WriteUserToFile(writeFile, systemOfUsers.users[i], systemOfBlocks.blocks->transactions[i].coins);

    }
}

void WriteTransactionToFile(ofstream& file, Transaction& transaction)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }

    file.write((const char*)&transaction.receiver, sizeof(transaction.receiver));
    file.write((const char*)&transaction.sender, sizeof(transaction.sender));
    file.write((const char*)&transaction.coins, sizeof(transaction.coins));
}

void WriteTransactionBlockToFile(ofstream& file, TransactionBlock& transactionBlock)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }

    file.write((const char*)&transactionBlock.id, sizeof(transactionBlock.id));
    file.write((const char*)&transactionBlock.prevBlockHash, sizeof(transactionBlock.prevBlockHash));
    file.write((const char*)&transactionBlock.prevBlockId, sizeof(transactionBlock.prevBlockId));
    for (size_t i = 0; i < transactionBlock.validTransactions; i++)
    {
        WriteTransactionToFile(file, transactionBlock.transactions[i]);
    }
}

void WriteSystemOfBlockToFile(ofstream& file, SystemOfBlocks system)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }

    for (size_t i = 0; i < system.count; i++)
    {
        WriteTransactionBlockToFile(file, system.blocks[i]);
    }
}

void ReadUser(ifstream& file, User& user, Transaction& transaction)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }

    file.read((char*)&user.name, sizeof(user.name));
    file.read((char*)&transaction.coins, sizeof(transaction.coins));
    cout << user.name << " " << transaction.coins << endl;
}

void ReadUserFromFile(ifstream& file, SystemOfUsers& systemOfUsers, SystemOfBlocks& systemOfBlocks)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }

    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    int size = sizeof(systemOfUsers.users->name) + sizeof(systemOfBlocks.blocks->transactions->coins);
    int counter = fileSize / size;
    file.seekg(0, std::ios::beg);

    while (!file.eof())
    {
        char buff[128];
        file.read(buff, 128);

        strcpy_s(systemOfUsers.users->name, buff);
        cout << systemOfUsers.users->name << endl;
    }

   /* for (size_t i = 0; i < counter; i++)
    {
        file.read((char*)&systemOfUsers.users->name, sizeof(systemOfUsers.users->name));
        file.read((char*)&systemOfBlocks.blocks->transactions->coins, sizeof(systemOfBlocks.blocks->transactions->coins));
        cout << systemOfUsers.users->name << " " << systemOfBlocks.blocks->transactions->coins << endl;
    }*/
}

void ReadBlockFromFile(ifstream& file, SystemOfBlocks& system, TransactionBlock& transactionBlock)
{
    if (!file.is_open())
    {
        cout << "Error";
        return;
    }

    file.seekg(0, std::ios::end);
    int fileSize = file.tellg();
    int size = sizeof(transactionBlock.id) + sizeof(transactionBlock.prevBlockHash) + sizeof(transactionBlock.prevBlockId) + sizeof(transactionBlock.transactions);
    int counter = fileSize / size;
    file.seekg(0, std::ios::beg);

    for (size_t i = 1; i < counter; i++)
    {
        file.read((char*)&system.blocks[i].id, sizeof(system.blocks[i].id));
        file.read((char*)&system.blocks[i].prevBlockHash, sizeof(system.blocks[i].prevBlockHash));
        file.read((char*)&transactionBlock.prevBlockId, sizeof(transactionBlock.prevBlockId));
        file.read((char*)&transactionBlock.transactions, sizeof(transactionBlock.transactions));
        
        cout << system.blocks[i].id << " " << system.blocks[i].prevBlockHash << " " << system.blocks[i].prevBlockId << " " << system.blocks[i].transactions << endl;
    }
}


int main()
{
    User systemUser;
    User user;
    Transaction transaction;
    TransactionBlock transactionBlock;
    transactionBlock.validTransactions = 0;
    SystemOfUsers systemUsers;
    SystemOfBlocks systemBlocks;

    ofstream writeFileOfUsers(fileNameOfUsers, std::ios::binary | std::ios::app);
    ifstream readFileOfUsers(fileNameOfUsers, std::ios::binary);
    ofstream writeFileOfBlocks(fileNameOfBlocks, std::ios::binary | std::ios::app);
    ifstream readFileOfBlocks(fileNameOfBlocks, std::ios::binary);
       
    createSystemUser(user, systemUser);
    if (systemUsers.count == 0)
        AddUserToSystem(systemUser, systemUsers);

    cout << "Enter one of this functions: create-user" << endl;
    cout << "                             remove-user" << endl;
    cout << "                             send-coins" << endl;
    cout << "                             verify-transactions" << endl;
    cout << "                             wealthiest-users" << endl;
    cout << "                             biggest-blocks" << endl;

    char command[20];
    cin.getline(command, 20);

    if (strcmp(command, "create-user") == 0)
    {
        cout << "Enter the number of users you want to create: ";
        int numUsers;
        cin >> numUsers;

        for (size_t i = 0; i < numUsers; i++)
        {
            InitUser(user, transaction);
            AddUserToSystem(user, systemUsers);
            AddTransactionsToBlock(transactionBlock, systemBlocks, transaction);
        }

        WriteSystemOfUsersToFile(writeFileOfUsers, systemUsers, systemBlocks);
        WriteSystemOfBlockToFile(writeFileOfBlocks, systemBlocks);
        cout << "The function create-user is successful";
        ReadUserFromFile(readFileOfUsers, systemUsers, systemBlocks);
        cout << endl;
        cout << endl;
        //ReadBlockFromFile(readFileOfBlocks, systemBlocks, transactionBlock);
    }

    if (strcmp(command, "remove-user"))
    {

    }

    if (strcmp(command, "send-coins"))
    {
        char senderName[128];
        char recieverName[128];
        cout << "Enter your name: ";
        cin.getline(senderName, 128);
        cout << "Enter the user name that you want to send coins: ";
        cin.getline(recieverName, 128);
        createTransaction()
    }

    if (strcmp(command, "verify-transactions"))
    {

    }

    if (strcmp(command, "wealthiest-users"))
    {

    }

    if (strcmp(command, "biggest-blocks"))
    {

    }

    writeFileOfUsers.close();
    readFileOfUsers.close();
}

