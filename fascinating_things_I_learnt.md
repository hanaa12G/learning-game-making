# Fascinating things I learnt while I was doing this project


## C++

This code has fault:
``` C++
  struct Foo {
    BigObject* obj {nullptr};

    Foo(Foo&&) = default;
  };

  // usage code
  int main() {
    std::vector<Foo> vec;
    Foo foo;
    vec.push_back(std::move(foo));
  }
```

**My expectation**: We moved `foo` to `vector`, so `Foo::obj` should be moved too, it means we should swap pointer. `foo.obj` will become invalid (null), newly created `Foo` in vector will have the pointer to `BigObject` instead.

Actually, when call move constructor on `Foo`, compiler's generated move constructor is called (because we use `=default`), every members of `Foo` in new object will be called with move constructor too. This means compiler calls move constructor on `Foo::obj`. When this is a simple value (pointer, int, etc.), this operation is similar to a copy. Thus `foo` keeps pointer after move and we have use-after-free segfault when we use object in `vec` later.

**Lesson**: This is basically rule of five in C++.