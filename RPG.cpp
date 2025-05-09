#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

struct Quest {
    string description;
    bool completed;
    int difficulty; // Уровень сложности квеста

    Quest(string desc, int diff) : description(desc), completed(false), difficulty(diff) {}
};

struct NPC {
    string name;
    vector<Quest> quests;

    NPC(string n) : name(n) {}
};

struct Enemy {
    string name;
    int health;
    int attack;

    Enemy(string n, int h, int a) : name(n), health(h), attack(a) {}
};

struct Resource {
    string name;
    int quantity;

    Resource(string n, int q) : name(n), quantity(q) {}
};

struct GameState {
    int level;
    int score;
    string playerName;
    int age;
    string currentWorld;
    vector<NPC> npcs;
    vector<Resource> resources;
    int experience; // Опыт игрока

    void save() {
        ofstream outFile("save.txt");
        if (outFile.is_open()) {
            outFile << playerName << endl;
            outFile << level << endl;
            outFile << score << endl;
            outFile << age << endl;
            outFile << currentWorld << endl;
            outFile << experience << endl;
            outFile << resources.size() << endl;
            for (const auto& resource : resources) {
                outFile << resource.name << " " << resource.quantity << endl;
            }
            outFile.close();
        }
        else {
            cout << "Ошибка при открытии файла для сохранения." << endl;
        }
    }

    void load() {
        ifstream inFile("save.txt");
        if (inFile.is_open()) {
            getline(inFile, playerName);
            inFile >> level;
            inFile >> score;
            inFile >> age;
            inFile >> currentWorld;
            inFile >> experience;
            int resourceCount;
            inFile >> resourceCount;
            resources.clear();
            for (int i = 0; i < resourceCount; ++i) {
                string resourceName;
                int quantity;
                inFile >> resourceName >> quantity;
                resources.emplace_back(resourceName, quantity);
            }
            inFile.close();
        }
        else {
            cout << "Ошибка при открытии файла для загрузки." << endl;
        }
    }

    void addNPC(const NPC& npc) {
        npcs.push_back(npc);
    }

    void addResource(const Resource& resource) {
        for (auto& res : resources) {
            if (res.name == resource.name) {
                res.quantity += resource.quantity;
                return;
            }
        }
        resources.push_back(resource);
    }

    void levelUp() {
        if (experience >= level * 100) { // Уровень сложности для повышения уровня
            level++;
            experience = 0; // Сброс опыта
            cout << "Поздравляем! Вы достигли уровня " << level << "!" << endl;
        }
    }
};

void completeQuest(GameState& state, NPC& npc) {
    for (auto& quest : npc.quests) {
        if (!quest.completed) {
            quest.completed = true;
            state.score += 20 * quest.difficulty; // Очки зависят от сложности квеста
            state.experience += 50 * quest.difficulty; // Опыт зависит от сложности квеста
            cout << "Вы выполнили квест: " << quest.description << "!" << endl;
            state.levelUp(); // Проверка на повышение уровня
            return;
        }
    }
    cout << "У " << npc.name << " нет доступных квестов." << endl;
}

void interactWithNPC(GameState& state) {
    cout << "Выберите NPC для взаимодействия:" << endl;
    for (size_t i = 0; i < state.npcs.size(); ++i) {
        cout << i + 1 << ". " << state.npcs[i].name << endl;
    }
    cout << "Введите номер NPC: ";
    int choice;
    cin >> choice;

    if (choice > 0 && choice <= state.npcs.size()) {
        NPC& selectedNPC = state.npcs[choice - 1];
        cout << "Вы взаимодействуете с " << selectedNPC.name << "." << endl;
        completeQuest(state, selectedNPC);
    }
    else {
        cout << "Неверный выбор." << endl;
    }
}

void battle(GameState& state, Enemy& enemy) {
    cout << "Вы вступили в бой с " << enemy.name << "!" << endl;
    int playerHealth = 100; // Здоровье игрока

    while (enemy.health > 0 && playerHealth > 0) {
        cout << "Ваши очки здоровья: " << playerHealth << endl;
        cout << enemy.name << " Здоровье: " << enemy.health << endl;
        cout << "1. Атаковать" << endl;
        cout << "2. Убежать" << endl;
        int choice;
        cin >> choice;

        if (choice == 1) {
            int playerDamage = rand() % 20 + 10; // Урон игрока
            enemy.health -= playerDamage;
            cout << "Вы нанесли " << playerDamage << " урона " << enemy.name << "!" << endl;

            if (enemy.health > 0) {
                int enemyDamage = rand() % enemy.attack + 5; // Случайный урон врага
                playerHealth -= enemyDamage;
                cout << enemy.name << " атакует вас и наносит " << enemyDamage << " урона!" << endl;
            }
            else {
                cout << "Вы победили " << enemy.name << "!" << endl;
                state.score += 50; // Награда за победу
                state.experience += 30; // Опыт за победу
                state.levelUp(); // Проверка на повышение уровня
            }
        }
        else if (choice == 2) {
            cout << "Вы убежали от боя." << endl;
            break;
        }
        else {
            cout << "Неверный выбор." << endl;
        }
    }

    if (playerHealth <= 0) {
        cout << "Вы погибли в бою." << endl;
        exit(0); // Завершение игры при смерти игрока
    }
}

