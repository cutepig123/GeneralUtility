#ifndef SG_ATOMIC_HPP_INCLUDED
#define SG_ATOMIC_HPP_INCLUDED

#if defined(_MSC_VER)
#define NOMINMAX
#include <windows.h>
//#define InterlockedAdd _InterlockedAdd
//#include <intrin.h>
//
//#pragma intrinsic(_InterlockedAdd)
#if  _MSC_VER < 1800	// <Visual Studio 2013
inline LONG __cdecl InterlockedAdd(
  _Inout_  LONG volatile *Addend,
  _In_     LONG Value
){
LONG x = InterlockedExchangeAdd(Addend,Value);
return x+Value;
}
#endif

#endif
#if defined(__SUNPRO_CC)
#include <atomic.h>
#include <sched.h>
#endif
#if defined(__GNUC__)
#include <pthread.h>
#endif

namespace sg {

namespace detail {

#if defined(__SUNPRO_CC)

template<int N> struct AtomicImplAux {
    template<typename T> static void increase(T *ptr) { atomic_inc_32(ptr); }
    template<typename T> static void decrease(T *ptr) { atomic_dec_32(ptr); }
    template<typename T> static T increase_nv(T *ptr) { return atomic_inc_32_nv(ptr); }
    template<typename T> static T decrease_nv(T *ptr) { return atomic_dec_32_nv(ptr); }
    template<typename T> static T add_nv(T *ptr, T val) { return atomic_add_32_nv(ptr, val); }
    template<typename T> static T cas(volatile T *ptr, T oldval, T newval) { return atomic_cas_32(ptr, oldval, newval); }
};

#if defined(_INT64_TYPE)
template<> struct AtomicImplAux<8> {
    template<typename T> static void increase(T *ptr) { atomic_inc_64(ptr); }
    template<typename T> static void decrease(T *ptr) { atomic_dec_64(ptr); }
    template<typename T> static T increase_nv(T *ptr) { return atomic_inc_64_nv(ptr); }
    template<typename T> static T decrease_nv(T *ptr) { return atomic_dec_64_nv(ptr); }
    template<typename T> static T add_nv(T *ptr, T val) { return atomic_add_64_nv(ptr, val); }
    template<typename T> static T cas(volatile T *ptr, T oldval, T newval) { return atomic_cas_64(ptr, oldval, newval); }
};
#endif // defined(_INT64_TYPE)
#endif // defined(__SUNPRO_CC)

#if defined(__INTEL_COMPILER) || defined(__GNUC__)
template<int N> struct AtomicImplAux {
    template<typename T> static void increase(T *ptr) { __sync_add_and_fetch(ptr, 1); }
    template<typename T> static void decrease(T *ptr) { __sync_sub_and_fetch(ptr, 1); }
    template<typename T> static T increase_nv(T *ptr) { return __sync_add_and_fetch(ptr, 1); }
    template<typename T> static T decrease_nv(T *ptr) { return __sync_sub_and_fetch(ptr, 1); }
    template<typename T> static T add_nv(T *ptr, T n) { return __sync_add_and_fetch(ptr, n); }
    template<typename T> static T cas(volatile T *ptr, T oldval, T newval) { return __sync_val_compare_and_swap(ptr, oldval, newval); }
};

#endif // defined(__INTEL_COMPILER) || defined(__GNUC__)

#if defined(_MSC_VER)
template<int N> struct AtomicImplAux {
    template<typename T> static void increase(T *ptr) { InterlockedIncrement(ptr); }
	template<> static void increase(int *ptr) { InterlockedIncrement((volatile unsigned int *)ptr); }
    template<typename T> static void decrease(T *ptr) { InterlockedDecrement(ptr); }
    template<typename T> static T increase_nv(T *ptr) { return InterlockedIncrement(ptr); }
    template<typename T> static T decrease_nv(T *ptr) { return InterlockedDecrement(ptr); }
	template<typename T> static T add_nv(T *ptr, T val) { return atomic_add_32_nv(ptr, val); }

	static int atomic_add_32_nv(int *prt, int val)
	{
		return InterlockedAdd((volatile unsigned int *)prt, val);
	}
	static unsigned int atomic_add_32_nv(volatile unsigned int *prt, int val)
	{
		return InterlockedAdd((volatile LONG *)prt, val);
	}
	static PVOID cas(PVOID *ptr, PVOID oldval, PVOID newval) { InterlockedCompareExchangePointer(ptr, newval, oldval); }
	//template<typename T> static T cas(volatile T *ptr, T oldval, T newval) { InterlockedCompareExchange(ptr,newval, oldval); }
};
#endif // _MSC_VER


#if defined(__SUNPRO_CC)
struct AtomicImpl {
    static void memory_fence_enter() {
        // Any store preceding membar_enter() will reach global visibility
        // before all loads and stores following it.

        // membar_enter() is typically used in code that implements locking
        // primitives to ensure that a lock protects its data.

        // write | read/write
        membar_enter();
    }
    static void memory_fence_exit() {
        // All loads and stores preceding membar_exit() will reach global visi-
        // bility before any store that follows it.

        // membar_exit() is typically used in code that implements locking
        // primitives to ensure that a lock protects its data.
        // read/write | write
        membar_exit();
    }
    static void memory_fence_producer() {
        // All stores preceding the memory barrier will reach global visibility
        // before any stores after the memory barrier reach global visibility.
        // write | write
        membar_producer();
    }
    static void memory_fence_consumer() {
        // All loads preceding the memory barrier will complete before any
        // loads after the memory barrier complete.
        // read | read
        membar_consumer();
    }

