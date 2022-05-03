/*
[Rule]
1. 모든 노드는 red or black
2. 루트 노드는 black
3. 모든 nil(leaf) 노드는 black
4. red의 자녀들은 black (red는 연속으로 존재 불가)
5. 임의의 노드에서 자손 nil노드들까지 가는 경로 상의 black height는 동일
*/

#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  node_t *nilNode = (node_t *)calloc(1, sizeof(node_t));

  nilNode->color = RBTREE_BLACK;
  p->root = nilNode;
  p->nil = nilNode;

  return p;
}

void left_rotation(rbtree *t, node_t *x){
  node_t *y = x->right;

  x->right = y->left;
  if (y->left != t->nil)
    y->left->parent = x;

  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;
  
  y->left = x;
  x->parent = y;
}

void right_rotation(rbtree *t, node_t *x){
  node_t *y = x->left;

  // y의 오른쪽 자식을 x의 왼쪽 자식으로 보내기
  x->left = y->right; // x->left는 y->right을 바라보게 해주고
  if (y->right != t->nil) // nil이 아닐 경우에는
    y->right->parent = x; // y->right의 부모가 x라는 것을 알려주기(이걸 해주기 전까지 y->right의 부모는 여전히 y임)
  
  // 기존 x의 부모를 y의 부모로 넘겨주기(y를 위로 올려주는 것)
  y->parent = x->parent;
  if (x->parent == t->nil)
    t->root = y;
  else if (x == x->parent->left) // x가 부모의 왼쪽 자식이었다면
    x->parent->left = y;         // 이제는 y를 x 부모의 왼쪽 자식으로
  else                           // x가 부모의 오른쪽 자식이었다면
    x->parent->right = y;        // 이제는 y를 x 부모의 오른쪽 자식으로
  
  // 뒤바뀐 관계에 따라 x와 y를 연결
  y->right = x;
  x->parent = y;
}

void free_node(rbtree *t, node_t *x) {
  if (x->left != t->nil) 
    free_node(t, x->left);
  if (x->right != t->nil)
    free_node(t, x->right);
  free(x);
  x = NULL;
}

void delete_rbtree(rbtree *t) {
  if (t->root != t->nil)
    free_node(t, t->root);
  free(t->nil);
  t->nil = NULL;
  free(t);
  t = NULL;
}

