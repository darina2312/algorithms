#include <iostream>
#include <cstdint>

struct Node {
    uintptr_t npx;   // prev ^ next (адреса как число)
    int data;
};

// ================== РЕКУРСИВНОЕ СОЗДАНИЕ ==================
void create_recursive(Node*& head, Node*& tail) {
    int buffer = 1;
    std::cin >> buffer;

    if (buffer == 0) {
        head = nullptr;
        tail = nullptr;
        return;
    }
    // сначала рекурсивно строим хвост
    Node* h = nullptr;
    Node* t = nullptr;
    create_recursive(h, t);
    // создаём текущий узел
    Node* new_node = new Node;
    new_node->data = buffer;
    new_node->npx = (uintptr_t)h;   // nullptr ^ h

    if (h != nullptr) {
        // у h раньше prev = nullptr, next = second
        Node* second = (Node*)((uintptr_t)nullptr ^ h->npx);  // просто (Node*)h->npx
        h->npx = (uintptr_t)new_node ^ (uintptr_t)second;

        head = new_node;
        tail = t;
    } else {
        // первый и единственный элемент
        head = tail = new_node;
    }
}

// ================== ИТЕРАЦИОННОЕ СОЗДАНИЕ ==================
void create_iterate(Node*& head, Node*& tail) {
    head = nullptr;
    tail = nullptr;

    int buffer = 1;

    while (buffer) {
        std::cin >> buffer;
        if (buffer == 0) break;

        Node* new_node = new Node;
        new_node->data = buffer;
        new_node->npx = 0;

        if (head == nullptr) {
            head = tail = new_node;
        } else {
            // tail->npx = prev ^ nullptr → prev = (Node*)tail->npx
            Node* prev = (Node*)tail->npx;
            // новый хвост: prev = tail, next = nullptr
            new_node->npx = (uintptr_t)tail;
            // tail: prev ^ new_node
            tail->npx = (uintptr_t)prev ^ (uintptr_t)new_node;

            tail = new_node;
        }
    }
}

// ================== ВЫВОД СПИСКА ==================
void print_recursive(Node* curr, Node* prev) {
    if (curr == nullptr) {
        return;
    }
    std::cout << curr->data << " ";
    Node* next = (Node*)((uintptr_t)prev ^ curr->npx);
    print_recursive(next, curr);
}

void print_iterate(Node* head) {
    Node* prev = nullptr;
    Node* curr = head;
    while (curr != nullptr) {
        std::cout << curr->data << " ";
        Node* next = (Node*)((uintptr_t)prev ^ curr->npx);
        prev = curr;
        curr = next;
    }
    std::cout << "\n";
}

// ================== ДОБАВЛЕНИЕ В НАЧАЛО ==================
void add_head_iter(Node*& head, Node*& tail, int value) {
    Node* new_node = new Node;
    new_node->data = value;
    new_node->npx = 0;
    if (head == nullptr) {
        head = tail = new_node;
        return;
    }
    Node* old_head = head;
    Node* second = (Node*)((uintptr_t)nullptr ^ old_head->npx); // (Node*)old_head->npx
    // новый узел: prev = nullptr, next = old_head
    new_node->npx = (uintptr_t)old_head;
    // old_head: prev = new_node, next = second
    old_head->npx = (uintptr_t)new_node ^ (uintptr_t)second;
    head = new_node;
}

// ================== ДОБАВЛЕНИЕ В КОНЕЦ ==================
void add_tail_iter(Node*& head, Node*& tail, int value) {
    Node* new_node = new Node;
    new_node->data = value;
    new_node->npx = 0;
    if (head == nullptr) {
        head = tail = new_node;
        return;
    }
    // tail->npx = prev ^ nullptr → prev = (Node*)tail->npx
    Node* prev = (Node*)tail->npx;
    // новый хвост: prev = tail, next = nullptr
    new_node->npx = (uintptr_t)tail;
    // старый хвост: prev ^ new_node
    tail->npx = (uintptr_t)prev ^ (uintptr_t)new_node;
    tail = new_node;
}

// ================== ОБМЕН ПЕРВЫХ ДВУХ УЗЛОВ ==================
void swap_12(Node*& head, Node*& tail) {
    if (head == nullptr) return;          // пустой список
    if (head->npx == 0) return;           // один узел
    Node* first = head;
    Node* second = (Node*)((uintptr_t)nullptr ^ first->npx); // (Node*)first->npx
    if (second == nullptr) return;
    // second->npx = first ^ third
    Node* third = (Node*)((uintptr_t)first ^ second->npx);
    // новая голова — second
    head = second;
    // second: prev = nullptr, next = first
    second->npx = (uintptr_t)first;
    // first: prev = second, next = third
    first->npx = (uintptr_t)second ^ (uintptr_t)third;
    if (third != nullptr) {
        // было: third->npx = second ^ fourth
        // надо: third->npx = first ^ fourth
        uintptr_t old_npx = third->npx;
        old_npx ^= (uintptr_t)second;
        old_npx ^= (uintptr_t)first;
        third->npx = old_npx;
    }
    // если элементов всего 2, third == nullptr → новый хвост = first
    if (third == nullptr) {
        tail = first;
    }
}

// ================== УДАЛЕНИЕ СПИСКА ==================
void delete_iter(Node*& head) {
    Node* prev = nullptr;
    Node* curr = head;
    while (curr != nullptr) {
        Node* next = (Node*)((uintptr_t)prev ^ curr->npx);
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
    Node* next = (Node*)((uintptr_t)prev ^ curr->npx);
    delete_rec(next, curr);
    delete curr;
}

// ================== MAIN ==================
int main() {
    Node* head = nullptr;
    Node* tail = nullptr;
    std::cout << "Введите элементы списка (0 — конец ввода):\n";
    // создание списка
    // create_recursive(head, tail);
    create_iterate(head, tail);

    // вывод
    std::cout << "print_iterate: ";
    print_iterate(head);

    std::cout << "print_recursive: ";
    print_recursive(head, nullptr);
    std::cout << "\n";

    // добавление в начало
    std::cout << "add_head_iter(100): ";
    add_head_iter(head, tail, 100);
    print_iterate(head);

    // добавление в конец
    std::cout << "add_tail_iter(200): ";
    add_tail_iter(head, tail, 200);
    print_iterate(head);

    // обмен первых двух
    std::cout << "swap_12: ";
    swap_12(head, tail);
    print_iterate(head);

    // удаление (выбери один вариант)

    delete_iter(head);
    tail = nullptr;

    // или рекурсивно:
    // delete_rec(head, nullptr);
    // head = nullptr;
    // tail = nullptr;

    return 0;
}