    static void membar_sync() {
        // All loads and stores preceding the memory barrier will complete and
        // reach global visibility before any loads and stores after the memory
        // barrier complete and reach global visibility.
        // read/write | read/write
        membar_sync();
    }

    static bool lock_test_and_set(volatile unsigned int *ptr) {
        if (atomic_swap_32(ptr, 1) == 0) {
            memory_fence_enter();
            return true;
        }
        return false;
    }
    static void lock_release(volatile unsigned int *ptr) {
        memory_fence_exit();
        *ptr = 0;
    }

    static void yield() { sched_yield(); }
    static void rep_nop() { asm ("rep;nop":::"memory"); }
    static void compiler_fence() { asm ("":::"memory"); }
};
#endif // __SUNPRO_CC

#if defined(__INTEL_COMPILER) || defined(__GNUC__)
struct AtomicImpl {
    static bool lock_test_and_set(volatile unsigned int *ptr) { return __sync_lock_test_and_set(ptr, 1) == 0; }
    static void lock_release(volatile unsigned int *ptr) { __sync_lock_release(ptr); }

    static void yield() { sched_yield(); }
    static void compiler_fence() { __asm __volatile ("":::"memory"); }

#if defined(__SSE2__)

#if defined(__INTEL_COMPILER)
    static void memory_fence_enter()    { _mm_mfence(); }
    static void memory_fence_exit()     { _mm_mfence(); }
    static void memory_fence_producer() { _mm_mfence(); }
    static void memory_fence_consumer() { _mm_mfence(); }
#else // defined(__INTEL_COMPILER)
    static void memory_fence_enter()    { __builtin_ia32_mfence(); }
    static void memory_fence_exit()     { __builtin_ia32_mfence(); }
    static void memory_fence_producer() { __builtin_ia32_mfence(); }
    static void memory_fence_consumer() { __builtin_ia32_mfence(); }
#endif // defined(__INTEL_COMPILER)

#else // defined(__SSE2__)
    static void memory_fence_enter()    { __asm __volatile ("":::"memory"); }
    static void memory_fence_exit()     { __asm __volatile ("":::"memory"); }
    static void memory_fence_producer() { __asm __volatile ("":::"memory"); }
    static void memory_fence_consumer() { __asm __volatile ("":::"memory"); }
#endif // defined(__SSE2__)


#if __ARM_ARCH_7__ || __ARM_ARCH_7A__ || __ARM_ARCH_7R__ || __ARM_ARCH_7M__
    static void rep_nop() { __asm __volatile ("":::"memory"); }
#else
    static void rep_nop() { __asm __volatile ("rep;nop":::"memory"); }
#endif
};
#endif // defined(__INTEL_COMPILER) or defined(__GNUC__)


#if defined(_MSC_VER)
struct AtomicImpl {
    static void memory_fence_enter() { MemoryBarrier(); }
    static void memory_fence_exit() { MemoryBarrier(); }
    static void memory_fence_producer() { MemoryBarrier(); }
    static void memory_fence_consumer() { MemoryBarrier(); }

