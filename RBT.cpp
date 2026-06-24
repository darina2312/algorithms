/*
 * КРАСНО-ЧЁРНОЕ ДЕРЕВО (Red-Black Tree) — полная реализация на C++
 * ==================================================================
 *
 * Используется классический подход (как в книге Кормена и др.):
 *  - вместо nullptr используется один общий служебный узел-страж NIL
 *    (sentinel). Он всегда ЧЁРНЫЙ. Это сильно упрощает код, потому что
 *    не нужно отдельно проверять "а не пустой ли это указатель" перед
 *    каждым обращением к цвету/детям — у NIL есть цвет (BLACK) и его
 *    можно безопасно использовать как обычный узел при сравнениях.
 *  - каждый узел хранит указатель на родителя (parent) — это нужно,
 *    чтобы при вставке/удалении подниматься вверх по дереву и чтобы
 *    вращения могли быстро узнать "дедушку" и "дядю".
 *
 * Свойства КЧД, которые мы поддерживаем после каждой вставки/удаления:
 *  1) Каждый узел — красный или чёрный.
 *  2) Корень — чёрный.
 *  3) Каждый лист (NIL) — чёрный.
 *  4) У красного узла оба ребёнка — чёрные (нет двух красных подряд).
 *  5) Все пути от узла до листьев-потомков содержат одинаковое число
 *     чёрных узлов (чёрная высота).
 */

#include <iostream>
#include <string>
#include <stdexcept>

enum Color { RED, BLACK };

struct Node {
    int key;
    Color color;
    Node* left;
    Node* right;
    Node* parent;
};

class RedBlackTree {
private:
    Node* NIL;   // общий "страж" — заменяет nullptr, всегда чёрный
    Node* root;

    // ---------- ВРАЩЕНИЯ ----------
    // Левое вращение вокруг узла x.
    // До:      x  (a - левый ребёнок x, y - правый)
    //             y - правый ребёнок x, у y есть дети b (левый) и c (правый)
    // После:   y становится на место x, x становится левым ребёнком y,
    //          поддерево b "переезжает" от y к x как его правое поддерево,
    //          поддерево c остаётся правым поддеревом y.
    // y "поднимается" на место x, его левое поддерево (b) переходит
    // к x как правое поддерево. Порядок ключей при этом не нарушается.
    void leftRotate(Node* x) {
        Node* y = x->right;          // y — правый ребёнок x, он станет новым "верхним"
        x->right = y->left;          // поддерево b "переезжает" от y к x
        if (y->left != NIL) {
            y->left->parent = x;     // не забываем обновить родителя у b
        }
        y->parent = x->parent;       // y встаёт на место x в дереве
        if (x->parent == NIL) {
            root = y;                 // x был корнем — теперь корень y
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }
        y->left = x;                 // x становится левым ребёнком y
        x->parent = y;
    }

