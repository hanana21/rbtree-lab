#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
    rbtree *t = (rbtree*)calloc(1,sizeof(rbtree));
    node_t *nil =(node_t*)calloc(1,sizeof(node_t));
    nil->color = RBTREE_BLACK;
    t->nil = t->root = nil; //빈트리일때 root는 nil노드여야함
    return t;
}

void delete_rbtree(rbtree *t) {
    node_t* node = t->root;
    if (node != t->nil){
        traverse_and_delete_node(t,node);
    }
    free(t->nil);
    free(t);
}
void traverse_and_delete_node(rbtree* t, node_t* node) {
    if (node->left != t->nil) {
        traverse_and_delete_node(t, node->left);
    }
    if (node->right != t->nil) {
        traverse_and_delete_node(t, node->right);
    }
    free(node);
}
node_t *rbtree_insert(rbtree *t, const key_t key) {
    //새 노드 만들기
    node_t* new_node = (node_t*)calloc(1,sizeof(node_t));
    new_node -> key = key;
    new_node -> color = RBTREE_RED;
    new_node -> left = new_node ->right = t->nil;
    //노드 삽입 할 위치 찾기 
    node_t* current = t->root;
    while (current != t->nil){
        if(key<current->key){
            if(current->left == t->nil){ //자식 없으면 걍 넣기
                current->left = new_node;
                break;
            }
            current = current->left; 
        }else{
             if(current->right == t->nil){
                current->right = new_node;
                break;
            }
            current = current->right;
        }
    }
    new_node->parent = current;
    //빈트리라면 루트노드로 설정
    if (current == t->nil){
        t->root = new_node;
    }
    rbtree_insert_fixup(t,new_node);
    
    return new_node;
}
//색깔 바꾸기 
void exchange_color(node_t* a, node_t* b){
    int tmp = a->color;
    a->color = b->color;
    b->color = (tmp ==RBTREE_BLACK)? RBTREE_BLACK:RBTREE_RED;
}
// 삽입 노드 불균형 해결
void rbtree_insert_fixup(rbtree* t, node_t* Node){
    node_t* parent = Node->parent;
    node_t* grand_parent = parent -> parent;
    node_t* uncle;
    int is_left = Node ==parent->left; // 현재노드 왼쪽 오른쪽 확인
    int is_parent_is_left; // 부모 왼쪽 오른쪽 확인

    //추가된 노드가 root노드인 경우: 색만 변경
    if(Node == t->root){
        Node -> color = RBTREE_BLACK;
        return;
    }
    //부모가 black인 경우: 변경 없음 
    if(parent->color==RBTREE_BLACK){
        return;
    }
    is_parent_is_left = grand_parent->left == parent; //부모가 왼쪽 오른쪽 판단
    uncle =(is_parent_is_left)?grand_parent->right:grand_parent->left; // 부모위치에 따라 삼촌 위치 판단
    //부모와 부모의 형제가 RED인 경우
    if(uncle->color == RBTREE_RED){
        parent ->color = RBTREE_BLACK;
        uncle -> color = RBTREE_BLACK;
        grand_parent -> color = RBTREE_RED;
        rbtree_insert_fixup(t,grand_parent);
        return;
    }
    if(is_parent_is_left){
        //부모형제가 black, 부모가 왼쪽 자식, 현재노드가 왼쪽 자식
        if(is_left){
            right_rotate(t,parent);
            exchange_color(parent,parent->right);
            return;
        }
        //부모의 형제가 black, 부모가 왼쪽 자식, 현재 노드가 오른쪽 자식
        left_rotate(t,Node);
        right_rotate(t,Node);
        exchange_color(Node,Node->right);
        return;
    }
    //삼촌 black,부모가 오른쪽, 현재노드 왼쪽 
    if(is_left){
        right_rotate(t,Node);
        left_rotate(t,Node);
        exchange_color(Node,Node->left);
        return;
    }
    //삼촌 black,부모가 오른쪽, 현재노드 오른쪽 
    left_rotate(t,parent);
    exchange_color(parent,parent->left);
}
//오른쪽 회전
void right_rotate(rbtree* t, node_t* Node){
    node_t* parent = Node -> parent;
    node_t* grand_parent = parent->parent;
    node_t* node_right = Node->right;

    if(parent == t->root){
        t->root = Node;
    }else{
        if(grand_parent->left == parent){
            grand_parent -> left = Node;
        }else{
            grand_parent -> right =Node;
        }
    }
    Node -> parent = grand_parent;
    parent -> parent = Node;
    Node-> right = parent;
    node_right -> parent = parent;
    parent -> left = node_right;
}

//왼쪽으로 회전 
void left_rotate(rbtree* t, node_t* Node){
    node_t* parent = Node -> parent;
    node_t* grand_parent = parent->parent;
    node_t* node_left = Node->left;

    if (parent == t->root){
        t->root = Node;
    }else{
        if(grand_parent->left == parent){
            grand_parent->left = Node;
        }else{
            grand_parent->right = Node;
        }
    }
    Node -> parent = grand_parent;
    parent -> parent = Node;
    Node-> left = parent;
    parent -> right = node_left;
    node_left ->parent = parent;
}
node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *current  = t->root;
  while(current != t->nil){
    if(key == current -> key){
      return current;
    }else{
      current = (key < current->key)? current->left:current->right;
    }
  }
  return NULL;
}

node_t *rbtree_min(const rbtree *t) {
  node_t* current = t-> root;
  while(current->left != t->nil){
    current = current->left;
  }
  return current;
}

node_t *rbtree_max(const rbtree *t) {
  node_t* current = t-> root;
  while(current->right != t->nil){
    current = current->right;
  }
  return current;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}
//t를 n번 순회한 결과를 arr에 담는 함수 
int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  node_t* current = rbtree_min(t);
  arr[0] = current->key;
  for(int i=1; i<n; i++){
    if(current == t->nil)
      break;
    current = get_next_node(t,current);
    if(current == t->nil)
      break;
    arr[i] = current->key;
  }
  return 0;
}
node_t* get_next_node(const rbtree* t, node_t *p){
  node_t* current = p->right;
  if(current == t->nil){//오른쪽 자식이 없다면
    current = p;
    while(1){
      if(current->parent->right == current){
        current = current->parent;
      }else{
        return current->parent;
      }
    }
  }  
  while(current->left != t->nil){
      current = current->left;
    }
  return current;
}
