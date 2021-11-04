#pragma once

extern "C"
{
#ifdef _WIN64
#include <Rpc.h>
#pragma comment(lib, "Rpcrt4.lib")
#else
#include <uuid/uuid.h>
#endif
}
#include <string>

const std::string ZEROED_UUID = "00000000-0000-0000-0000-000000000000";

std::string newUUID()
{
    #ifdef _WIN64
        UUID uuid;
        const auto status1 = UuidCreate(&uuid);
        if (status1 != RPC_S_OK)
        {
            return ZEROED_UUID;
        }

        unsigned char* str;
        const auto status2 = UuidToStringA(&uuid, &str);
        if (status2 != RPC_S_OK)
        {
            return ZEROED_UUID;
        }

        std::string s((char*)str);

        RpcStringFreeA(&str);
    #else
		uuid_t uuid;
        const auto status1 = uuid_generate_random(uuid);
        if (status1 != 0)
        {
            return ZEROED_UUID;
        }
        char s[37];
        const auto status2 = uuid_unparse(uuid, s);
        if (status2 != 0)
        {
            return ZEROED_UUID;
        }
    #endif
        return s;
}