void collectResources(GameState& state) {
    cout << "Вы собираете ресурсы в " << state.currentWorld << "!" << endl;
    int resourceFound = rand() % 3 + 1; // Случайное количество найденных ресурсов
    for (int i = 0; i < resourceFound; ++i) {
        string resourceName = "Ресурс_" + to_string(rand() % 100); // Генерация уникального имени ресурса
        int quantity = rand() % 5 + 1; // Случайное количество
        state.addResource(Resource(resourceName, quantity));
        cout << "Вы нашли " << quantity << " " << resourceName << "(ов)." << endl;
    }
}

void mainbody(GameState& state) {
    cout << "Вы находитесь в мире: " << state.currentWorld << endl;
    cout << "Ваш уровень: " << state.level << ", Очки: " << state.score << ", Опыт: " << state.experience << endl;
    cout << "Что вы хотите сделать?" << endl;
    cout << "1. Путешествовать в другой мир" << endl;
    cout << "2. Выполнить квест" << endl;
    cout << "3. Взаимодействовать с NPC" << endl;
    cout << "4. Сражаться с врагом" << endl;
    cout << "5. Собрать ресурсы" << endl;
    cout << "6. Сохранить игру" << endl;
    cout << "7. Выйти из игры" << endl;

    int choice;
    cin >> choice;

    switch (choice) {
    case 1:
        if (state.currentWorld == "Мир 1") {
            state.currentWorld = "Мир 2";
        }
        else {
            state.currentWorld = "Мир 1";
        }
        cout << "Вы переместились в " << state.currentWorld << "!" << endl;
        break;
    case 2:
        cout << "Вы выполнили квест в " << state.currentWorld << "!" << endl;
        state.score += 10;
        break;
    case 3:
        interactWithNPC(state);
        break;
    case 4: {
        Enemy enemy("Враг", 100, 15); // Создание врага с 100 здоровья и 15 атаки
        battle(state, enemy);
        break;
    }
    case 5:
        collectResources(state);
        break;
    case 6:
        state.save();
        cout << "Игра сохранена." << endl;
        break;
    case 7:
        cout << "Выход из игры." << endl;
        exit(0);
    default:
        cout << "Неверный выбор. Попробуйте снова." << endl;
        break;
    }
}

void startGame(GameState& state) {
    cout << "Введите ваше имя: ";
    cin >> state.playerName;
    state.level = 1;
    state.score = 0;
    state.age = 18;
    state.currentWorld = "Мир 1";
    state.experience = 0; // Инициализация опыта

    // Добавление NPC с квестами
    NPC npc1("Гарри");
    npc1.quests.push_back(Quest("Принести 5 яблок", 1));
    npc1.quests.push_back(Quest("Убить 3 врага", 2));
    state.addNPC(npc1);

    NPC npc2("Люси");
    npc2.quests.push_back(Quest("Собрать 10 трав", 1));
    npc2.quests.push_back(Quest("Найти редкий артефакт", 3));
    state.addNPC(npc2);

    NPC npc3("Майк");
    npc3.quests.push_back(Quest("Поговорить с Гарри", 1));
    npc3.quests.push_back(Quest("Собрать 5 камней", 2));
    state.addNPC(npc3);

    cout << "Добро пожаловать в игру, " << state.playerName << "!" << endl;
    state.save();
    while (true) {
        mainbody(state);
    }
}

void showMenu() {
    cout << "Меню:" << endl;
    cout << "1. Начать игру" << endl;
    cout << "2. Загрузить игру" << endl;
    cout << "3. Выйти" << endl;
}

int main() {
    srand(static_cast<unsigned int>(time(0))); // Инициализация генератора случайных чисел
    setlocale(LC_ALL, "ru");
    GameState gameState;
    int choice;

    while (true) {
        showMenu();
        cin >> choice;

        if (choice == 1) {
            startGame(gameState);
            break;
        }
        else if (choice == 2) {
            gameState.load();
            cout << "Игра загружена! Добро пожаловать обратно, "
                << gameState.playerName
                << "! Уровень: "
                << gameState.level
                << ", Очки: "
                << gameState.score
                << ", Опыт: "
                << gameState.experience
                << ", Мир: "
                << gameState.currentWorld << endl;

            while (true) {
                mainbody(gameState);
            }
            break;
        }
        else if (choice == 3) {
            cout << "Выход из игры." << endl;
            return 0;
        }
        else {
            cout << "Неверный выбор. Попробуйте снова." << endl;
        }
    }

    return 0;
}


