
# v2.0.13

#### Ubuntu 18.04 Package Install
```sh
wget https://github.com/eosio/eos/releases/download/v2.0.13/eosio_2.0.13-1-ubuntu-18.04_amd64.deb
sudo apt install ./eosio_2.0.13-1-ubuntu-18.04_amd64.deb
```
#### Ubuntu 16.04 Package Install
```sh
wget https://github.com/eosio/eos/releases/download/v2.0.13/eosio_2.0.13-1-ubuntu-16.04_amd64.deb
sudo apt install ./eosio_2.0.13-1-ubuntu-16.04_amd64.deb
```
#### Ubuntu Package Uninstall
```sh
sudo apt remove eosio
```

## Uninstall Script
To uninstall the EOSIO built/installed binaries and dependencies, run:
```sh
./scripts/eosio_uninstall.sh
```

### TODO
core
	new action
		onbilltrxs
			[{trx_id,account,action,ram,cpu}]
system contract
	new action
		onbilltrxs
			pay for each trx