
# nch.v2.2.0

#### Ubuntu 18.04 Package Install
```sh
wget https://github.com/eosio/eos/releases/download/v2.2.0-rc1/eosio_2.2.0-rc1-ubuntu-18.04_amd64.deb
sudo apt install ./eosio_2.2.0-rc1-ubuntu-18.04_amd64.deb
```
#### Ubuntu Package Uninstall
```sh
sudo apt remove eosio
```

#### RPM Package Install
```sh
wget https://github.com/eosio/eos/releases/download/v2.2.0-rc1/eosio-2.2.0-rc1.el7.x86_64.rpm
sudo yum install ./eosio-2.2.0-rc1.el7.x86_64.rpm
```
#### RPM Package Uninstall
```sh
sudo yum remove eosio
```

## Uninstall Script
To uninstall the EOSIO built/installed binaries and dependencies, run:
```sh
./scripts/eosio_uninstall.sh
```

## Documentation
1. [Nodeos](http://eosio.github.io/eos/latest/nodeos/)
    - [Usage](http://eosio.github.io/eos/latest/nodeos/usage/index)
    - [Replays](http://eosio.github.io/eos/latest/nodeos/replays/index)
    - [Chain API Reference](http://eosio.github.io/eos/latest/nodeos/plugins/chain_api_plugin/api-reference/index)
    - [Troubleshooting](http://eosio.github.io/eos/latest/nodeos/troubleshooting/index)
1. [Cleos](http://eosio.github.io/eos/latest/cleos/)
1. [Keosd](http://eosio.github.io/eos/latest/keosd/)
