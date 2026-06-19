#include <iostream>
// максимально допустимое количество элементов, при котором
// узел не является переполненным.
#define LIMIT 2
struct KeyContainer{
    int key;
    KeyContainer* next=nullptr;
};
 
struct Page;
struct AddrContainer{
    Page* page=nullptr;
    AddrContainer* next = nullptr;
};
 
struct Page{
    KeyContainer* keys = nullptr;
    AddrContainer* addrs = nullptr;
};
 
int list_len(Page* page){
    KeyContainer * cur =page->keys;
    int count = 0;
    while (cur){
        cur=cur->next;
        count ++;
    }
    return count;
}
 
// Если после добавления узел не является переполненным,
// возвращает 0
// иначе 1
int add_to_leaf(Page* list_page, int value){
    KeyContainer* new_key_cont = new KeyContainer;
    new_key_cont->key = value;
    if (list_page){
        if (!list_page->keys){
            list_page->keys = new_key_cont;
            return 0;
        }else{
            if (value < list_page->keys->key){
                new_key_cont->next = list_page->keys;
                list_page->keys = new_key_cont;
            }else{
                // TODO добавление нового контейнера в середину или конец списка
                KeyContainer* cur = list_page->keys;
                while(cur->next && cur->next->key < value){
                    cur = cur->next;
                }
                new_key_cont->next = cur->next;
                cur->next = new_key_cont;                
            }
        }        
    }
    if (list_len(list_page) > LIMIT){
        return 1;
    }
    return 0; 
}
 
void split_leaf(Page* leaf, KeyContainer*& median, Page*& new_page){
    KeyContainer* current = leaf->keys;
    for (int i = 0; i<LIMIT /2-1; i++){
        current = current->next;
    }
    median = current->next;
    current->next = nullptr;
    new_page = new Page;
    new_page->keys = median->next;
    median->next= nullptr;
}
 
int add_to_page(Page* node, int value);
int add_to_node(Page* node, int value){
    //  TODO Вычислить потомка, которому рекурсивно передать это значение
    //  TODO Передать значение потомку рекурсивно.
    //  TODO Если вернётся 1, то
        //  TODO Вызвать split для этого потомка. На выходе получить контейнер с центральным элементом
        //       и новую страницу.
        //  TODO Добавить контейнер в keys в вычисленную позицию
        //  TODO а новую страницу добавить в addr
    //  Если количество ключей в node превышает LIMIT,
        //  то вернуть 1
        //  иначе вернуть 0
    int result;
    if (value < node->keys->key){ //  Случай рекурсивного вызова самому левому потомку
        //int result;
        result = add_to_page(node->addrs->page, value);
        if (result){
            
            // TODO Сплит для потомка
            // Добавить централльный элемент потомка в цепочку ключей
            // Добавить нового потомка вторым номером.
        }            
    }else{
        KeyContainer* cur = node->keys;
        AddrContainer* curAddr = node->addrs->next;
        while(cur->next && cur->next->key < value){
            cur = cur->next;
            curAddr = curAddr->next;
        }
        result = add_to_page(curAddr->page, value);
        if (result){
            // TODO Сплит для потомка
            // Добавть централльный элемент потомка в цепочку ключей
            // Добавить нового потомка после curAddr.
        }         
    }
    return 0;
}
 
int add_to_page(Page* node, int value){
    if (node->addrs){ // Если 
        return add_to_node(node,  value);
    }else{
        return add_to_leaf(node,  value);
    }
}
 
void printPage(Page* node) {
    if (!node) {
        return;
    }
 
    KeyContainer* cur = node->keys;
    while (cur) {
        std::cout << cur->key << " ";
        cur = cur->next;
    }
    std::cout << std::endl;
}
 
int main(){
    Page* root = new Page;
    add_to_leaf(root, 10);
    add_to_leaf(root, 20);
    add_to_leaf(root, 30);
 
    printPage(root);
}