    // Правое вращение — зеркально левому.
    void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right != NIL) {
            y->right->parent = x;
        }
        y->parent = x->parent;
        if (x->parent == NIL) {
            root = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }
        y->right = x;
        x->parent = y;
    }

    // ---------- ВСТАВКА ----------
    // Восстановление свойств КЧД после обычной BST-вставки нового
    // КРАСНОГО узла z. Возможное нарушение — только свойство 4
    // ("два красных подряд"), потому что мы вставили красный узел.
    void insertFixup(Node* z) {
        // Пока родитель z красный — свойство 4 нарушено и его нужно
        // исправлять. Если родитель чёрный — всё уже в порядке, выходим.
        while (z->parent->color == RED) {
            if (z->parent == z->parent->parent->left) {
                // Родитель — левый ребёнок дедушки, значит "дядя" — справа
                Node* uncle = z->parent->parent->right;

                if (uncle->color == RED) {
                    // СЛУЧАЙ 1: дядя красный.
                    // Просто перекрашиваем родителя и дядю в чёрный,
                    // а дедушку — в красный. Чёрная высота не меняется,
                    // но дедушка теперь красный, и проблема "два красных
                    // подряд" может всплыть ещё выше — поэтому продолжаем
                    // цикл, поднявшись к дедушке.
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    // Дядя чёрный — структурная проблема, нужно вращение.
                    if (z == z->parent->right) {
                        // СЛУЧАЙ 2 ("треугольник"): z — правый ребёнок,
                        // а его родитель — левый ребёнок дедушки.
                        // Превращаем в "линию" левым вращением родителя,
                        // после чего попадаем в случай 3.
                        z = z->parent;
                        leftRotate(z);
                    }
                    // СЛУЧАЙ 3 ("линия"): z и его родитель оба левые дети.
                    // Перекрашиваем родителя в чёрный, дедушку в красный
                    // и проворачиваем дедушку направо — после этого
                    // свойства полностью восстановлены, цикл завершится
                    // (новый родитель z чёрный).
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    rightRotate(z->parent->parent);
                }
            } else {
                // Зеркальная ветка: родитель — правый ребёнок дедушки.
                Node* uncle = z->parent->parent->left;

                if (uncle->color == RED) {
                    z->parent->color = BLACK;
                    uncle->color = BLACK;
                    z->parent->parent->color = RED;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }
                    z->parent->color = BLACK;
                    z->parent->parent->color = RED;
                    leftRotate(z->parent->parent);
                }
            }
        }
        root->color = BLACK; // свойство 2: корень всегда чёрный
    }

    // ---------- УДАЛЕНИЕ ----------
    // "Пересадка" поддерева v на место поддерева u (стандартная
    // операция замены ссылки у родителя, используется при удалении).
    void transplant(Node* u, Node* v) {
        if (u->parent == NIL) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }
        v->parent = u->parent; // даже если v == NIL, NIL->parent временно
                                // используется в deleteFixup как "откуда пришли"
    }

    // Минимальный узел в поддереве (самый левый) — нужен, чтобы найти
    // преемника при удалении узла с двумя детьми.
    Node* minimum(Node* x) {
        while (x->left != NIL) {
            x = x->left;
        }
        return x;
    }

    // Восстановление свойств КЧД после удаления ЧЁРНОГО узла.
    // x — узел, который "получил" недостающую черноту (double black):
    // формально на пути через x теперь на 1 чёрный узел меньше, чем
    // должно быть, и это нужно либо передать вверх по дереву, либо
    // погасить локально с помощью перекраски/вращения.
    void deleteFixup(Node* x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node* sibling = x->parent->right;

                if (sibling->color == RED) {
                    // СЛУЧАЙ 1: брат красный.
                    // Красный брат не может напрямую участвовать в
                    // перекраске (т.к. у него по определению чёрные дети
                    // — свойство 4), поэтому "превращаем" его в чёрного
                    // вращением, чтобы свести к одному из случаев 2-4.
                    sibling->color = BLACK;
                    x->parent->color = RED;
                    leftRotate(x->parent);
                    sibling = x->parent->right; // обновляем брата после вращения
                }

                if (sibling->left->color == BLACK && sibling->right->color == BLACK) {
                    // СЛУЧАЙ 2: брат чёрный, и оба его ребёнка чёрные.
                    // Можно "одолжить" чернину у брата — перекрашиваем
                    // его в красный (локально баланс восстановлен по
                    // поддереву брата), но дефицит черноты теперь
                    // переходит на родителя x — поднимаемся вверх.
                    sibling->color = RED;
                    x = x->parent;
                } else {
                    if (sibling->right->color == BLACK) {
                        // СЛУЧАЙ 3: брат чёрный, ближний к x ребёнок
                        // брата (левый) красный, дальний (правый) чёрный.
                        // Вращаем брата вправо, чтобы красный узел
                        // переместился на "дальнюю" позицию — сводим к
                        // случаю 4.
                        sibling->left->color = BLACK;
                        sibling->color = RED;
                        rightRotate(sibling);
                        sibling = x->parent->right;
                    }
                    // СЛУЧАЙ 4: брат чёрный, дальний ребёнок брата красный.
                    // Это финальный случай — одним вращением и перекраской
                    // полностью устраняем дефицит черноты без необходимости
                    // подниматься выше.
                    sibling->color = x->parent->color;
                    x->parent->color = BLACK;
                    sibling->right->color = BLACK;
                    leftRotate(x->parent);
                    x = root; // выходим из цикла — проблема решена
                }
            } else {
                // Зеркальная ветка: x — правый ребёнок родителя.
                Node* sibling = x->parent->left;

                if (sibling->color == RED) {
                    sibling->color = BLACK;
                    x->parent->color = RED;
                    rightRotate(x->parent);
                    sibling = x->parent->left;
                }

                if (sibling->right->color == BLACK && sibling->left->color == BLACK) {
                    sibling->color = RED;
                    x = x->parent;
                } else {
                    if (sibling->left->color == BLACK) {
                        sibling->right->color = BLACK;
                        sibling->color = RED;
                        leftRotate(sibling);
                        sibling = x->parent->left;
                    }
                    sibling->color = x->parent->color;
                    x->parent->color = BLACK;
                    sibling->left->color = BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK; // либо x был красным (просто перекрашиваем —
                           // "лишняя чернота" гасится), либо это root
    }

    // ---------- ВСПОМОГАТЕЛЬНЫЕ РЕКУРСИВНЫЕ ФУНКЦИИ ----------

    void inorderHelper(Node* x) const {
        if (x == NIL) return;
        inorderHelper(x->left);
        std::cout << x->key << (x->color == RED ? "(R) " : "(B) ");
        inorderHelper(x->right);
    }

    // Удаляет все узлы поддерева (для деструктора)
    void destroySubtree(Node* x) {
        if (x == NIL) return;
        destroySubtree(x->left);
        destroySubtree(x->right);
        delete x;
    }

    // Красивая печать дерева "набок", с отступами и цветами
    void printHelper(Node* x, const std::string& prefix, bool isLeft) const {
        if (x == NIL) return;

        std::cout << prefix;
        std::cout << (isLeft ? "├── L:" : "└── R:");
        std::cout << x->key << (x->color == RED ? " [RED]" : " [BLACK]") << "\n";

        std::string childPrefix = prefix + (isLeft ? "│   " : "    ");
        printHelper(x->left, childPrefix, true);
        printHelper(x->right, childPrefix, false);
    }

    // Считает чёрную высоту (число чёрных узлов на пути к NIL, не
    // считая сам узел) и одновременно проверяет свойства 4 и 5.
    // Возвращает -1, если свойства нарушены (используется в verify()).
    int blackHeightHelper(Node* x) const {
        if (x == NIL) return 0; // NIL чёрный, но не считаем сам NIL как "+1"

        if (x->color == RED) {
            // свойство 4: у красного узла оба ребёнка должны быть чёрными
            if (x->left->color == RED || x->right->color == RED) {
                return -1;
            }
        }

        int leftBH = blackHeightHelper(x->left);
        int rightBH = blackHeightHelper(x->right);

        if (leftBH == -1 || rightBH == -1 || leftBH != rightBH) {
            // свойство 5: чёрная высота слева и справа должна совпадать
            return -1;
        }

        return leftBH + (x->color == BLACK ? 1 : 0);
    }

public:
    RedBlackTree() {
        NIL = new Node{0, BLACK, nullptr, nullptr, nullptr};
        NIL->left = NIL->right = NIL->parent = NIL;
        root = NIL;
    }

    ~RedBlackTree() {
        destroySubtree(root);
        delete NIL;
    }

    // ---------- ПОИСК ----------
    // Обычный поиск, как в любом BST — КЧД остаётся BST по структуре.
    bool search(int key) const {
        Node* x = root;
        while (x != NIL) {
            if (key == x->key) return true;
            x = (key < x->key) ? x->left : x->right;
        }
        return false;
    }

    // ---------- ВСТАВКА ----------
    void insert(int key) {
        // Шаг 1: обычная BST-вставка, запоминая родителя по пути
        Node* y = NIL;        // будущий родитель нового узла
        Node* x = root;
        while (x != NIL) {
            y = x;
            x = (key < x->key) ? x->left : x->right;
        }

        Node* z = new Node{key, RED, NIL, NIL, y}; // новый узел — ВСЕГДА красный

        if (y == NIL) {
            root = z;                 // дерево было пустым
        } else if (key < y->key) {
            y->left = z;
        } else {
            y->right = z;
        }

        // Шаг 2: восстановить свойства КЧД (могло нарушиться свойство 4)
        insertFixup(z);
    }

    // ---------- УДАЛЕНИЕ ----------
    void remove(int key) {
        Node* z = root;
        while (z != NIL && z->key != key) {
            z = (key < z->key) ? z->left : z->right;
        }
        if (z == NIL) {
            return; // ключа нет — ничего не делаем
        }

        Node* y = z;                  // y — узел, который физически удаляется
        Color yOriginalColor = y->color;
        Node* x;                      // x — узел, который встаёт на место y

        if (z->left == NIL) {
            // нет левого ребёнка (либо вообще лист, либо только правый) —
            // просто подвешиваем правое поддерево на место z
            x = z->right;
            transplant(z, z->right);
        } else if (z->right == NIL) {
            // только левый ребёнок — симметрично
            x = z->left;
            transplant(z, z->left);
        } else {
            // два ребёнка: ищем преемника (минимум правого поддерева),
            // копируем его значение в z, а реально удаляем сам преемник
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z) {
                // преемник — непосредственный правый ребёнок z
                x->parent = y; // важно даже если x == NIL, для deleteFixup
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color; // преемник занимает место z, включая его цвет
        }

        delete z;

        // Если удалённый/перемещённый узел был ЧЁРНЫМ — свойство 5
        // (одинаковая чёрная высота) могло нарушиться, нужно исправлять.
        // Если он был красным — удаление красного узла никогда не
        // нарушает ни одно из 5 свойств, поэтому фиксап не нужен.
        if (yOriginalColor == BLACK) {
            deleteFixup(x);
        }
    }

    // ---------- ВЫВОД ----------
    void inorder() const {
        inorderHelper(root);
        std::cout << "\n";
    }

    void printTree() const {
        if (root == NIL) {
            std::cout << "(дерево пустое)\n";
            return;
        }
        std::cout << root->key << (root->color == RED ? " [RED]" : " [BLACK]") << " (root)\n";
        printHelper(root->left, "", true);
        printHelper(root->right, "", false);
    }

    // Проверка всех свойств КЧД — удобно для самопроверки на тестах.
    bool verifyProperties() const {
        if (root == NIL) return true;
        if (root->color != BLACK) return false;        // свойство 2
        return blackHeightHelper(root) != -1;            // свойства 4 и 5
    }
};

