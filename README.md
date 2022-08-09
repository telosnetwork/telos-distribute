# Telos Distribute

This native Telos smart contract sends rewards periodically from Telos' reserves to a list of receivers such as REX, STLOS on EVM, TCD & TF.

## Requirements

This repository requires

## Rundown

Payout are sent periodically by calling the public `pay` action on the __exrsrv.tf__ contract

If the action isn't called within the time period set in configuration, payouts are accrued over time until then. 
