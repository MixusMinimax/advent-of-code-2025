#include <print>

#include <lexy/action/parse.hpp>
#include <lexy/input/file.hpp>
#include <lexy_ext/report_error.hpp>

#include "model.h"

int main() {
    const auto file = lexy::read_file<lexy::utf8_encoding>("../../d12/sample.txt");
    // const auto file = lexy::read_file<lexy::utf8_encoding>("../../d12/assignment.txt");
    const auto result = lexy::parse<grammar::file>(file.buffer(), lexy_ext::report_error).value();
    std::println("{}", result);
    return 0;
}

/*


Core MILP Formulation (Feasibility Only)
Sets

Board cells:

G={(i,j)∣i=1..n, j=1..m}
G={(i,j)∣i=1..n, j=1..m}

Polyominos:

P={1,…,∣P∣}
P={1,…,∣P∣}

For each polyomino
p
p, precompute:

All allowed orientations (rotations/reflections)

All translations that fit in the board

Let:

Kp
K
p
​

= set of all valid placements of polyomino
p
p

K
K = union of all placements

Each placement
k∈K
k∈K is associated with:

a polyomino
p(k)
p(k)

a set of board cells
C(k)⊆G
C(k)⊆G

Decision Variables
xk∈{0,1}∀k∈K
x
k
​

∈{0,1}∀k∈K

Interpretation:

xk=1
x
k
​

=1 ⇔ placement
k
k is chosen

Constraints
1. Each polyomino is placed exactly once

For every polyomino
p
p:

∑k∈Kpxk=1
k∈K
p
​

∑
​

x
k
​

=1

This enforces “use all shapes exactly once.”

2. No overlapping placements

For every board cell
(i,j)
(i,j):

∑k: (i,j)∈C(k)xk≤1
k:(i,j)∈C(k)
∑
​

x
k
​

≤1

This prevents overlap.

Objective

No objective is needed:

min⁡0
min0

You are only checking feasibility.

Optional but Important Strengthening
Area sanity check (outside MILP)

Before solving, check:

∑p∈P∣p∣≤n⋅m
p∈P
∑
​

∣p∣≤n⋅m

If equality holds, you are looking for an exact tiling.

If total area matches board area

You may replace the overlap constraints with:

∑k: (i,j)∈C(k)xk=1∀(i,j)
k:(i,j)∈C(k)
∑
​

x
k
​

=1∀(i,j)

This makes the model much tighter.

Why This Works Well

This is an exact cover formulation:

Polyominos must be covered once

Grid cells may be covered at most once

MILP solvers are very good at this structure.

Practical Notes
Model size

The number of variables is:

∑p(orientationsp)×(valid translations)
p
∑
​

(orientations
p
​

)×(valid translations)

This grows fast with board size, but is manageable for many practical cases.

Symmetry breaking (optional)

To avoid equivalent solutions:

Fix one polyomino to one canonical placement:

xk0=1
x
k
0
​

​

=1

Or restrict its top-leftmost position.

This is not required for correctness, only speed.

*/
