#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <net/genetlink.h>
#include <net/netlink.h>

#define MY_FAMILY_NAME "calc_family"
#define MY_FAMILY_VERSION 1

enum {
    CMD_CALC = 1,
};

enum {
    ATTR_UNSPEC,
    ATTR_MSG,
    __ATTR_MAX,
};
#define ATTR_MAX (__ATTR_MAX - 1)

static const struct nla_policy calc_policy[ATTR_MAX + 1] = {
    [ATTR_MSG] = { .type = NLA_NUL_STRING },
};

// Прототип функции, чтобы использовать в calc_ops
static int calc_cmd(struct sk_buff *skb, struct genl_info *info);

// Операции для Generic Netlink
static const struct genl_ops calc_ops[] = {
    {
        .cmd = CMD_CALC,
        .flags = 0,
        .policy = calc_policy,
        .doit = calc_cmd,
    },
};

// Описание семейства Generic Netlink
static struct genl_family calc_family = {
    .name = MY_FAMILY_NAME,
    .version = MY_FAMILY_VERSION,
    .maxattr = ATTR_MAX,
    .ops = calc_ops,
    .n_ops = ARRAY_SIZE(calc_ops),
};

// Функция обработки команды calc
static int calc_cmd(struct sk_buff *skb, struct genl_info *info)
{
    char *json_str;
    int result = 0;

    if (!info || !info->attrs[ATTR_MSG])
        return -EINVAL;

    json_str = nla_data(info->attrs[ATTR_MSG]);

    printk(KERN_INFO "calc_family: received JSON: %s\n", json_str);

    // Простейший парсинг JSON (очень упрощённый)
    if (strncmp(json_str, "{\"action\":\"add\"", 15) == 0) {
        int a, b;
        sscanf(json_str, "{\"action\":\"add\",\"argument_1\":%d,\"argument_2\":%d}", &a, &b);
        result = a + b;
    } else if (strncmp(json_str, "{\"action\":\"sub\"", 15) == 0) {
        int a, b;
        sscanf(json_str, "{\"action\":\"sub\",\"argument_1\":%d,\"argument_2\":%d}", &a, &b);
        result = a - b;
    } else if (strncmp(json_str, "{\"action\":\"mul\"", 15) == 0) {
        int a, b;
        sscanf(json_str, "{\"action\":\"mul\",\"argument_1\":%d,\"argument_2\":%d}", &a, &b);
        result = a * b;
    } else {
        printk(KERN_INFO "calc_family: unknown action\n");
        return -EINVAL;
    }

    // Формируем JSON-ответ
    char resp[64];
    snprintf(resp, sizeof(resp), "{\"result\": %d}", result);

    struct sk_buff *skb_out;
    void *msg_head;
    int rc;

    skb_out = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
    if (!skb_out) {
        printk(KERN_ERR "calc_family: failed to allocate skb\n");
        return -ENOMEM;
    }

    msg_head = genlmsg_put(skb_out, 0, info->snd_seq, &calc_family, 0, CMD_CALC);
    if (!msg_head) {
        nlmsg_free(skb_out);
        return -ENOMEM;
    }

    rc = nla_put_string(skb_out, ATTR_MSG, resp);
    if (rc) {
        nlmsg_free(skb_out);
        return rc;
    }

    genlmsg_end(skb_out, msg_head);

    rc = genlmsg_reply(skb_out, info);
    if (rc)
        printk(KERN_ERR "calc_family: genlmsg_reply failed: %d\n", rc);

    return 0;
}

static int __init calc_init(void)
{
    int rc;

    rc = genl_register_family(&calc_family);
    if (rc) {
        pr_err("calc_family: failed to register family: %d\n", rc);
        return rc;
    }

    pr_info("calc_family: registered\n");
    return 0;
}

static void __exit calc_exit(void)
{
    genl_unregister_family(&calc_family);
    pr_info("calc_family: unregistered\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Generic Netlink Calculator Family");
