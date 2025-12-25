/*
 * This software is dual-licensed to the public domain and under the following
 * license: you are granted a perpetual, irrevocable license to copy, modify,
 * publish, and distribute this file as you see fit.
 */

/*
 * HEHE_DA - Dynamic Array Implementation
 * 
 * Usage:
 *   typedef struct {
 *       int *items;
 *       size_t count;
 *       size_t capacity;
 *   } IntArray;
 *   
 *   IntArray arr = {0};
 *   hehe_da_append(&arr, 42);
 *   hehe_da_free(&arr);
 */

/*
 * HEHE_QUE - Circular Queue Implementation
 * 
 * Usage:
 *   typedef struct {
 *       int *items;
 *       size_t head;
 *       size_t tail;
 *       size_t count;
 *       size_t capacity;
 *   } IntQueue;
 *   
 *   IntQueue que = {0};
 *   hehe_que_mem_init(&que, 64);  // Fixed capacity of 64
 *   
 *   int value = 42;
 *   hehe_que_enque(&que, value);  // Add to queue
 *   
 *   int result;
 *   hehe_que_deque(&que, result); // Remove from queue
 *   
 *   HEHE_DA_FREE(que.items);      // Free when done
 */

/*
 * Define before including to enable features:
 * HEHE_GIVE_ALL   - Include everything (defined by default if you dont define anything.)
 * HEHE_GIVE_DA    - Include dynamic array
 * HEHE_GIVE_BITS  - Include bit manipulation
 * HEHE_GIVE_QUEUE - Include queue
 */

//////////////
// TODO: 
// - Size_t overflow checks in DA.
// - Add string builder
// - Hashmap
//////////////

////////////////////////////////////
// NOTE:
// This library is made primarily for personal use, so there may be bugs,
// and some parts may be unfinished or unpolished. As I keep using it,
// i expect to discover what works well and what needs improvement.
// 
// Thank you for using my library! (feel free to contribute!)
////////////////////////////////////

#ifndef HEHe_H_
#define HEHe_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if !defined(HEHE_GIVE_DA) && !defined(HEHE_GIVE_QUEUE) && !defined(HEHE_GIVE_BITS)
#ifndef HEHE_GIVE_ALL
#define HEHE_GIVE_ALL
#endif /*HEHE_GIVE_ALL*/
#endif /*!defined(HEHE_GIVE_DA) && !defined(HEHE_GIVE_QUEUE) && !defined(HEHE_GIVE_BITS)*/

#ifndef HEHE_DA_INIT_CAP
#define HEHE_DA_INIT_CAP 256
#endif /* HEHE_DA_INIT_CAP */
#if HEHE_DA_INIT_CAP <= 0
#error "HEHE_DA_INIT_CAP must be positive"
#endif

// You can override these with your own implementations before including this header.
#ifndef HEHE_DA_ASSERT
#include <assert.h>
#define HEHE_DA_ASSERT assert
#endif /* HEHE_DA_ASSERT */

#ifndef HEHE_DA_FREE
#define HEHE_DA_FREE free
#endif /* HEHE_DA_FREE */

#ifndef HEHE_DA_REALLOC
#define HEHE_DA_REALLOC realloc
#endif /* HEHE_DA_REALLOC */

#ifndef HEHE_DA_MALLOC
#define HEHE_DA_MALLOC malloc
#endif /* HEHE_DA_MALLOC */

#define hehe_perma_assert(exp) do { if(!(exp)) { fprintf(stderr, "[HEHE_H] Assertion failed: %s, file %s, line %d\n", #exp, __FILE__, __LINE__); abort() ;} ;} while (0)

// [DYNAMIC ARRAY]
#if defined(HEHE_GIVE_ALL) || defined(HEHE_GIVE_DA)
#define hehe_da_grow(hda, needs)                                                \
     do {                                                                       \
         void* tmp = NULL;                                                      \
         if ((hda)->capacity == 0) {                                            \
             (hda)->capacity = HEHE_DA_INIT_CAP;                                \
         }                                                                      \
         while ((hda)->capacity < (hda)->count + needs) {                       \
             (hda)->capacity *= 2;                                              \
         }                                                                      \
         tmp = HEHE_DA_REALLOC((hda)->items, (hda)->capacity * sizeof(*(hda)->items));  \
         HEHE_DA_ASSERT(tmp && "HEHE GROW FAILED!");                            \
         (hda)->items = tmp;                                                    \
     } while (0)                                                                

#define hehe_da_append(hda, item)                                                      \
    do {                                                                               \
        if ((hda)->capacity < (hda)->count + 1) {                                      \
            hehe_da_grow((hda), 1);                                                    \
        }                                                                              \
        (hda)->items[(hda)->count++] = (item);                                         \
    } while (0)                                                                        

#define hehe_da_pop(hda, pop_out)                                   \
    do {                                                            \
        HEHE_DA_ASSERT(pop_out != NULL);                            \
        if ((hda)->count > 0) {                                     \
           *pop_out = ((hda)->items[--(hda)->count]);               \
        }                                                           \
    } while (0)                                                     

