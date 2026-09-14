*This project has been created as part of the 42 curriculum by dde-fite*

<div align="center">
    <img align="center" src="media/codexion.png" width="200" alt="Project logo">
    <h3>Dining Philosophers + making C OOP</h3>
</div>
<div align="center">
    	<img alt="Pending score at 42" src="https://img.shields.io/badge/pending/100-yellow?logo=42&logoColor=white">
		<img alt="MIT license" src="https://img.shields.io/badge/license-MIT-orange">
</div>

---

## Description

Codexion is a remake of the Dining Philosophers problem. `N` coders
sit in a circular co-working hub with `N` USB dongles on the table, one
between each pair of neighbors. Compiling takes two dongles, one per hand.
Each coder loops through compile, debug, refactor, then goes for the dongles
again. The simulation stops when a coder burns out (fails to start compiling
within `time_to_burnout` ms) or when every coder has compiled
`number_of_compiles_required` times.

On top of the textbook problem there are two extra rules: a released dongle
stays locked for its cooldown, and every dongle is assigned through a
scheduler, `fifo` (first come, first served) or `edf` (earliest deadline
first, deadline = last compile + `time_to_burnout`). A separate monitor
thread detects burnout and ends the simulation, and all output goes through
a single logger thread.

The code is POSIX threads plus C experimenting with object-oriented style.
Every model (hub, coder, usb, monitor, logger) is a struct with a
`create`/`init`/`reset`/`destroy` lifecycle, and the two schedulers are
interchangeable classes behind a single vtable. The Makefile builds with
`-O3 -march=native` and `-Werror` by default; `make DEBUG=1` gives `-g -O0`
without `-Werror`. There is no libft, just six `ft_*` helpers in
`src/utils/ft/`, and no globals beyond the static mutex and `burned` flag in
`print.c`.

## The OOP experiment

Structs as objects, function pointers as methods, and a
`create`/`init`/`reset`/`destroy` lifecycle that every model follows.

**Objects and lifecycles.** `create` allocates and calls `init`, `reset`
tears down what `init` built, `destroy` calls `reset` and frees. Ownership
stays manual; nothing frees itself.

**Inheritance and vtables.** The real experiment is the scheduler hierarchy.
`t_scheduler` is an abstract base: `__self` plus six function pointers,
`__put`, `__pop`, `__delete`, `__first`, `__reset`, and `__destroy`. `t_fifo`
and `t_edf` each embed a `t_scheduler super` as their first member, which is
how C fakes inheritance: the address of the derived struct is also the
address of the base struct.

`fifo__init` and `edf__init` wire their own function pointers into the
vtable, then `usb__init_scheduler` casts whichever one it created to
`t_scheduler *`. From there the usb only calls `scheduler__put`,
`scheduler__pop`, and the other dispatchers. The same call appends to a ring
buffer under `fifo` and sifts up a min-heap under `edf`, and the call site
never learns which one it got. Both queues are `t_coder *[MAX_CODERS]` with
`MAX_CODERS` set to 2 (each dongle has exactly two neighbors), so the `edf`
heap sifts at most one level. The heap is overkill, but it makes the vtable
look like a real scheduler.

The payoff: dongle and coder code only know the `t_scheduler` interface;
`usb__init_scheduler` is the only place that names `fifo` or `edf`. The
price: C checks none of it. A wrong function pointer in the vtable compiles
fine and blows up wherever the dispatcher happens to run. The nullability
macros add their own cost, in the shape of casts like
`(void *NULLABLE (*NONNULL)(void *NONNULL))monitor__th_start_routine` on the
`pthread_create` calls.

## Blocking cases handled

**Deadlock.** Every coder takes its dongles in the same global order:
`coder__th_compile` sorts its two dongles by pointer address before acquiring
either, so two neighbors can never hold the shared pair in opposite order
and Coffman's circular-wait condition can't form. The other three conditions
stay by design, and with circular wait broken there is no deadlock.

**Starvation.** `fifo` grants the dongle in arrival order; a coder only
touches it once it reaches the head of the queue, and later arrivals can't
cut in. `edf` keeps waiters in a min-heap ordered by deadline, so the coder
closest to burning out is always served first.

