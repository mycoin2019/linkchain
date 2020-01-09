#include <map>
#include <string>

#include "tctpl.hpp"

#define RIGHT_KEY               "right"
#define MINTAGE_WHITELIST_KEY   "default"
#define EMPTY_ADDRESS           "0x0000000000000000000000000000000000000000"
#define CONTRACT_COMMITTEE_ADDR "0x0000000000000000000000436f6d6d6974746565"

class MintageWhitelist : public TCBaseContract
{
    public:
        MintageWhitelist()  = default;
        ~MintageWhitelist() = default;

        void Insert(const std::string coinName, const tc::Address tokenId);

        void Delete(const std::string coinName);

        void Update(const std::string coinName, const tc::Address tokenId);

        std::string Select(const std::string coinName);

    private:
        StorMap<Key<std::string>, tc::Address> m_mapWhitelist{"m_mapWhitelist"};
}; // end class MintageWhitelist

TC_ABI(MintageWhitelist,
    (Insert)\
    (Delete)\
    (Update)\
    (Select)\
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

bool IsCommitteeAccountAddress(const tc::Address& addr, const std::string& key) {
	return addr == GetCommitteeAccountAddress(key);
}

void MintageWhitelist::Insert(const std::string coinName, const tc::Address tokenId)
{
    TC_Payable(false);
    TC_RequireWithMsg(coinName.length() > 0, "coin name param not set");
    TC_RequireWithMsg(tokenId != EMPTY_ADDRESS, "tokenId param illegal");
    const tc::Address& sender = tc::App::getInstance()->sender();
    TC_RequireWithMsg(IsCommitteeAccountAddress(sender, MINTAGE_WHITELIST_KEY), "Address does not have permission");
    tc::Address checkAddr = m_mapWhitelist.get(coinName);
    TC_RequireWithMsg(checkAddr == EMPTY_ADDRESS, "this mintage contarct has been set");
    m_mapWhitelist.set(tokenId, coinName);
}

void MintageWhitelist::Delete(const std::string coinName)
{
    TC_Payable(false);
    TC_RequireWithMsg(coinName.length() > 0, "coin name param not set");
    const tc::Address& sender = tc::App::getInstance()->sender();
    TC_RequireWithMsg(IsCommitteeAccountAddress(sender, MINTAGE_WHITELIST_KEY), "Address does not have permission");
    tc::Address checkAddr = m_mapWhitelist.get(coinName);
    TC_RequireWithMsg(checkAddr != EMPTY_ADDRESS, "this mintage contarct not exist");
    tc::Address emptyTokenId = EMPTY_ADDRESS;
    m_mapWhitelist.set(emptyTokenId, coinName);
}

void MintageWhitelist::Update(const std::string coinName, const tc::Address tokenId)
{
    TC_Payable(false);
    TC_RequireWithMsg(coinName.length() > 0, "coin name param not set");
    TC_RequireWithMsg(tokenId != EMPTY_ADDRESS, "tokenId param illegal");
    const tc::Address& sender = tc::App::getInstance()->sender();
    TC_RequireWithMsg(IsCommitteeAccountAddress(sender, MINTAGE_WHITELIST_KEY), "Address does not have permission");
    tc::Address checkAddr = m_mapWhitelist.get(coinName);
    TC_RequireWithMsg(checkAddr != EMPTY_ADDRESS, "this mintage contarct not set");
    m_mapWhitelist.set(tokenId, coinName);
}

std::string MintageWhitelist::Select(const std::string coinName)
{
    TC_Payable(false);
    TC_RequireWithMsg(coinName.length() > 0, "coin name param not set");
    tc::Address tokenId = m_mapWhitelist.get(coinName);
    TC_RequireWithMsg(tokenId != EMPTY_ADDRESS, "this mintage contarct not exist");
    return tokenId.toString();
}
