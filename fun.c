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

static int identity_create(int id) {
	node_t *cur = head;
	node_t *prev = NULL;

	if (head == NULL) {
		// Add first identity
		printk(KERN_INFO "Creating first identity: %d", id);
		printk(KERN_INFO "Allocating memory...");
	        head = vmalloc(sizeof(node_t));
		if (head == NULL) {
			return -ENOMEM;
		}
		printk(KERN_INFO "Memory allocated!");
	        head->obj.id = id;
		head->obj.busy = false;
		head->previous = NULL;
	        head->next = NULL;
	        return 0;
	}

	while (cur->next != NULL) {
		prev = cur;
		cur = cur->next;
	}

	printk(KERN_INFO "Creating identity: %d", id);

	// Add a new identity
	printk(KERN_INFO "Allocating memory...");
	cur->next = vmalloc(sizeof(node_t));
	if (cur->next == NULL) {
		return -ENOMEM;
	}
	printk(KERN_INFO "Memory allocated!");
	cur->previous = prev;
	cur->next->obj.id = id;
	cur->next->obj.busy = false;
	cur->next->next = NULL;
	cur->next->previous = cur;

	return 0;
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

    return NULL;
}

static void identity_destroy(int id) {
	node_t *n = identity_find(id);

	printk(KERN_INFO "Destroying identity: %d", id);

	if (n != NULL) {
		printk(KERN_INFO "Found %d!", n->obj.id);
		if (n->previous == NULL) {
			head = n->next;
		} else {
			n->previous->next = n->next;
		}

		if (n->next != NULL) {
			n->next->previous = n->previous;
		}

		vfree(n);
		printk(KERN_INFO "Memory freed");
	}
}

static void __exit argus_exit(void) {
        node_t *cur = head;

        printk(KERN_INFO "Cleaning up all identities...\n");
        while (cur != NULL) {
                node_t *next = cur->next;
                vfree(cur);
                cur = next;
        }

        printk(KERN_INFO "Goodbye!\n");
}

static int __init argus_init(void) {

	int err;
	err = identity_create(1);
	if (err) goto fail_this;
	err = identity_create(2);
	if (err) goto fail_this;

	identity_destroy(1);
	identity_destroy(2);

	printk(KERN_INFO "Fun init!\n");

        return 0; /* success */

	fail_this: 
	argus_exit();
	return err; /* propagate the error */
}

module_init(argus_init);
module_exit(argus_exit);
