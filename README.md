# Telos Distribute

This native Telos smart contract sends rewards periodically from Telos' reserves to a list of receivers such as REX, STLOS on EVM, TCD & TF.

## Requirements

This repository requires `cleos` installed

## Rundown

Payout are sent periodically by calling the public `pay` action.

If the action isn't called within the time period set in configuration, payouts are accrued over time until then. 

The smart contract also split Staking rewards between Native's REX and EVM's sTLOS with a ratio of the total TLOS locked in each and applies as well an extra, configurable, ratio to the EVM rewards.

## Configuration

The EVM staking can be configured using the following actions:

- `setratio(uint64_t ratio_value)`
  - The `ratio_value` is multiplied to the rewards sent to EVM, a ratio of 90 will for example decrease EVM rewards by 10%

- `setevmconfig(string stlos_contract, eosio::checksum256 storage_key, uint64_t wtlos_index)`
  - `stlos_contract` is your StakedTLOS EVM contract address
  - `storage_key` is the computed storage key for the STLOS contract WTLOS balance
  - `wtlos_index` is the index of WTLOS in the eosio.evm accounts table

### Get the WTLOS index

The WTLOS index can be found on eosio.evm's accounts table using the WTLOS EVM address (without the 0x)

### Get the storage key

The storage key we are looking for is part of the `address => uint` balance mapping at position 2 of the WTLOS contract. We need to access the mapping value corresponding to the STLOS address in order to retreive its WTLOS balance.

To compute that storage key, you can use the following snippet that uses the ethers library:
```
    const provider = ethers.getDefaultProvider("https://testnet.telos.net/evm");

    const wtlos = "0xaE85Bf723A9e74d6c663dd226996AC1b8d075AA9"; // WTLOS address on testnet
    const stlos = "YOUR_STLOS_CONTRACT_ADDRESS"; // StakedTLOS address on testnet

    // Get the stlos balance slot aka our storage key
    const stlos_balance_slot = ethers.utils.keccak256(
        ethers.utils.hexZeroPad("0x02", 32), // Our mapping position
        ethers.utils.hexZeroPad(stlos, 32), // The stlos key
    );
    console.log("Storage key:", stlos_balance_slot);
```