	static LONG atomic_swap_32(LONG volatile *Target, LONG Value)
	{
		return InterlockedExchange(Target,Value);
	}

	static bool lock_test_and_set(volatile unsigned int *ptr) {
        if (atomic_swap_32((LONG*)ptr, 1) == 0) {
            memory_fence_enter();
            return true;
        }
        return false;
    }
    static void lock_release(volatile unsigned int *ptr) {
        memory_fence_exit();
        *ptr = 0;
    }

	static void sched_yield()
	{
		SwitchToThread ();
	}
	static void yield() { sched_yield(); }
    static void rep_nop() {  }
    static void compiler_fence() {  }
};
#endif // _MSC_VER

} // namespace detail

struct Atomic {
    template<typename T> static void increase(T *ptr) { detail::AtomicImplAux<sizeof(T)>::increase(ptr); }
    template<typename T> static void decrease(T *ptr) { detail::AtomicImplAux<sizeof(T)>::decrease(ptr); }
    template<typename T> static T increase_nv(T *ptr) { return detail::AtomicImplAux<sizeof(T)>::increase_nv(ptr); }
    template<typename T> static T decrease_nv(T *ptr) { return detail::AtomicImplAux<sizeof(T)>::decrease_nv(ptr); }
    template<typename T> static T add_nv(T *ptr, T n) { return detail::AtomicImplAux<sizeof(T)>::add_nv(ptr, n); }
    template<typename T> static T cas(T *ptr, T oldval, T newval) { return detail::AtomicImplAux<sizeof(T)>::cas(ptr, oldval, newval); }

    template<typename T> static T swap(volatile T *ptr, T newval) {
        for (;;) {
            T prev = *ptr;
            T oldval = detail::AtomicImplAux<sizeof(T)>::cas(ptr, prev, newval);
            if (oldval == prev)
                return oldval;
        }
    }

    // test to grab a lock (true = success, you now own the lock), and perform the required memory barriers
    static bool lock_test_and_set(volatile unsigned int *ptr) { return detail::AtomicImpl::lock_test_and_set(ptr); }

    // release lock grabbed by lock_test_and_set, and perform the required memory barriers
    static void lock_release(volatile unsigned int *ptr) { return detail::AtomicImpl::lock_release(ptr); }

    static void memory_fence_enter() { detail::AtomicImpl::memory_fence_enter(); }
    static void memory_fence_exit() { detail::AtomicImpl::memory_fence_exit(); }
    static void memory_fence_producer() { detail::AtomicImpl::memory_fence_producer(); }
    static void memory_fence_consumer() { detail::AtomicImpl::memory_fence_consumer(); }
    static void yield() { detail::AtomicImpl::yield(); }

    // rep_nop issues the "pause" instruction. also clobbers memory.
    static void rep_nop() { detail::AtomicImpl::rep_nop(); }

    static void compiler_fence() { detail::AtomicImpl::compiler_fence(); }
};

} // namespace sg

#endif // SG_ATOMIC_HPP_INCLUDED