// =====================================================================
//                           ДЕМОНСТРАЦИЯ
// =====================================================================
int main() {
    RedBlackTree tree;

    int keysToInsert[] = {10, 20, 30, 15, 25, 5, 1, 40, 35, 22};

    std::cout << "=== Вставляем ключи по одному ===\n";
    for (int key : keysToInsert) {
        tree.insert(key);
        std::cout << "Вставили " << key << ", свойства КЧД "
                  << (tree.verifyProperties() ? "сохранены" : "НАРУШЕНЫ!!!") << "\n";
    }

    std::cout << "\n=== Дерево после всех вставок ===\n";
    tree.printTree();

    std::cout << "\n=== Симметричный обход (должен быть отсортирован) ===\n";
    tree.inorder();

    std::cout << "\n=== Поиск ===\n";
    std::cout << "Есть ли 25? " << (tree.search(25) ? "да" : "нет") << "\n";
    std::cout << "Есть ли 99? " << (tree.search(99) ? "да" : "нет") << "\n";

    std::cout << "\n=== Удаляем несколько ключей ===\n";
    int keysToDelete[] = {20, 5, 30};
    for (int key : keysToDelete) {
        tree.remove(key);
        std::cout << "Удалили " << key << ", свойства КЧД "
                  << (tree.verifyProperties() ? "сохранены" : "НАРУШЕНЫ!!!") << "\n";
    }

    std::cout << "\n=== Дерево после удалений ===\n";
    tree.printTree();

    std::cout << "\n=== Симметричный обход после удалений ===\n";
    tree.inorder();

    return 0;
}
