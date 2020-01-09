#include <string>

#include "tctpl.hpp"

#define RIGHT_KEY               "right"
#define MINTAGE_ADMIN_KEY       "default"
#define EMPTY_ADDRESS           "0x0000000000000000000000000000000000000000"
#define CONTRACT_COMMITTEE_ADDR "0x0000000000000000000000436f6d6d6974746565"

class MintageAdmin : public TCBaseContract
{
    public:
        MintageAdmin() = default;
        ~MintageAdmin() = default;

        void SetAdminAddr(const tc::BInt addr);

        const std::string AdminAddr();

    private:
        tc::StorValue<tc::BInt> m_adminAddr{"m_adminAddr"};
}; // end class MintageAdmin

TC_ABI(MintageAdmin,
    (SetAdminAddr)\
    (AdminAddr)\
)

const tc::Address GetCommitteeAccountAddress(const std::string& rightKey)
{
    tc::Address addr;
    StorMap<Key<std::string>, tc::Address> rights(RIGHT_KEY);

    const tlv::BufferWriter key = Key<std::string>::keyStr(rightKey);
    uint8_t* tmp = rights.getKeyBytes(key);
    uint8_t* value = TC_ContractStoragePureGet(CONTRACT_COMMITTEE_ADDR, tmp, std::string(RIGHT_KEY).length() + key.length());
    free(tmp);

    tc::tlv::BufferReader buffer(value);
    unpack(buffer, addr);

	return addr;
}

bool CheckAddrRight(const tc::Address& addr, const std::string& key) {
	return addr == GetCommitteeAccountAddress(key);
}

void MintageAdmin::SetAdminAddr(const tc::BInt addr)
{
    TC_Payable(false);
    TC_RequireWithMsg(addr != EMPTY_ADDRESS, "admin address can not be empty address");
    const tc::Address& sender = tc::App::getInstance()->sender();
    TC_RequireWithMsg(CheckAddrRight(sender, MINTAGE_ADMIN_KEY), "Address does not have permission");
    m_adminAddr.set(addr);
}

const std::string MintageAdmin::AdminAddr()
{
    TC_Payable(false);
    if (m_adminAddr.get() == EMPTY_ADDRESS) {
        return GetCommitteeAccountAddress(MINTAGE_ADMIN_KEY).toString();
    }
    return m_adminAddr.get().toString();
}