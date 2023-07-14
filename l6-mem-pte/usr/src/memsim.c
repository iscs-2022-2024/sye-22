/*
 * Copyright (C) 2021 Mattia Gallacchi <mattia.gallacchi@heig-vd.ch>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEBUG 0

/* Switch between memory sizes. 1 for 256B,  0 for 1024B */
#define MEMORY_SIZE_SMALL               0

/* Activate memory attributes. This is used in step 3 */
#define MEMORY_PTE_ATTRIBUTES_ACTIVE    0

/** 
 *  Defines for step 2 :
 *      MEMORY_SIZE_SMALL 1
 *      MEMORY_PTE_ATTRIBUTES_ACTIVE 0
 * 
 * Defines for step 3 :
 *      MEMORY_SIZE_SMALL 1
 *      MEMORY_PTE_ATTRIBUTES_ACTIVE 1
 * 
 * Defines for step 4 :
 *      MEMORY_SIZE_SMALL 0
 *      MEMORY_PTE_ATTRIBUTES_ACTIVE 0
 */

#if MEMORY_SIZE_SMALL
/* Number of memory pages */
#define MEMORY_PAGE_NUM         256
#define MEMORY_PAGE_SIZE        256
#else
#define MEMORY_PAGE_NUM         64
#define MEMORY_PAGE_SIZE        1024
#endif

/* Defines for step 3 */
#define VALID_BIT       (1 << 0)
#define VALID_POS       0

#define RWX_BITS        0x06
#define RWX_POS         1
#define R_PATTERN       0b01
#define W_PATTERN       0b10
#define X_PATTERN       0b11
#define RWX_PATTERN     0b00

/* Custom defines */
#define HIGH8(x) (x) >> (8) & (0xFF)
#define LOW8(x) (x) & (0xFF)
#define HIGH6(x) (x) >> (3) & (0x7)
#define LOW10(x) (x) & (0x3FF)


/** 
 *  Step 3 address
 *  PADDR 8 MSB = page no.
 *  PADDR 8 LSB = [0] VALID, [2..1] RWX: 00 = ALL, 01 = R, 10: W, 11 = X, [7..3] offset 
 */

#define PAGE_FAULT(addr, msg) printf("Page Fault @0x%x: %s\n", addr, msg);


uint16_t page_table[MEMORY_PAGE_NUM];

/* 256 pages of 256B or 64 pages of 4096B */
uint8_t main_mem[MEMORY_PAGE_NUM][MEMORY_PAGE_SIZE] = {0};


/**
 * @brief   Convert a virtual address to a page table entry. 
 * @param   vaddr virtual address
 * @param   pte page table entry   
 * 
 * @return  0 on success, -1 on error.
 */
int virt_to_pte(uint16_t vaddr, uint16_t *pte)
{
    /*A COMPLETER*/

    if (pte == NULL) {
        return -1;
    }

#if MEMORY_SIZE_SMALL // A completer étape 4
    *pte = page_table[HIGH8(vaddr)];
#else
    *pte = page_table[HIGH6(vaddr)];
#endif

    return 0;
}

/**
 * @brief   Read a byte from memory. For step 3 also checks RWX and Valid bit.
 *          Uses virt to phys to get the physical address.
 * @param   vaddr virtual address
 * @param   byte value to read   
 * 
 * @return  0 on success, -1 on error. Page faults must use PAGE_FAULT macro
 *          to print an error message
 */
int get_byte(uint32_t vaddr, uint8_t *byte) {    

    if (byte == NULL) {
        return -1;
    }

    uint16_t pte;
    virt_to_pte((uint16_t)vaddr, &pte);

#if MEMORY_PTE_ATTRIBUTES_ACTIVE //A completer étape 3
    uint16_t rwx;
    rwx = ((pte & (RWX_BITS)) >> RWX_POS);
    if (rwx != R_PATTERN && rwx != RWX_PATTERN) {
        fprintf(stderr, "Unable to read page\n");
        return -1;
    }
    if (((pte >> VALID_POS) & 1) != VALID_BIT) {
        PAGE_FAULT(vaddr, "Page is not mapped\n");
        return -1;
    }
#endif

#if MEMORY_SIZE_SMALL // A completer étape 4
    *byte = main_mem[HIGH8(pte)][LOW8(vaddr)];
#else
    *byte = main_mem[HIGH6(pte)][LOW10(vaddr)];
#endif
    return 0;
}

/**
 * @brief   Write a byte to memory. For step 3 also checks RWX and Valid bit.
 *          Uses virt to phys to get the physical address.
 * @param   vaddr virtual address
 * @param   byte value to write   
 * 
 * @return  0 on success, -1 on error. Page faults must use PAGE_FAULT macro
 *          to print an error message
 */
int store_byte(uint32_t vaddr, uint8_t byte) {
    /*A COMPLETER*/
    uint16_t pte; 
    virt_to_pte((uint16_t)vaddr, &pte);

#if MEMORY_PTE_ATTRIBUTES_ACTIVE //A completer étape 3
    uint16_t rwx;
    rwx = ((pte & (RWX_BITS)) >> RWX_POS);
    if (rwx != W_PATTERN && rwx != RWX_PATTERN) {
        fprintf(stderr, "Unable to write to page\n");
        return -1;
    }
    if (pte & ((1 << VALID_POS) >> VALID_POS) != VALID_BIT) {
        PAGE_FAULT(vaddr, "Page is not mapped\n");
        return -1;
    }
#endif

#if MEMORY_SIZE_SMALL // A completer étape 4
    main_mem[HIGH8(pte)][LOW8(vaddr)] = byte;
#else
    main_mem[HIGH6(pte)][LOW10(vaddr)] = byte;
#endif
    return 0;

}

