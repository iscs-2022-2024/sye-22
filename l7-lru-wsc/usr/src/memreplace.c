#include <ctype.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memreplace_gui/memreplace_gui.h>

#define RESET        	2
#define DELTA        	2
#define EXIT_MEMRPLC 	(-2)

#define PAGE_NB_MASK    0x00F0
#define REF_BIT_MASK    0x0004
#define SWAP_BIT_MASK   0x0002
#define VALID_BIT_MASK  0x0001

/* bitmask manipulations macros */
#define CHECK_MASK(x, mask) (((x) & (mask)) == (mask))
#define SET_MASK(x, mask) ((x) |= (mask))
#define UNSET_MASK(x, mask) ((x) &= ~(mask))

/* utility macro to unload a page */
#define UNLOAD_PAGE(page)                     \
    do {                                      \
        UNSET_MASK((page), (VALID_BIT_MASK)); \
        SET_MASK((page), (SWAP_BIT_MASK));    \
    } while (0)

#define NB_PAGES    	16

/* [7..4] = Page number on 4 bits */
/* [3] = Unused 1 bit */
/* [2] = Reference on 1 bit */
/* [1] = SWAP on 1 bit */
/* [0] = Valid on 1 bit */
uint8_t page_table[NB_PAGES] = {0};

/* TDU array */
uint8_t tdu[NB_PAGES] = {0};
/* TVC counter*/
uint8_t tvc = 0;

void replaceLRU(void) {
    /* least used page search */
    int page_to_swap = -1;
    uint8_t min = tvc;
    for (size_t i = 0; i < NB_PAGES; ++i) {
        if ((CHECK_MASK(page_table[i], VALID_BIT_MASK))) {
            if (page_to_swap == -1 || tdu[i] < min) {
                page_to_swap = i;
                if (tdu[i] < min) {
                    min = tdu[i];
                }
            }
        }
    }

    if (page_to_swap != -1) {
        UNLOAD_PAGE(page_table[page_to_swap]);
    } else {
        fprintf(stderr, "An error occurred: no valid page to swap could be found\n");
        return;
    }
    
}

void inc_compteur(uint8_t no_page) {
    tdu[no_page] = tvc++;
}

void replaceWSC(void) {
    int page_to_swap = -1;

    for (uint8_t i = 0; i < NB_PAGES; ++i) {
        if (CHECK_MASK(page_table[i], VALID_BIT_MASK)) {
            /* save the first page in RAM */
            if (page_to_swap == -1) {
                page_to_swap = i;
            }
            if (CHECK_MASK(page_table[i], REF_BIT_MASK)) {
                UNSET_MASK(page_table[i], REF_BIT_MASK);
            } else if (tvc - tdu[i] > DELTA) {
                page_to_swap = i;
                break;
            }
        }
    }

    if (page_to_swap != -1) {
        UNLOAD_PAGE(page_table[page_to_swap]);
    } else {
        fprintf(stderr, "An error occurred: no valid page to swap could be found\n");
        return;
    }
}

void updateTDU(void) {
    for (size_t i = 0; i < NB_PAGES; ++i) {
        if (CHECK_MASK(page_table[i], REF_BIT_MASK)) {
            tdu[i] = tvc;
        }
    }
}

void init_page_table(void) {
    int i;

    for (i = 0; i < NB_PAGES; i++)
        page_table[i] |= (i & 0xF) << 4;

    page_table[0] |= VALID_BIT_MASK;
	page_table[1] |= VALID_BIT_MASK;
	page_table[2] |= VALID_BIT_MASK;
	page_table[3] |= SWAP_BIT_MASK;
}

void print_memory(void) {
    int i;

    printf("RAM : ");

    for (i = 0; i < NB_PAGES; i++)
        if (page_table[i] & VALID_BIT_MASK)
            printf("[%d] ", i);

    printf("\nSWAP : ");
    for (i = 0; i < NB_PAGES; i++)
        if (!(page_table[i] & VALID_BIT_MASK) && (page_table[i] & SWAP_BIT_MASK))
            printf("[%d] ", i);

    printf("\n");
}

int ask_user_page_no() {
    char input[4];

    printf("Enter the page to be access: ");
    fflush(stdout);
    fgets(input, sizeof(input), stdin);

    if (input[0] == 'E' || input[0] == 'e')
        return EXIT_MEMRPLC;

    if (!isdigit(input[0]))
        return -1;

    return atoi(input);
}

void sigint_handler(int sig) {
    memreplace_exit();
    exit(0);
}

int main(int argc, char *argv[]) {
    int page_no;
    bool is_valid = true;
    bool gui_run = true; 
    int LRU_run  = 0;

    if (argc < 2 || argc > 3) {
        is_valid = 0;
    }
    else if (strcmp("LRU", argv[1]) == 0) {
        LRU_run = 1;
    }
    else if (strcmp("WSC", argv[1]) == 0) {
        LRU_run = 0;
    }
    else {
        is_valid = 0;
    }

    if (argc == 3) {
        if (strcmp("--ng", argv[2]) == 0) {
            gui_run = false;
        }
    }

    if (!is_valid) {
        printf("Usage: memereplace LRU|WSC [--ng]\n");
        exit(1);
    }

    signal(SIGINT, sigint_handler);

    if (gui_run)
        memreplace_init();
		
    init_page_table();
    print_memory();

    if (gui_run)
        memreplace_gui_update_pages(page_table, NB_PAGES);

    for (;;) {
        page_no = ask_user_page_no();

        if (page_no == EXIT_MEMRPLC) {
            memreplace_exit();
            return 0;
        }
        else {
            if (page_no >= 0 && page_no < NB_PAGES) {
                if (LRU_run) {
                    /* LRU Related */
                    if (!(page_table[page_no] & VALID_BIT_MASK)) { /* VALID = 0 ? */
                        replaceLRU();
                    }
                    inc_compteur(page_no);
                }
                else {
                    /* WSC Related */
                    if (!(page_table[page_no] & VALID_BIT_MASK)) { /* VALID = 0 ? */
                        replaceWSC();
                    }
                    updateTDU();   
                    tvc++;
                }

                // set ref bit and valid bit
                SET_MASK(page_table[page_no], VALID_BIT_MASK | REF_BIT_MASK);
                // unset swap bit
                UNSET_MASK(page_table[page_no], SWAP_BIT_MASK);

                print_memory();

                if (gui_run)
                    memreplace_gui_update_pages(page_table, NB_PAGES);
            }
            else
                printf("Invalid page number\n");
        }
    }

    return 0;
}