#define hehe_da_append_many(hda, item, amount)                                                 \
    do {                                                                                       \
        if ((hda)->capacity < (hda)->count + (amount)) {                                       \
            hehe_da_grow((hda), (amount));                                                     \
        }                                                                                      \
        memcpy(&((hda)->items[(hda)->count]), (item), sizeof(*(hda)->items) * (amount));       \
        (hda)->count += (amount);                                                              \
    } while (0)                                                                                

#define hehe_da_free(hda)                                                   \
    do {                                                                    \
        if ((hda)) {                                                        \
            HEHE_DA_FREE((hda)->items);                                             \
            (hda)->items = NULL;                                            \
            (hda)->count = 0;                                               \
            (hda)->capacity = 0;                                            \
        }                                                                   \
    } while (0)                                                             

#define hehe_da_clear(hda)  \
    do {                    \
        (hda)->count = 0;   \
    } while (0)             


#define hehe_da_reserve(hda, cap)              \
    do {                                       \
        if ((cap) > (hda)->capacity) {         \
            void* tmp = NULL;                  \
            tmp = HEHE_DA_REALLOC((hda)->items, (cap) * sizeof(*(hda)->items)); \
            HEHE_DA_ASSERT(tmp && "HEHE_DA_REALLOC FAILED!"); \
            (hda)->items = tmp;                \
            (hda)->capacity = (cap);           \
        }                                      \
    } while (0)                                
#endif /*defined(HEHE_GIVE_ALL) || defined(HEHE_GIVE_DA)*/

// [QUEUE / CIRCULAR BUFFER]
#if defined(HEHE_GIVE_ALL) || defined(HEHE_GIVE_QUEUE)
#define hehe_que_mem_init(que, cap)                                         \
    do {                                                                    \
        (que)->items = HEHE_DA_MALLOC(sizeof(*(que)->items) * (cap));       \
        hehe_perma_assert((que)->items != NULL);                            \
        (que)->head = 0;                                                    \
        (que)->tail = 0;                                                    \
        (que)->count = 0;                                                   \
        (que)->capacity = (cap);                                            \
    } while (0)

#define hehe_que_enque(que, item)                               \
    do {                                                        \
        if ((que)->count < (que)->capacity) {                   \
           (que)->items[(que)->tail] = (item);                  \
           (que)->tail = ((que)->tail + 1) % (que)->capacity;   \
           (que)->count++;                                      \
        }                                                       \
    } while (0)                     

#define hehe_que_deque(que, item)                                   \
    do {                                                            \
        if ((que)->count > 0) {                                     \
            (item) = (que)->items[(que)->head];                     \
            (que)->head = ((que)->head + 1) % (que)->capacity;      \
            (que)->count--;                                         \
        }                                                           \
    } while (0)

#define hehe_que_free(que)                      \
    do {                                        \
        if ((que)) {                            \
            HEHE_DA_FREE((que)->items);         \
            (que)->items = NULL;                \
            (que)->head = 0;                    \
            (que)->tail = 0;                    \
            (que)->count = 0;                   \
            (que)->capacity = 0;                \
        }                                       \
    } while (0)

#define hehe_que_is_full(que)  ((que)->count == (que)->capacity)
#define hehe_que_is_empty(que) ((que)->count == 0)
#define hehe_que_size(que)     ((que)->count)
#endif/*defined(HEHE_GIVE_ALL) || defined(HEHE_GIVE_QUEUE)*/

// [HEHE_BITS]
// BIG SHOUTOUT [https://www.andreinc.net/2023/02/01/demystifying-bitwise-ops]
#if defined(HEHE_GIVE_ALL) || defined(HEHE_GIVE_BITS)

#define hehe_bits_set_nth(num, nbit) do {(num) |= (1 << (nbit));} while (0)
#define hehe_bits_toggle_nth(num, nbit) do {(num) ^= (1 << (nbit));} while (0)
#define hehe_bits_clear_nth(num, nbit) do {(num) &= ~(1 << (nbit));} while (0)
#define hehe_bits_check_nth(num, nbit) (((num) >> (nbit)) & 1)

#define hehe_bits_print_uint8_t(num, out)            \
    do {                                             \
        for (int i = 7; i >= 0; i--) {               \
            fprintf((out),"%d", (((num) >> i) & 1)); \
        }                                            \
    } while (0) 

#define hehe_bits_print_uint16_t(num, out)           \
    do {                                             \
        for (int i = 15; i >= 0; i--) {              \
            fprintf((out),"%d", (((num) >> i) & 1)); \
        }                                            \
    } while (0) 

#define hehe_bits_print_uint32_t(num, out)           \
    do {                                             \
        for (int i = 31; i >= 0; i--) {              \
            fprintf((out),"%d", (((num) >> i) & 1)); \
        }                                            \
    } while (0) 

#define hehe_bits_print_uint64_t(num, out)           \
    do {                                             \
        for (int i = 63; i >= 0; i--) {              \
            fprintf((out),"%d", (((num) >> i) & 1)); \
        }                                            \
    } while (0) 
#endif/*defined(HEHE_GIVE_ALL) || defined(HEHE_GIVE_BITS)*/

#endif /* HEHe_H_ */
/****************************************/
/* Written by ethersai - Public domain. */
/****************************************/
