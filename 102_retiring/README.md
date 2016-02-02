# kbox: retiring

Enables the ability to retire code which at run time is determined
to never be usable after a specific point in time. Code retiring is
needed for different reasons. Each reason has its own set of priorities
for retiring code at run time.

  * memory, low priority: help free up memory by freeing unused kernel code
    segments. For more details refer to 101_intro/README.md

  * single kernel, medium priority: due to architectural compromises to enable
    a single kernel to run under entirely different run time environments with
    different features which can only be determined if needed at run time. For
    more details on this as an example refer to the virtualization situation:

http://www.do-not-panic.com/2015/12/avoiding-dead-code-pvops-not-silver-bullet.html
http://www.do-not-panic.com/2015/12/xen-and-x86-linux-zero-page.html

  * security and kernel ABI, high priority: kernel symbols could be used in
    ways not intended to be used, or exposed in situations where one knows they
    should not be available.

## Ways to retire code

There are different ways to retire code.

1. set_memory_nx() - marks memory as non-executable.
2. set_memory_rw() - makes code with read-write access, required prior to
   making memory range useful for the kernel before freeing it so that the
   kernel can repurpose it.
3. free_reserved_area() - used for freeing a memory range of kernel code.
   This is typically prefixed with set_memory_nx() and set_memory_rw() on
   on the same range prior to issuing free_reserved_area().
4. set_memory_np() - marks memory not present, useful for debugging
   and testing before using free_reserved_area() on a region on production
   environments. This forces the kernel to trigger a page fault on
   access to the memory range given.

## What code to retire ?

Perhaps the biggest challenge to retiring code is knowing the address of all
the code you want to retire. There are two current possible strategies on
the kernel:

1. As with the __init strategy declare a section for your own code and
   free the range once you determine you don't need it. There is currently
   no generic way to define these custom section areas, however work is
   underway to do just that with Linux linker tables [0].

   [0] http://lkml.kernel.org/r/1450217797-19295-1-git-send-email-mcgrof@do-not-panic.com

2. Determine symbol starting address with page_address() for starting address
   and either compute the ending address knowing the number of pages you've
   used or use ksize() if your symbol was kmalloc()'d. ksize() is only
   guaranteed to work for pointers returned by kmalloc().

3. XXX: anything else?

The challenges with identifying what code to retire and its size leads us
to the next section on iovisor.

# Build

This currently doesn't do anything yet. We'll eventually demo each way to
retire code.

Run:

```
make
```
