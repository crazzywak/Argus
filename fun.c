#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yakir Dorani");
MODULE_DESCRIPTION("Argus interview");
MODULE_VERSION("0.01");

#define IDENTITY_NAME_LEN 20

typedef struct identity {
    char name[IDENTITY_NAME_LEN];
    int id;
    bool busy;
    struct list_head list;
} identity;

static LIST_HEAD(identity_list);

static struct kmem_cache *identity_cache;

static identity* identity_find(int id) {
        struct identity *cur;
        pr_debug("Finding identity: %d", id);
        list_for_each_entry(cur, &identity_list, list) {
                if (cur->id == id) {
			pr_debug("Found identity: %d", id);
                        return cur;
		}
        }

        pr_debug("Identity %d not found!", id);

        return NULL;
}

static int identity_create(char *name, int id) {
	identity *temp;
	int retval = -EINVAL;

	if (identity_find(id))
		goto out;

	temp = kmem_cache_alloc(identity_cache, GFP_KERNEL);
	if (!temp) {
		goto out;
	}

	strncpy(temp->name, name, IDENTITY_NAME_LEN);
	temp->name[IDENTITY_NAME_LEN-1] = '\0';
	temp->id = id;
	temp->busy = false;
	list_add(&(temp->list), &identity_list);
	retval = 0;

	pr_debug("identity %d: %s created\n", id, name);

	out:
	return retval;
}

static void identity_destroy(int id) {
	identity *item;
	pr_debug("Destroying identity: %d", id);
	item = identity_find(id);

	if (!item)
		return;

	list_del(&(item->list));
	kmem_cache_free(identity_cache, item);
}

static void __exit argus_exit(void) {
	pr_debug("Cleaning everything...\n");
	if (identity_cache)
		kmem_cache_destroy(identity_cache);

        pr_debug("Goodbye!\n");
}

static void identity_constructor(void *addr)
{
    memset(addr, 0, sizeof(identity));
}

static int __init argus_init(void) {

	int err;
	identity* ret;

	pr_debug("Argus exercise init!\n");

	identity_cache = kmem_cache_create("identity",
					   sizeof(struct identity),
					   0, 0, identity_constructor);
	if (!identity_cache)
		return -ENOMEM;

	err = identity_create("Alice", 1);
	if (err) goto fail_this;
	err = identity_create("Bob", 2);
	if (err) goto fail_this;
	err = identity_create("Dave", 3);
	if (err) goto fail_this;
	err = identity_create("Gena", 10);
	if (err) goto fail_this;

	ret = identity_find(3);
	pr_debug("id 3 = %s\n", ret->name);
	ret = identity_find(42);
	if (ret == NULL)
		pr_debug("id 42 not found\n");

	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	pr_debug("Done init!\n");

        return 0; /* success */

	fail_this:
	pr_debug("Failure initializing, cleaning everything...");
	argus_exit();
	return err; /* propagate the error */
}

module_init(argus_init);
module_exit(argus_exit);
