#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yakir Dorani");
MODULE_DESCRIPTION("Argus interview");
MODULE_VERSION("0.01");

//#define IDENTITY_NAME_LEN 15

//typedef int bool;
//enum { false, true };

typedef struct identity {
//    char name[IDENTITY_NAME_LEN];
    int id;
    bool busy;
} identity;

typedef struct node {
    identity obj;
    struct node *previous;
    struct node *next;
} node_t;


static node_t *head = NULL;

//static void identity_create(char str[20] name, int id) {
//	push(
//}

static void identity_create(int id) {
    node_t *cur = head;
    node_t *prev = NULL;

    if (head == NULL) {
	printk(KERN_INFO "Creating first identity: %d", id);
	printk(KERN_INFO "Allocating memory...");
        head = vmalloc(sizeof(node_t));
	printk(KERN_INFO "Memory allocated!");
        head->obj.id = id;
	head->obj.busy = false;
	head->previous = NULL;
        head->next = NULL;
        return;
    }

    while (cur->next != NULL) {
	prev = cur;
	cur = cur->next;
    }

	printk(KERN_INFO "Creating identity: %d", id);

    /* now we can add a new variable */
	printk(KERN_INFO "Allocating memory...");
	cur->next = vmalloc(sizeof(node_t));
	printk(KERN_INFO "Memory allocated!");
	cur->previous = prev;
	cur->next->obj.id = id;
	cur->next->obj.busy = false;
	cur->next->next = NULL;
	cur->next->previous = cur;
}

static node_t * identity_find(int id) {
    node_t *cur = head;
    
    printk(KERN_INFO "Finding identity: %d", id);

    while (cur != NULL) {
	node_t *next = cur->next;
	if (cur->obj.id == id) {
		printk(KERN_INFO "Found identity! %d", id);
		return cur;
	}

        cur = next;
    }
}

static void identity_destroy(int id) {
	node_t *n = identity_find(id);

	printk(KERN_INFO "Destroying identity: %d", id);

	if (n != NULL) {
		if (n->previous == NULL) {
			head = n->next;
		} else {
			n->previous->next = n->next;
			n->next->previous = n->previous;
		}

		vfree(n);
		printk(KERN_INFO "Memory freed");
	}
}

static int __init lkm_example_init(void) {

	identity_create(1);
	identity_create(2);

	identity_destroy(1);
	identity_destroy(2);

	printk(KERN_INFO "Fun init!\n");

        return 0;
}

static void __exit lkm_example_exit(void) {
 printk(KERN_INFO "Fun, exit!\n");
}

module_init(lkm_example_init);
module_exit(lkm_example_exit);
