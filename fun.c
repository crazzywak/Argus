#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yakir Dorani");
MODULE_DESCRIPTION("Argus interview");
MODULE_VERSION("0.01");

#define IDENTITY_NAME_LEN 15

typedef struct identity {
    char name[IDENTITY_NAME_LEN];
    int id;
    bool busy;
    struct list_head list;
} identity;

static LIST_HEAD(identity_list);

static identity* identity_find(int id) {
        struct identity *cur;
        pr_debug("Finding identity: %d", id);
        list_for_each_entry(cur, &identity_list, list) {
                if (cur->id == id)
                        return cur;
        }

        pr_debug("Identity %d not found!", id);

        return NULL;
}

static int identity_create(char *name, int id) {
	identity *temp;
	int retval = -EINVAL;

	if (identity_find(id))
		goto out;

	temp = vmalloc(sizeof(identity));
	if (!temp)
		goto out;

	strncpy(temp->name, name, IDENTITY_NAME_LEN);
	temp->name[IDENTITY_NAME_LEN-1] = '\0';
	temp->id = id;
	temp->busy = false;
	list_add(&(temp->list), &identity_list);
	retval = 0;

	pr_debug("identity %d: %s created\n", id, name);

	out:	return retval;
}

static void identity_destroy(int id) {
	identity *temp;
	pr_debug("Destroying identity: %d", id);
	temp = identity_find(id);

	if (!temp)
		return;

	pr_debug("destroying identity %i: %s\n", temp->id, temp->name);

	list_del(&(temp->list));
	vfree(temp);

	return;
}

static void __exit argus_exit(void) {
	identity* cur;
	pr_debug("Cleaning everything...\n");
	list_for_each_entry(cur, &identity_list, list) {
		vfree(cur);
        }

        pr_debug("Goodbye!\n");
}

static int __init argus_init(void) {

	int err;
	identity* ret;
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

	pr_debug("Fun init!\n");

        return 0; /* success */

	fail_this: 
	argus_exit();
	return err; /* propagate the error */
}

module_init(argus_init);
module_exit(argus_exit);