/**
 * @brief   Prints an entire page. Can be used for debug.
 * @param   vaddr: virtual address of the page
 * @param   width: number of bytes per line (1/2/3/../32)
 */
void print_page(uint16_t vaddr, uint8_t width)
{
#if DEBUG
    uint16_t i;
    uint8_t byte;

    if (width > 32) {
        width = 32;
    }

    for (i = 0; i < MEMORY_PAGE_SIZE; ++i) {
        if (i % width == 0 && i > 0) 
            printf("\n");

        if (get_byte(vaddr | i, &byte) < 0) {
            break;
        }
        printf("%02x ", byte);
    }
    printf("\n");
#else
    printf("To print activate debug\n");
#endif
}

/**
 * @brief   This function is used to test the read and write methods.
 *          !!! DO NOT MODIFY !!!
 */
int test_mem()
{
    uint8_t byte;
    int vaddr;
    int error_count = 0;

    for (vaddr = 0; vaddr < MEMORY_PAGE_NUM * MEMORY_PAGE_SIZE; ++vaddr) {
        if (get_byte(vaddr, &byte) < 0) {
            printf("Error vaddr : 0x%04x\n");
            return -1;
        }

        if (byte != (uint8_t)(vaddr & 0xFF)) {
            error_count++;
        }
    }

    if (error_count > 0) {
        printf("Memory test failed. There are %d errors\n", error_count);
        return -1;
    }
    else {
        printf("=== Memory test successfull !!!===\n");
        return 0;
    }
}

/**
 * @brief   This function is used to test the RWX and Valid bit.
 *          !!! DO NOT MODIFY !!!
 */
int test_mem_2()
{
    int i, j;
    uint16_t vaddr = 0;
    uint8_t byte = 0;

    for (i =0; i < MEMORY_PAGE_NUM; i++) {
        for (j = 0; j < MEMORY_PAGE_SIZE; j++) {
            vaddr = (i << 8) | ((j << 3) & 0xFF);

            if (i == 10 || i == 11 || i == 12) {
                /* Test read only pages*/
                if (get_byte(vaddr, &byte) < 0) {
                    printf("Readonly test failed @0x%04x\n", vaddr);
                    return -1;
                }
                if (store_byte(vaddr, 0x01) != -1) {
                    printf("Readonly test failed @0x%04x\n", vaddr);
                    return -1;
                }
                // printf("i = %d, j = %d\n", i, j);
            }
            else if (i == 5 || i== 6 || i == 7 || i == 8) {
                /* Valid bit test */
                if (get_byte(vaddr, &byte) != -1) {
                    printf("Valid bit test failed @0x%04x\n", vaddr);
                    return -1;
                }
                // printf("i = %d, j = %d\n", i, j);
            }
            else {
                if (get_byte(vaddr, &byte) < 0) {
                    printf("Readonly test failed @0x%04x\n", vaddr);
                    return -1;
                }
                if (store_byte(vaddr, 0x01) < 0) {
                    printf("Readonly test failed @0x%04x\n", vaddr);
                    return -1;
                }
            }
        }   
    }

    return 0;
}

/**
 * @brief   Initializes the page table. The virtual addresses are
 *          the inverse of the physical ones. Ex. physical 0x0000
 *          is equal to virtual 0xFFFF
 */
void init_page_table(void) {
    int i;
    uint8_t r_shift = 0;
#if MEMORY_SIZE_SMALL
    r_shift = 8;
#else
    r_shift = 10;
#endif
    for (i = 0; i < MEMORY_PAGE_NUM; ++i) {
        page_table[i] = ((MEMORY_PAGE_NUM -1) - i) << r_shift;
#if MEMORY_PTE_ATTRIBUTES_ACTIVE
        /* TODO step 3 */
        /* Make PTE 10 to 12 read-only */
        if (i >= 10 && i <= 12) {
            page_table[i] |= R_PATTERN << RWX_POS;
        }
        /* Valid bit*/
        page_table[i] |= VALID_BIT;
        /* Make PTE 5 to 8 not valid */
        if (i >= 5 && i <= 8) {
            page_table[i] |= ~VALID_BIT;
        }
#endif
    }
}
int main(int argc, char *argv[]) {    
    /*
        !!! DO NOT MODIFY the tests of the main function!!! 
    */

    init_page_table();

#if MEMORY_PTE_ATTRIBUTES_ACTIVE
    if (test_mem_2() == 0) {
        printf("===Readonly and Valid bit tests passed successfully!===\n");
    }

#else
    int vaddr;

    /** Puts data into memory */
    for (vaddr = 0; vaddr < MEMORY_PAGE_NUM * MEMORY_PAGE_SIZE; ++vaddr) {
        store_byte(vaddr, (uint8_t)(vaddr & 0xFF));
    }
    test_mem();
#endif

    /* ADD your code here for debug purposes */

    // print_page(0x0000, 32);

    return 0;
}
