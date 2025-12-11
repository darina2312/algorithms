#include <iostream>
#include <cstdint>

struct Node {
    Node* npx;   // prev ^ next
    int data;

    Node(int val) : npx(nullptr), data(val) {}
};

Node* XOR(Node* a, Node* b) {
    return (Node*)((uintptr_t)a ^ (uintptr_t)b);
}

void create_recursive(Node*& head, Node*& tail) {
    int buffer = 1;
    std::cin >> buffer;

    if (buffer == 0) {
        head = nullptr;
        tail = nullptr;
        return;
    }
    // сначала рекурсивно строим "хвост" списка
    Node* h = nullptr;
    Node* t = nullptr;
    create_recursive(h, t);
    // теперь создаём текущий узел
    Node* new_node = new Node(buffer);
    // вставляем его ПЕРЕД списком [h ... t]
    new_node->npx = XOR(nullptr, h);
    if (h != nullptr) {
        // у h раньше prev = nullptr, next = second
        // стало prev = new_node, next = second
        Node* second = XOR(nullptr, h->npx);
        h->npx = XOR(new_node, second);

        head = new_node;
        tail = t;
    } else {
        // это был первый (и единственный) узел
        head = tail = new_node;
    }
}


void create_iterate(Node*& head, Node*& tail) {
    head = nullptr;
    tail = nullptr;

    int buffer = 1;        // ← как ты хочешь
    while (buffer) {
        std::cin >> buffer;
        if (buffer == 0) break;

        Node* new_node = new Node(buffer);

        // первый элемент
        if (head == nullptr) {
            head = tail = new_node;
            new_node->npx = XOR(nullptr, nullptr);
        }
        else {
            // у tail сейчас: npx = prev ^ nullptr
            // поэтому prev = XOR(nullptr, tail->npx)
            Node* real_prev = XOR(nullptr, tail->npx);

            // новый хвост: prev = tail, next = nullptr
            new_node->npx = XOR(tail, nullptr);

            // tail обновляется: новый next = new_node
            tail->npx = XOR(real_prev, new_node);

            // сдвигаем хвост
            tail = new_node;
        }
    }
}


void print_recursive(Node* curr, Node* prev) {
    if (curr == nullptr) {
        return;
    }
    std::cout << curr->data << " ";
    Node* next = XOR(prev, curr->npx);
    print_recursive(next, curr);
}

void print_iterate(Node* head) {
    Node* prev = nullptr;
    Node* curr = head;
    while (curr != nullptr) {
        std::cout << curr->data << " ";
        Node* next = XOR(prev, curr->npx);
        prev = curr;
        curr = next;
    }
    std::cout << "\n";
}

void add_head_iter(Node*& head, Node*& tail, int value) {
    Node* new_node = new Node(value);

    if (head == nullptr) {
        head = tail = new_node;
        return;
    }

    Node* old_head = head;
    Node* second = XOR(nullptr, old_head->npx);  // next узел за old_head
    // prev = nullptr, next = old_head
    new_node->npx = XOR(nullptr, old_head);
    // old_head теперь имеет prev = new_node
    old_head->npx = XOR(new_node, second);

    head = new_node;
}

void add_tail_iter(Node*& head, Node*& tail, int value) {
    Node* new_node = new Node(value);
    // список пуст
    if (head == nullptr) {
        head = tail = new_node;
        new_node->npx = XOR(nullptr, nullptr);  // можно и просто nullptr
        return;
    }
    // tail — текущий хвост
    // у хвоста: npx = prev ^ nullptr → prev = XOR(nullptr, tail->npx)
    Node* prev = XOR(nullptr, tail->npx);
    // у нового хвоста: prev = tail, next = nullptr
    new_node->npx = XOR(tail, nullptr);
    // у старого хвоста: prev ^ new_node
    tail->npx = XOR(prev, new_node);
    // сдвигаем хвост
    tail = new_node;
}

void swap_12(Node*& head, Node*& tail) {
    if (head == nullptr) return;          // пустой список
    if (head->npx == nullptr) return;     // один узел, менять нечего

    Node* first = head;
    Node* second = XOR(nullptr, first->npx);   // prev = nullptr, значит npx = next

    if (second == nullptr) return;        // на всякий случай, хотя сюда не дойдём
    // second->npx = first ^ third
    Node* third = XOR(first, second->npx);
    // делаем second новой головой
    head = second;
    // second: prev = nullptr, next = first
    second->npx = XOR(nullptr, first);
    // first: prev = second, next = third
    first->npx = XOR(second, third);
    // нужно поправить third, если он есть
    if (third != nullptr) {
        // было: third->npx = second ^ fourth
        // надо: third->npx = first ^ fourth
        uintptr_t old_npx = (uintptr_t)third->npx;
        old_npx ^= (uintptr_t)second;   // убираем second
        old_npx ^= (uintptr_t)first;    // добавляем first
        third->npx = (Node*)old_npx;
    }
    // хвост меняется только если в списке было ровно 2 элемента:
    // тогда third == nullptr, и новым хвостом становится first
    if (third == nullptr) {
        tail = first;
    }
}

void delete_iter(Node*& head) {
    Node* prev = nullptr;
    Node* curr = head;

    while (curr != nullptr) {
        Node* next = XOR(prev, curr->npx);
        prev = curr;
        delete curr;
        curr = next;
    }

    head = nullptr;
}

void delete_rec(Node* curr, Node* prev) {
    if (curr == nullptr) {
        return;
    }

    Node* next = XOR(prev, curr->npx);
    delete_rec(next, curr); // рекурсивно удалили хвост, пока curr ещё "живой"
    delete curr; // теперь удаляем curr
}

int main() {
    Node* head = nullptr;
    Node* tail = nullptr;

    // СОЗДАНИЕ СПИСКА

    // Вариант 1: рекурсивно
    // create_recursive(head, tail);

    // Вариант 2: итеративно
    create_iterate(head, tail);

    // ВЫВОД

    std::cout << "print_iterate: ";
    print_iterate(head);

    std::cout << "print_recursive: ";
    print_recursive(head, nullptr);
    std::cout << "\n";

    // ДОБАВЛЕНИЕ В НАЧАЛО

    std::cout << "add_head_iter(100): ";
    add_head_iter(head, tail, 100);
    print_iterate(head);

    // ДОБАВЛЕНИЕ В КОНЕЦ

    std::cout << "add_tail_iter(200): ";
    add_tail_iter(head, tail, 200);
    print_iterate(head);

    // ОБМЕН ПЕРВЫХ ДВУХ

    std::cout << "swap_12: ";
    swap_12(head, tail);
    print_iterate(head);

    // УДАЛЕНИЕ СПИСКА

    delete_iter(head);
    tail = nullptr;

    // или так:
    // delete_rec(head, nullptr);
    // head = nullptr;
    // tail = nullptr;

    return 0;
}