**Cooldown.** `usb__release_safe` stamps `__last_used` when a dongle is
released. The coder now at the head of the queue computes the deadline
`__last_used + dongle_cooldown` and blocks in `pthread_cond_timedwait` until
it passes, re-checking on every wakeup.

**Burnout detection.** A separate monitor thread polls every coder in a
tight loop with no sleep. A coder burns out when it is below its compile
quota and `time_to_burnout` ms have passed since its last compile (or since
the simulation started) without a new compile starting. The monitor queues
the `BURNED` log entry and calls `hub__end` immediately, so the burnout
message goes out well within the 10 ms the subject demands. The price is one
core spinning for the whole run (`MONITOR_TICK_US` in `monitor.h` goes
unused), plus reads of `__compiles` and `__last_compile` with no lock, a
data race on paper that has held up fine in practice.

**Log serialization.** Coders and the monitor never print. They push entries
onto a mutex-protected ring queue, and one logger thread drains it. That
thread is the only caller of `log_state`, which is what keeps two messages
from interleaving on one line. The static mutex inside `log_state` guards a
`burned` flag that silences output after a burnout, and it is released
before the `printf` itself.

**Shutdown without stuck threads.** `hub__end` is the only exit path and is
idempotent: if the flag is already down it returns, otherwise it clears it,
broadcasts the sim condition variable, wakes every dongle with
`usb__wake_safe`, and wakes the logger. Threads blocked in
`pthread_cond_wait` or `pthread_cond_timedwait` come back, see the
simulation is over, and clean up.

## Thread synchronization mechanisms

The program uses two primitives: `pthread_mutex_t` and `pthread_cond_t`.
No semaphores, no atomics. Each shared structure has its own pair:

- `t_usb`: `__mutex` and `__cond` guard the scheduler queue and the cooldown
  timestamp. Each dongle is shared by exactly two coders, so a queue holds at
  most two entries.
- `t_hub`: `__sim_mutex` and `__sim_cond` guard the running flag; `main`
  waits on them until the simulation ends.
- `t_logger`: `__mutex` and `__cond` guard the queue between producers and
  the logger thread.
- `print.c`: a static mutex guards the `burned` flag in `log_state`; the
  logger thread is its only caller.

**Dongle acquisition.** A coder locks the dongle's mutex, enqueues itself in
the scheduler, then waits on the condition variable until it is first in
line. While it sleeps the mutex is free, so the other neighbor keeps moving.
When the holder releases, `usb__release_safe` pops it from the queue, updates
`__last_used`, and broadcasts, waking every waiter to re-check head-of-line
under the mutex.

**Race example.** Two coders share one dongle and both want it. Whoever
locks the dongle's mutex first enqueues and finds itself at the head; the
other enqueues, finds it is not first, and sleeps on the condition variable.
A coder only proceeds once it has been head of the queue under the mutex.

**Cooldown wait.** The head-of-line coder converts the cooldown deadline to
a `timespec` and blocks in `pthread_cond_timedwait`, which releases the
mutex while it sleeps. A release or shutdown broadcast wakes it early, and
it re-checks the clock before proceeding.

**Coder and monitor communication.** The monitor only reads `__compiles` and
`__last_compile`; it never touches dongle state. When it decides the
simulation is over, `hub__end` flips the flag under `__sim_mutex` and
broadcasts. Coders check `hub__is_running` at every loop step and after
every wait, so they notice the shutdown and release any dongle they still
hold.

## Instructions
### Prerequisites
Must run on a POSIX OS (Linux, macOS, BSD...). Built and tested on macOS;
the `NONNULL`/`NULLABLE` annotations in `annotations.h` are checked by clang
and compile away on gcc.

For compiling:
- Clang or gcc
- make

### Quick build
```bash
git clone https://codeberg.org/noradefitero/42_Codexion.git
cd 42_Codexion
make all
```