void rbtree_insert_fixup(rbtree *t, node_t *z) {
  node_t *y;
  
  // 삽입된 red 노드 z의 부모가 red(즉, 부모와 자식이 모두 red로 rule 4 위반)
  while (z->parent->color == RBTREE_RED) {
    // case 1~3: z의 부모가 조부모의 왼쪽 자식일 경우
    if (z->parent == z->parent->parent->left) {
      y = z->parent->parent->right; // 삼촌은 오른쪽 자식
      // case 1 : z의 삼촌 y가 red인 경우
      if (y->color == RBTREE_RED) {
        // 부모와 삼촌의 color을 조부모의 color와 교환(rule 5 위반 x)
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent; // 조부모 node에서부터 다시 while문 확인
      }
      else {
        // case 2 : z의 삼촌 y가 black이고 z가 오른쪽 자식인 경우
        if (z == z->parent->right) {
          z = z->parent;
          left_rotation(t, z); // z의 부모를 기준으로 왼쪽으로 회전
        }
        // case 3 : 삼촌 y가 black이고 z가 왼쪽 자식인 경우
        z->parent->color = RBTREE_BLACK; // 부모의 color : red -> black
        z->parent->parent->color = RBTREE_RED; // 조부모의 color : black -> red
        right_rotation(t, z->parent->parent); // 조부모를 기준으로 오른쪽 회전
      }
    }
    // case 4~ 6 : z의 부모가 조부모의 오른쪽 자식일 경우
    else {
      y = z->parent->parent->left;
      // case 4
      if (y->color == RBTREE_RED) {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      // case 5
      else { 
        if (z == z->parent->left) {
          z = z->parent;
          right_rotation(t, z);
        }
        // case 6
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotation(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) { 
  node_t *y = t->nil;
  node_t *x = t->root;
  node_t *z = (node_t *)calloc(1, sizeof(node_t));

  z->key = key;

  while (x != t->nil) {
    y = x;
    if (z->key < x->key)
      x = x->left;
    else
      x = x->right;
  }

  z->parent = y;

  if (y == t->nil)
    t->root = z;
  else if (z->key < y->key)
    y->left = z;
  else
    y->right = z;
  
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED;

  rbtree_insert_fixup(t, z);

  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  node_t *curr = t->root;

  while (curr != t->nil) {
    if (curr->key == key) {
      return curr; // key를 찾으면 현재 노드 curr을 반환
    }
    if (key < curr->key)
      curr = curr->left;
    else
      curr = curr->right;
  }
  return NULL; // key를 못찾으면 NULL 반환
}

node_t *rbtree_min(const rbtree *t) {
  if (t->root == t->nil) {
    return NULL;
  }
  node_t * curr = t->root;

  while (curr->left != t->nil) {
    curr = curr->left;
  }
  return curr;
}

node_t *rbtree_max(const rbtree *t) {
  if (t->root == t->nil) {
    return NULL;
  }

  node_t * curr = t->root;

  while (curr->right != t->nil) {
    curr = curr->right;
  }
  return curr;
}

node_t *find_successor(rbtree *t, node_t *curr) {
  while (curr->left != t->nil) {
    curr = curr->left;
  }
  return curr;
}

// 서브트리 이동을 위해 노드 u가 루트인 서브트리를 노드 v가 루트인 서브트리로 교체
void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) // u의 부모가 nil 즉, u가 루트노드라면
    t->root = v;           // v를 트리의 루트노드로 삼는다
  else if (u == u->parent->left)  // u가 부모의 왼쪽 자식일 경우
    u->parent->left = v;          // v를 왼쪽 자식으로 이식(u를 대체)
  else                            // 오른쪽 자식일 경우
    u->parent->right = v;         // v를 오른쪽 자식으로 이식
  
  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x) {
  while (x != t->root && x->color == RBTREE_BLACK) {
    // case 1 ~ 4 : left case
    if (x == x->parent->left){
      node_t *w = x->parent->right;
      
      // case 1 : x의 형제 w가 red인 경우
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotation(t, x->parent);
        w = x->parent->right;
      }

      // case 2 : x의 형제 w는 black이고 w의 두 지식이 모두 black인 경우
      if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }

      // case 3 : x의 형제 w는 black, w의 왼쪽 자식은 red, w의 오른쪽 자식은 black인 경우
      else{ 
        if (w->right->color == RBTREE_BLACK) {
            w->left->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            right_rotation(t, w);
            w = x->parent->right;
        }

        // case 4 : x의 형제 w는 black이고 w의 오른쪽 자식은 red인 경우
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotation(t, x->parent);
        x = t->root;
      }
    }
    // case 5 ~ 8 : right case
    else {
      node_t *w = x->parent->left;

      // case 5 : x의 형제 w가 red인 경우
      if (w->color == RBTREE_RED){
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotation(t, x->parent);
        w = x->parent->left;
      }

      // case 6 : x의 형제 w는 black이고 w의 두 자식이 모두 black인 경우
      if (w->right->color == RBTREE_BLACK && w->left->color == RBTREE_BLACK) {
        w->color = RBTREE_RED;
        x = x->parent;
      }

      // case 7 : x의 형제 w는 black, w의 왼쪽 자식은 red, w의 오른쪽 자식은 black인 경우
      else 
      {
          if (w->left->color == RBTREE_BLACK) {
            w->right->color = RBTREE_BLACK;
            w->color = RBTREE_RED;
            left_rotation(t, w);
            w = x->parent->left;
            }

        // case 8 : x의 형제 w는 black이고 w의 오른쪽 자식은 red인 경우
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotation(t, x->parent);
        x = t->root;
      }
    }
  }

  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  node_t *y;
  node_t *x;
  color_t y_original_color;

  // 삭제하려는 노드 p의 정보를 우선 y에 다 저장해두자(y가 p를 기준으로 시작하지만 중간에 바뀔 수 있음)
  y = p;
  y_original_color = y->color;

  // 노드 p에게 유효한 값을 가진 자식이 하나 있는데 그 자식이 오른쪽에 있는 경우(왼쪽 자식은 nil)
  if (p->left == t->nil) {
    x = p->right; // 오른쪽 자식을 x에 담아두고
    rbtree_transplant(t, p, p->right); // p의 오른쪽 자식을 p의 위치에 이식(transplant)하면서 p는 제거
  }
  // 유효한 값을 가진 자식이 왼쪽에만 하나 있는 경우
  else if (p->right == t->nil) {
    x = p->left;
    rbtree_transplant(t, p, p->left); // p의 왼쪽 자식을 p의 위치에 이식하면서 p는 제거됨
  }
  // 유효한 자식이 둘인 경우(왼쪽 자식도 nil이 아니고 오른쪽 자식도 nil이 아님)
  else {
    y = find_successor(t, p->right);
    y_original_color = y->color;
    x = y->right;

    if (y->parent == p) {
      x->parent = y;
    }
    else {
      rbtree_transplant(t, y, y->right);
      y->right = p->right;
      y->right->parent = y;
    }

    rbtree_transplant(t, p, y);
    y->left = p->left;
    y->left->parent = y;
    y->color = p->color;    
  }
  // 삭제되는 색이 black인 경우에만 속성 위반 여부 확인
  if (y_original_color == RBTREE_BLACK) {
    rbtree_delete_fixup(t, x);
  }

  free(p);

  return 0;
}

void subtree_to_array(const rbtree *t, node_t *curr, key_t *arr, size_t n, size_t *count) {
  if (curr == t->nil) {
    return;
  }
  
  subtree_to_array(t, curr->left, arr, n, count);
  if (*count < n) {
    arr[(*count)++] = curr->key;
  }
  else return;
  subtree_to_array(t, curr->right, arr, n, count);

}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  if (t->root == t->nil) {
    return 0;
  }

  size_t cnt = 0;
  subtree_to_array(t, t->root, arr, n, &cnt); 
  
  return 0;
}