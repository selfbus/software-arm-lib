Type Usage
===

The following types are used in the code base:

* **Memory address (Flash, RAM)**: `uint8_t *` (in few cases also `void *` or `uintptr_t`)

  Memory is an array of bytes. Therefore, `uint8_t *` is the best type for generic memory addresses.
  
  Sometimes, developers recommend to use `void *` for this. That is problematic as any pointer is implicitly convertible
  to `void *`, so the code actually becomes less clear and also less type safe. However, this type does make sense
  to use for functions that need to accept generic memory addresses (pointers) for whatever reason, e.g. debug logging
  or the canonical `memcpy` and `memcmp` functions.

  And there's also `uintptr_t`. This is not a pointer type per se, but a pointer-sized unsigned integer, i.e. on a
  32-bit target platform it is 32 bits wide and on a 64-bit target platform 64 bits wide. This is useful for data
  structures that need to represent generic data where the concrete data depends on some other value. For example, the
  data structure for IAP commands has a `cmd` field and the command's parameters can have any type as their
  interpretation depends on the value of `cmd`. So in cases like this, a `uintptr_t` can be used as it can store any
  integer, and is big enough for pointers as well.
