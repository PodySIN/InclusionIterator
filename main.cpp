#include <iostream>
#include <tuple>
#include <utility>

template< class T >
struct BiTree {
  T val;
  BiTree< T > *lt, *rt, *parent;
};

enum Dir {
  fall_left,
  parent_dir
};

template< class T >
std::pair< size_t, BiTree< T >* > fallLeft(BiTree< T >* root)
{
  if (!root) {
    return { 0, nullptr };
  }
  size_t path = 0;
  while (root->lt) {
    root = root->lt;
    path++;
  }
  return { path, root };
}

template< class T >
std::pair< size_t, BiTree< T >* > parent(BiTree< T >* root)
{
  if (!root) {
    return { 0, nullptr };
  }
  size_t path = 0;
  BiTree< T >* par = root->parent;
  while (par && par->lt != root) {
    root = par;
    par = root->parent;
    path++;
  }
  return { path, par };
}

template< class T >
std::tuple< Dir, size_t, BiTree< T >* > nextStruct(BiTree< T >* root)
{
  if (!root) {
    return { Dir::parent_dir, 0, nullptr };
  }
  if (root->rt) {
    std::pair< size_t, BiTree< T >* > r = fallLeft(root->rt);
    return { Dir::fall_left, r.first, r.second };
  }
  std::pair< size_t, BiTree< T >* > r = parent(root);
  return { Dir::parent_dir, r.first, r.second };
}

template< class T >
BiTree< T >* prev(BiTree< T >* curr)
{
  if (!curr) {
    return nullptr;
  }
  BiTree< T >* next = curr;
  if (next->lt) {
    next = next->lt;
    while (next->rt) {
      next = next->rt;
    }
  } else {
    BiTree< T >* par = next->parent;
    while (par && par->rt != next) {
      next = par;
      par = next->parent;
    }
    next = par;
  }
  return next;
}

template< class T >
std::tuple< BiTree< T >*, BiTree< T >*, bool > isEqualStructStart(BiTree< T >* lhs_start, BiTree< T >* rhs_start)
{
  auto ln = nextStruct(lhs_start);
  auto rn = nextStruct(rhs_start);
  while (std::get< 0 >(ln) == std::get< 0 >(rn) &&
        std::get< 1 >(ln) == std::get< 1 >(rn) &&
        std::get< 2 >(ln) && std::get< 2 >(rn)) {
    ln = nextStruct(std::get< 2 >(ln));
    rn = nextStruct(std::get< 2 >(rn));
  }
  bool dir = (std::get< 0 >(ln) == std::get< 0 >(rn));
  return {std::get< 2 >(ln), std::get< 2 >(rn), dir};
}

template< class T >
bool isEqualStruct(BiTree< T >* lhs, BiTree< T >* rhs)
{
  auto lhs_begin = fallLeft(lhs);
  auto rhs_begin = fallLeft(rhs);
  if (lhs_begin.first != rhs_begin.first) {
    return false;
  }
  auto result = isEqualStructStart(lhs_begin.second, rhs_begin.second);
  return !std::get< 0 >(result) && !std::get< 1 >(result);
}

template< class T >
std::pair< BiTree< T >*, BiTree< T >* > inclusion(BiTree< T >* lhs, BiTree< T >* pattern)
{
  BiTree< T >* lhs_curr = fallLeft(lhs).second;
  while (lhs_curr) {
    auto result = isEqualStructStart(lhs_curr, fallLeft(pattern).second);
    if (!std::get< 1 >(result) && std::get< 2 >(result)) {
      BiTree< T >* last_lhs_next = std::get< 0 >(result);
      BiTree< T >* lhs_end;
      if (!last_lhs_next) {
        lhs_end = lhs;
        while (lhs_end->rt) {
          lhs_end = lhs_end->rt;
        }
      } else {
        lhs_end = prev(last_lhs_next);
      }
      return {lhs_curr, lhs_end};
    }
    lhs_curr = std::get< 2 >(nextStruct(lhs_curr));
  }
  return {nullptr, nullptr};
}

template< class T >
struct InclusionIt {
  std::pair< BiTree< T >*, BiTree< T >* > incl;
};

template< class T >
InclusionIt< T > begin(BiTree< T >* lhs, BiTree< T >* pattern) {
  return { inclusion(lhs, pattern) };
}

template< class T >
InclusionIt< T > next(InclusionIt< T > curr, BiTree< T >* pattern) {
  if (!curr.incl.first) {
    InclusionIt< T > empty;
    empty.incl = {nullptr, nullptr};
    return empty;
  }
  BiTree< T >* start = std::get< 2 >(nextStruct(curr.incl.first));
  BiTree< T >* pattern_start = fallLeft(pattern).second;
  while (start) {
    auto result = isEqualStructStart(start, pattern_start);
    if (!std::get< 1 >(result) && std::get< 2 >(result)) {
      BiTree< T >* last_lhs_next = std::get< 0 >(result);
      BiTree< T >* new_end;
      if (!last_lhs_next) {
        BiTree< T >* root = start;
        while (root->parent) {
          root = root->parent;
        }
        new_end = root;
        while (new_end->rt) {
          new_end = new_end->rt;
        }
      } else {
        new_end = prev(last_lhs_next);
      }
      InclusionIt< T > it;
      it.incl = {start, new_end};
      return it;
    }
    start = std::get< 2 >(nextStruct(start));
  }
  InclusionIt< T > empty;
  empty.incl = {nullptr, nullptr};
  return empty;
}

template< class T >
bool hasNext(InclusionIt< T > curr, BiTree< T >* pattern) {
  return next(curr, pattern).incl.first;
}

template< class T >
BiTree<T>* createNode(T val) {
  BiTree<T>* node = new BiTree<T>();
  node->val = val;
  node->lt = nullptr;
  node->rt = nullptr;
  node->parent = nullptr;
  return node;
}

