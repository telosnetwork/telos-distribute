# Telos Distribute

This native Telos smart contract sends rewards periodically from Telos' reserves to a list of receivers such as REX, STLOS on EVM, TCD & TF.

## Requirements

This repository requires `cleos` installed

## Rundown

Payout are sent periodically by calling the public `pay` action.

If the action isn't called within the time period set in configuration, payouts are accrued over time until then. 

The smart contract also split Staking rewards between native REX and EVM's sTLOS with a ratio of the total TLOS locked in each and applies an extra, configurable, ratio to the EVM rewards.
