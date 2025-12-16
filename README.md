# Advent of code, as solved by yours truly.

The goals with this project were:

- do as much without as possible without relying on external libraries, except for parts which aren't part of the
  assignment.
- solve everything in different ways, using different thought processes.
- don't look up algorithms, just use rational thinking.
- Use modern c++23, with ranges, format, compile-time programming, etc.
    - `<format>` and `<print>` instead of `<iostream>`
    - `<ranges>` instead of imperative loops
    - templated functions instead of virtual stuff
- when writing code, always think about memory usage.
    - Don't pass a vector by value if you just intend to read from it.
    - Don't store in intermediate containers if an iteration pipeline is possible (std::ranges).
    - Use sensible collection types. std::unordered_map instead of std::map, if sorting is not needed. std::array if
      size is known at compile time.
- Good practices in terms of const-ness.
    - Everything that _can_ be const, _should_ be.
    - Const-qualified member functions.
- I like using `auto` where sensible. This is just for fun, there isn't really an advantage except in templated
  functions.
- Use correct initializers: `some_struct x{1, 2, 3};` and not `some_struct x(1, 2, 3)` or
  `some_struct x = some_struct(1, 2, 3)`. This has to do with implicit conversions etc.
- Format code using clang-format, and do it often. I actually sometimes turn on format-on-save.
- clang-tidy should find NOTHING. If you disagree with a lint for good reasons, suppress it. The point is, don't ignore
  them.

Because of this, I often didn't reuse code between part 1 and part 2 of a day, so I could solve the same task from two
different perspectives. Oftentimes, this was necessary anyway because I couldn't have known what part 2 would entail.
Trying to write code mode generic and reusable is fun, but only for parts that I know won't change.

## For example, input parsing:

The earlier days were parsed using iostream operators, hard-coded string offsets, and direct iteration over the string.
I would usually follow the same pattern:

```c++
std::ifstream f{"sample.txt"};
int a, b;
char comma;
while (f >> a >> comma >> b) {
    // ...
}
```

The good thing is that this never required the entire file in memory.

With some problems, parsing line by line and then doing something with the strings was preferred:

```c++
std::ifstream f{"sample.txt"};
for (std::string line; std::getline(f, line);) {
    // ...
}
```

This is pretty cool pattern I've grown to like.

### Lexy

For later problems, I wanted to try a parser dsl. [lexy](https://lexy.foonathan.net/) is pretty cool, as it does not
require you to declare a grammar in a separate file, but you do it directly in c++ using the provided dsl.

This is really cool because you get to construct your own ast data types directly, and have strong types at compile
time. Not just during compilation, but the IDE's language support understands it, too.

In addition, when your grammar doesn't have branching rules where it should, it will also be an error.
Even with a descriptive error message in the static_assert.

I don't think I fully learned to use lexy correctly, but it worked.

Examples can be seen in [d10](d10/model.hpp), [d11](d11/model.hpp), and [d12](d12/model.hpp).

Anyway, these were the productions for day 11:

```c++
namespace grammar {
namespace dsl = lexy::dsl;

struct id {
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha);
    static constexpr auto value = lexy::as_string<std::string>;
};

struct to {
    static constexpr auto rule = dsl::list(dsl::p<id>);
    static constexpr auto value = lexy::as_list<std::vector<std::string>>;
};

struct server {
    static constexpr auto whitespace = dsl::ascii::blank | dsl::ascii::newline;
    static constexpr auto rule = dsl::p<id> + dsl::colon + dsl::p<to>;
    static constexpr auto value = lexy::construct<ast::server>;
};
} // namespace grammar
```

It's pretty cool, using SFINAE to figure everything out. This is how fields like `whitespace` or `value` are entirely
optional. Well, `value` is only optional if you intend on only validating input, it is of course required for producing
data using the `parse` function.

Again, this could've easily been parsed manually, but I personally am a huge fan of grammars and parser generators. Or
in this case, a parser dsl.

This reminds me of [lexgen](https://github.com/osa1/lexgen)
and [parsegen](https://github.com/osa1/parsegen). They are lexing and parsing dsls for rust, using procedural macros.
That is really cool because you can define a grammar pretty declaratively, using a custom syntax, but still have
compile-time types, and can use your own AST datastructures (instead of having to iterate over a generic, non-typed
tree). I can recommend.

## SCIP

For day 10 part 2, I thought I'd try using a MILP solver, like scip(soplex). Was it necessary? No. But it was fast, and
it was new-to-me technology. Getting it to link and compile was a PAIN, it did require switching to WSL instead of
msys2. That's always the best solution anyway, but using msys2 was a snappier experience in the IDE, for some reason.

Either way, d10p2 was the perfect candidate: It was literally a linear equation system. Except with integers. And
multiple solutions, where the smallest was wished for. As I later learned, this is exactly what Mixed-Integer Linear
Programming (MILP) is. (Get your mind out of the gutter.)

## Day 12

This one was a huge troll. I knew the problem was NP-hard, because it's literally just a polyomino packing problem, but
I didn't know HOW hard it really was. NP-hard doesn't mean anything if you can optimize your input and reduce the
problem to fewer variables.

A more experienced person would've known to not even try, it would have been a safe assumption to make that "this can't
possibly be what he wants us to do". And that assumption would have been correct.

In the end, the gifts you needed to fit either TRIVIALLY didn't fit, or TRIVIALLY fit.

In the former case, counting up the total cells covered by the presents, regardless of their shape, resulted in a number
bigger than the field.

In the latter case, placing the presents next to each other as if they were 3x3 squares, without any overlap, was
possible.

Only areas that didn't meet those two conditions would have to be tested using a SAT or MILP solver.

Guess how many that was? Zero.

There go about 5h of my day. And it didn't even work anyway. I mean I was probably pretty close but I realized pretty
quickly that the solver was taking too long.

This is the only required code in the end:

```c++
// <-- ...parsing...

int skip_count = 0;
int guarantee_count = 0;

for (int i_tree = 0; i_tree < result.trees.size(); ++i_tree) {
    // <-- ...some setup code...
    
    const int total_count = std::ranges::fold_left(
            used_presents | std::views::transform(&ast::present::cell_count), 0, std::plus{});

    if (total_count > tree.w * tree.h) {
        std::println("Skipping {} as it definitely doesn't fit {}>{}",
                i_tree, total_count, tree.w * tree.h);
        ++skip_count;
        continue;
    }
    
    if (used_presents.size() <= (tree.w / 3) * (tree.h / 3)) {
        std::println("{} easily fits without overlaps", i_tree);
        ++guarantee_count;
        continue;
    }
    
    // <-- very expensive function goes here.
}

std::println("Skipped {}", skip_count);
std::println("Guaranteed {}", guarantee_count);
std::println("Remaining: {}", result.trees.size() - skip_count - guarantee_count);

```

And this was the output (for my data, note that input data is different for every user);

```
Skipped 505
Guaranteed 495
Remaining: 0
```