### Usage
The block below is the program's own `help` output, verbatim, including the
`-\--` rendering and the `numbers_of_compiles_required` typo (both live in
the code, not in this README):
```bash
$> ./codexion help
codexion Usage

Usage:
    ./codexion -\--<command> <arguments>

Commands:
    version
        Prints program's version.
    help
        Prints all available options for using this program.

Arguments:
    number_of_coders
        Number of coders (and dongles).
    time_to_burnout
        Time in milliseconds a coder can wait before burning out.
    time_to_compile
        Time in milliseconds a coder spends compiling.
    time_to_debug
        Time in milliseconds a coder spends debugging.
    time_to_refactor
        Time in milliseconds a coder spends refactoring.
    numbers_of_compiles_required
        Simulation stops once every coder has compiled this many times.
    dongle_cooldown
        Time in milliseconds a dongle remains unavailable after release.
    scheduler
        Scheduler: fifo or edf.
```

#### Example of use
```bash
./codexion 20 100 18 25 20 10 5 fifo
```

#### Burning case
```bash
$> ./codexion 2 20 34 10 34 20 50 fifo
0 1 has taken a dongle
0 1 has taken a dongle
0 1 is compiling
21 2 burned out

```

## Resources
<!-- TODO(author): prune this list to the links you actually opened. A short
     list of real references reads more human than a full bibliography. -->
- [Dining philosophers problem at Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
### Manual pages
- macOS man pages
- [NetBSD man pages](https://man.netbsd.org/)
- [FreeBSD man pages](https://man.freebsd.org/cgi/man.cgi)
### Threading
- [Pthreads at Wikipedia](https://en.wikipedia.org/wiki/Pthreads)
- [Multithreading in C at GeeksforGeeks](https://www.geeksforgeeks.org/c/multithreading-in-c/)
- [Thread Management Functions in C at GeeksforGeeks](https://www.geeksforgeeks.org/c/thread-functions-in-c-c/)
- [Mastering Concurrency in C with Pthreads: A Comprehensive Guide at dev.to (Emanuel Gustafzon)](https://dev.to/emanuelgustafzon/mastering-concurrency-in-c-with-pthreads-a-comprehensive-guide-56je)
- [Mutexes in C at Medium (Sherniiazov Daniiar)](https://medium.com/@sherniiazov.da/mutexes-in-c-ac2b0f1a6d34)
### C OOP
- [Object-Oriented Programming (OOP) in C at Codementor (Michael Safyan)](https://www.codementor.io/@michaelsafyan/object-oriented-programming-in-c-du1081gw2)
- [C is an Object-Oriented language (almost) at dev.to (Ashton Warner)](https://dev.to/drflamemontgomery/c-is-an-object-oriented-language-almost-3hgi)
- [How to implement object-oriented pro­gram­ming (OOP) in C at IONOS](https://www.ionos.com/digitalguide/websites/web-development/oop-in-c/#content-how-can-a-c-project-be-rewritten-in-an-object-oriented-manner)
### Schedulers
- [Answer to “How do I implement a circular list (ring buffer) in C?” at Stack Overflow (Adam Davis)](https://stackoverflow.com/a/215575)
- [Binary Heap at GeeksforGeeks](https://www.geeksforgeeks.org/dsa/binary-heap/)
- [Binary Heaps at Tutorials Point](https://www.tutorialspoint.com/data_structures_algorithms/binary_heaps.htm)
- [Heap (data structure) at Wikipedia](https://en.wikipedia.org/w/index.php?title=Heap_(data_structure))
### Understanding nullability qualifiers
- [Nullability Checks at Clang docs](https://clang.llvm.org/docs/analyzer/developer-docs/nullability.html)
- [RFC: Nullability qualifiers at LLVM forum (Gregor Doug)](https://discourse.llvm.org/t/rfc-nullability-qualifiers/35672)
- [Adding _Nonnull and _Nullable attributes to mujoco.h APIs at Google Deepmind GitHub repository](https://github.com/google-deepmind/mujoco/issues/309)
- [_Nullable and _Nonnull in GCC's analyzer (was: [PATCH v5] libio: Add nonnull attribute for most FILE * arguments in stdio.h) at GCC mailing list (Martin Uecker)](https://gcc.gnu.org/pipermail/gcc/2023-August/242248.html)

## Got any suggestions?
Issues and PRs welcome. If you spot a race I missed, especially under `edf`
with many coders, open an issue on the repo or write to nora@defitero.com.

## Licence
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
