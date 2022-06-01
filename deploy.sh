#! /bin/bash

while getopts ":t" opt; do
  case ${opt} in
    t )
        if [[ $OPTARG == "mainnet" ]]; then
            url="https://telos.caleos.io/"
        elif [[ $OPTARG == "testnet" ]]; then
            url="https://testnet.telos.caleos.io/"
        elif [[ $OPTARG == "local" ]]; then
            url="http://127.0.0.1:8888/"
        else
            echo "Please provide a deployment target with [-t] {TARGET_OPTION}. Input: $OPTARG Valid options: [ mainnet, testnet, local ]"
            exit 1
        fi
      ;;
  esac
done

command="cleos --url https://testnet.telos.caleos.io/$url set contract testtesttedc ./build/ eosio.tedp.wasm eosio.tedp.abi -p testtesttedc"
echo $command
eval $command