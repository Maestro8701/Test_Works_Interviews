#include <iostream>
#include <string>
#include <nlohmann/json.hpp>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

using json = nlohmann::json;

#define MY_FAMILY_NAME "calc_family"
#define CMD_CALC 1
#define ATTR_MSG 1

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <add|sub|mul> <arg1> <arg2>\n";
        return -1;
    }

    std::string action = argv[1];
    int arg1 = std::stoi(argv[2]);
    int arg2 = std::stoi(argv[3]);

    json request = {
        {"action", action},
        {"argument_1", arg1},
        {"argument_2", arg2}
    };
    std::string json_str = request.dump();

    struct nl_sock *sock = nl_socket_alloc();
    if (!sock) {
        std::cerr << "Failed to allocate netlink socket\n";
        return -1;
    }

    if (genl_connect(sock) != 0) {
        std::cerr << "Failed to connect to generic netlink\n";
        nl_socket_free(sock);
        return -1;
    }

    int family_id = genl_ctrl_resolve(sock, MY_FAMILY_NAME);
    if (family_id < 0) {
        std::cerr << "Family not found\n";
        nl_socket_free(sock);
        return -1;
    }

    struct nl_msg *msg = nlmsg_alloc();
    if (!msg) {
        std::cerr << "Failed to allocate netlink message\n";
        nl_socket_free(sock);
        return -1;
    }

    genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, family_id, 0, 0, CMD_CALC, 1);

    nla_put_string(msg, ATTR_MSG, json_str.c_str());

    // Отправляем и ждем ответ
    int err = nl_send_auto(sock, msg);
    nlmsg_free(msg);
    if (err < 0) {
        std::cerr << "Failed to send message\n";
        nl_socket_free(sock);
        return -1;
    }

    // Обработчик ответа
    auto cb = [](struct nl_msg *msg, void *arg) -> int {
        struct nlmsghdr *nlh = nlmsg_hdr(msg);
        struct genlmsghdr *ghdr = (genlmsghdr *)nlmsg_data(nlh);
        struct nlattr *attrs[ATTR_MSG + 1];
        nla_parse(attrs, ATTR_MSG, (struct nlattr *)genlmsg_attrdata(ghdr, 0),
                  genlmsg_attrlen(ghdr, 0), NULL);

        if (attrs[ATTR_MSG]) {
            std::string resp_str((char *)nla_data(attrs[ATTR_MSG]));
            try {
                json resp = json::parse(resp_str);
                std::cout << "Result: " << resp.at("result") << std::endl;
            } catch (...) {
                std::cerr << "Failed to parse JSON response\n";
            }
        }
        return NL_SKIP;
    };

    nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM, cb, nullptr);

    nl_recvmsgs_default(sock);

    nl_socket_free(sock);
    return 0;
}
