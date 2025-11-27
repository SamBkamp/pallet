# pallet

My attempt at containerization (get it - container but smaller is a pallet? Bazinga).

I want to write this using syscalls where I can. I won't **only** use syscalls as things like printf are very useful, but especially where it pertains to the actual containerization I want to rely entirely on syscalls if I can.

## get started

To start, you need to set up a file system in the fs/ subdirectory so that the pallet container can actually run things.

Then, you can run the container with
```bash
$ ./pallet <program name>
```
**make sure you have elevated priviledges so you can chroot (among other things)**


## compilation
just run the makefile with `make`. Et voila.