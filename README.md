#Build & test with eosio.bp repository

##########Build
git clone git@github.com:coffeio/eosio.core.git --branch testnet /var/server/eosio.core --recurse-submodules
cd /var/server/eosio.core && git checkout testnet && git submodule update --init --recursive

##### Update changes on Ubuntu
cd /var/server/eosio.core && git commit -a "Test build" && git push

##### Receive changes on Ubuntu
git pull origin testnet --recurse-submodules
git checkout testnet && git submodule update --init --recursive

chmod +x /var/server/eosio.core/scripts/eosio_build.sh
cd /var/server/eosio.core && ./scripts/eosio_build.sh

###### Build done. Install
### Copy files
cp -rf /var/server/eosio.core/build/programs/nodeos/nodeos /var/server/bp/nodeos

sudo systemctl stop NODEOS1 && sudo systemctl stop NODEOS2
sudo systemctl restart NODEOS1 && sudo systemctl restart NODEOS2
journalctl -f -u NODEOS1


##### Install BP

sudo apt-get update
sudo apt-get install -y psmisc zip unzip curl jq libncurses5
sudo apt-get update
curl -sL https://deb.nodesource.com/setup_18.x | sudo -E bash -
sudo apt-get install -y nodejs && nodejs-legacy && npm
	###If error libc6
	curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.39.1/install.sh | bash
	source ~/.bashrc
	nvm ls-remote
	nvm install v16.15.1
	nvm use v16.15.1
	node -v
sudo npm install pm2@latest -g
npm install -g npm@9.8.1
sudo pm2 startup
pm2 save

git clone git@github.com:coffeio/eosio.bp.git /var/server/bp
git pull origin master

#Test run
./nodeos -v
##### If error
libicuuc.so.60: cannot open shared
wget http://security.ubuntu.com/ubuntu/pool/main/i/icu/libicu60_60.2-3ubuntu3.2_amd64.deb
sudo apt-get install ./libicu60_60.2-3ubuntu3.2_amd64.deb

##### Run from snapshot
rm -r /var/server/bp/traces/* -R
rm -r /var/server/bp/blocks/* -R
rm -r /var/server/bp/state-history/* -R
rm -r /var/server/bp/datadir/* -R
/var/server/bp/nodeos --snapshot /var/server/bp/snapshots/snapshot-008512fc943cd8ad71233bc144b20f4761b05906275e50ffd014b66e1136f720.bin --config /var/server/bp/config.ini --data-dir /var/server/bp/datadir --verbose-http-errors --disable-replay-opts

###Setup
cd /var/server/bp
mkdir /root/eosio-wallet

mkdir /var/server/bp/test1
mkdir /var/server/bp/test1/traces
mkdir /var/server/bp/test1/blocks
mkdir /var/server/bp/test1/state-history
mkdir /var/server/bp/test1/datadir
mkdir /var/server/bp/test1/snapshots

mkdir /var/server/bp/test2
mkdir /var/server/bp/test2/traces
mkdir /var/server/bp/test2/blocks
mkdir /var/server/bp/test2/state-history
mkdir /var/server/bp/test2/datadir
mkdir /var/server/bp/test2/snapshots

sudo cp -rf /var/server/bp/KEOSD.service /etc/systemd/system/
sudo cp -rf /var/server/bp/test/*.service /etc/systemd/system/
sudo install -m 0755 -o root -g root -t /usr/local/bin /var/server/bp/cleos
sudo systemctl daemon-reload
sudo systemctl enable KEOSD
sudo systemctl restart KEOSD
sudo systemctl enable NODEOS1
sudo systemctl enable NODEOS2
sudo systemctl restart NODEOS1
sudo systemctl restart NODEOS2
sudo systemctl stop NODEOS1
sudo systemctl stop NODEOS2
# check the status
sudo systemctl stop KEOSD
sudo systemctl status KEOSD
journalctl -f -u KEOSD
journalctl -f -u NODEOS1
journalctl -f -u NODEOS2


##### START
###create wallet

#create PK for SYSTEM 
5Hr2BvKGQxXs8bJDDcaPBQ7GpA1oyim7TsqAXEjqNAJbuHdsq8s
EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4

#create PK for BPs 
5J8e9EbC12TuAMggvceqhqpesdBCXCLxeiQo9qZiHVjZn4gre8t
EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
5JySFsD6q5d8uNbkhGodSbeFPg9kPLntrA5Msuzz2KvtqapkcX9
EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz

#create PK for Dapps
5J28BQR7smwRH5gyjcEMVkPhFWCEuFn2ukBZv7NaWvqbbxERHgF
EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu

###Setup wallets
curl http://127.0.0.1:8900/v1/wallet/list_wallets
curl http://127.0.0.1:8900/v1/wallet/create -X POST -d '"gf"'
PW5KQ9jqpSvJFqCnZPYY5R69zd66AQWhpMjoJ5YZsmCFeMkGyTr9d
curl http://127.0.0.1:8900/v1/wallet/open -X POST -d '"gf"'
curl http://127.0.0.1:8900/v1/wallet/unlock -X POST -d '["gf", "PW5KQ9jqpSvJFqCnZPYY5R69zd66AQWhpMjoJ5YZsmCFeMkGyTr9d"]'

#Import for EOSIO
curl http://127.0.0.1:8900/v1/wallet/import_key -X POST -d '["gf","5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"]'
#Import our
curl http://127.0.0.1:8900/v1/wallet/import_key -X POST -d '["gf","5Hr2BvKGQxXs8bJDDcaPBQ7GpA1oyim7TsqAXEjqNAJbuHdsq8s"]'
curl http://127.0.0.1:8900/v1/wallet/import_key -X POST -d '["gf","5J8e9EbC12TuAMggvceqhqpesdBCXCLxeiQo9qZiHVjZn4gre8t"]'
curl http://127.0.0.1:8900/v1/wallet/import_key -X POST -d '["gf","5JySFsD6q5d8uNbkhGodSbeFPg9kPLntrA5Msuzz2KvtqapkcX9"]'
curl http://127.0.0.1:8900/v1/wallet/import_key -X POST -d '["gf","5J28BQR7smwRH5gyjcEMVkPhFWCEuFn2ukBZv7NaWvqbbxERHgF"]'
###Start node with eosio producer
	#####EMPTY START
./nodeos --config /var/server/bp/test/test1.ini --verbose-http-errors --disable-replay-opts --delete-all-blocks
	#####SECOND RESTART
./nodeos --config /var/server/bp/test/test1.ini --verbose-http-errors --disable-replay-opts

curl http://127.0.0.1:18881/v1/chain/get_info | jq
curl http://127.0.0.1:18882/v1/chain/get_info | jq

##### Create system account
cleos -u http://127.0.0.1:18881 create account eosio eosio.bpay EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.vpay EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.msig EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.ram EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.ramfee EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.stake EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.token EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.wrap EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.bios EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.rex EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.saving EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.names EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4
cleos -u http://127.0.0.1:18881 create account eosio eosio.prods EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4 EOS5TxTLYuUgwUniwjTNke1BebcGRv5KBVqWiucwWB4Lb7ftHTaV4

##### Create dapps account
cleos -u http://127.0.0.1:18881 create account eosio gf EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.asset EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.hold EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.nft EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.swap EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.address EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.fee EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.price EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.types EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.dex EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu
cleos -u http://127.0.0.1:18881 create account eosio gf.reg EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu EOS69MsMSD6X745uLskmva7GXvAvXNpoXApRvRvQG819H9GEpa4Eu

# init features
curl -X POST http://127.0.0.1:18881/v1/producer/schedule_protocol_feature_activations -d '{"protocol_features_to_activate": ["0ec7e080177b2c02b278d5088611686b49d739925a92d9bfcacd7fc6b74053bd"]}' | jq # PREACTIVATE_FEATURE

##### deploy bios
## deploy bios from 1.6.x
cleos -u http://127.0.0.1:18881 set contract eosio /var/server/dapps/eosio.contracts/build/contracts/eosio.bios/ eosio.bios.wasm eosio.bios.abi -p eosio@active
## activate WTMSIG_BLOCK_SIGNATURES
cleos -u http://127.0.0.1:18881 push action eosio activate '["299dcb6af692324b899b39f16d5a530a33062804e41f09dc97e9f156b4476707"]' -p eosio # WTMSIG_BLOCK_SIGNATURES
##deploy bios from 1.9.x
cleos -u http://127.0.0.1:18881 set contract eosio /var/server/contracts/build/contracts/eosio.bios/ eosio.bios.wasm eosio.bios.abi -p eosio@active

# init features
cleos -u http://127.0.0.1:18881 push action eosio activate '["f0af56d2c5a48d60a4a5b5c903edfb7db3a736a94ed589d0b797df33ff9d3e1d"]' -p eosio # GET_SENDER
cleos -u http://127.0.0.1:18881 push action eosio activate '["2652f5f96006294109b3dd0bbde63693f55324af452b799ee137a81a905eed25"]' -p eosio # FORWARD_SETCODE
cleos -u http://127.0.0.1:18881 push action eosio activate '["8ba52fe7a3956c5cd3a656a3174b931d3bb2abb45578befc59f283ecd816a405"]' -p eosio # ONLY_BILL_FIRST_AUTHORIZER
cleos -u http://127.0.0.1:18881 push action eosio activate '["68dcaa34c0517d19666e6b33add67351d8c5f69e999ca1e37931bc410a297428"]' -p eosio # DISALLOW_EMPTY_PRODUCER_SCHEDULE
cleos -u http://127.0.0.1:18881 push action eosio activate '["e0fb64b1085cc5538970158d05a009c24e276fb94e1a0bf6a528b48fbc4ff526"]' -p eosio # FIX_LINKAUTH_RESTRICTION
cleos -u http://127.0.0.1:18881 push action eosio activate '["ef43112c6543b88db2283a2e077278c315ae2c84719a8b25f25cc88565fbea99"]' -p eosio # REPLACE_DEFERRED
cleos -u http://127.0.0.1:18881 push action eosio activate '["4a90c00d55454dc5b059055ca213579c6ea856967712a56017487886a4d4cc0f"]' -p eosio # NO_DUPLICATE_DEFERRED_ID
cleos -u http://127.0.0.1:18881 push action eosio activate '["ad9e3d8f650687709fd68f4b90b41f7d825a365b02c23a636cef88ac2ac00c43"]' -p eosio # RESTRICT_ACTION_TO_SELF
cleos -u http://127.0.0.1:18881 push action eosio activate '["1a99a59d87e06e09ec5b028a9cbb7749b4a5ad8819004365d02dc4379a8b7241"]' -p eosio # ONLY_LINK_TO_EXISTING_PERMISSION

##### deploy token contracts
cleos -u http://127.0.0.1:18881 set contract eosio.token /var/server/contracts/build/contracts/eosio.token eosio.token.wasm eosio.token.abi -p eosio.token@active

##### deploy msig contract
cleos -u http://127.0.0.1:18881 set contract eosio.msig /var/server/contracts/build/contracts/eosio.msig eosio.msig.wasm eosio.msig.abi -p eosio.msig@active
cleos -u http://127.0.0.1:18881 push action eosio setpriv '["eosio.msig", 1]' -p eosio@active
cleos -u http://127.0.0.1:18881 push action eosio setpriv '["eosio.bios", 1]' -p eosio@active

# deploy wrap contract
cleos -u http://127.0.0.1:18881 set contract eosio.wrap /var/server/contracts/build/contracts/eosio.wrap eosio.wrap.wasm eosio.wrap.abi -p eosio.wrap@active
cleos -u http://127.0.0.1:18881 push action eosio setpriv '["eosio.wrap", 1]' -p eosio@active

# deploy network DAPPS
cleos -u http://127.0.0.1:18881 set contract gf.address /var/server/dapps/gf.address -p gf.address@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.address active

cleos -u http://127.0.0.1:18881 set contract gf.asset /var/server/dapps/gf.asset -p gf.asset@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.asset active
cleos -u http://127.0.0.1:18881 push action gf.asset configure '["0.0002 GFT", 1024, 102400]' -p gf.asset@active

cleos -u http://127.0.0.1:18881 set contract gf.dex /var/server/dapps/gf.dex -p gf.dex@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.dex active

cleos -u http://127.0.0.1:18881 set contract gf.fee /var/server/dapps/gf.fee -p gf.fee@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.fee active

cleos -u http://127.0.0.1:18881 set contract gf.hold /var/server/dapps/gf.hold -p gf.hold@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.hold active

cleos -u http://127.0.0.1:18881 set contract gf.nft /var/server/dapps/gf.nft -p gf.nft@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.nft active

cleos -u http://127.0.0.1:18881 set contract gf.price /var/server/dapps/gf.price -p gf.price@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.price active

cleos -u http://127.0.0.1:18881 set contract gf.types /var/server/dapps/gf.types -p gf.types@active
cleos -u http://127.0.0.1:18881 set account permission --add-code gf.types active

cleos -u http://127.0.0.1:18881 push action gf.types create '[ "GF", "GF", "https://dev.globalforce.io/transaction/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "BITCOIN", "BTC", "https://www.blockchain.com/btc/tx/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "BINANCE", "BEP20", "https://bscscan.com/tx/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "ETHEREUM", "ETH", "https://etherscan.io/tx/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "ETHEREUM", "ERC20", "https://etherscan.io/tx/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "TRON", "TRX", "https://tronscan.org/#/transaction/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "TRON", "TRC10", "https://tronscan.org/#/transaction/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "TRON", "TRC20", "https://tronscan.org/#/transaction/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "POLYGON", "MATIC", "https://polygonscan.com/tx/" ]' -p gf.types@active
cleos -u http://127.0.0.1:18881 push action gf.types create '[ "POLYGON", "MATIC20", "https://polygonscan.com/tx/" ]' -p gf.types@active

cleos -u http://127.0.0.1:18881 get table gf.types gf.types types

# create token 
cleos -u http://127.0.0.1:18881 push action eosio.token create '[ "eosio", "10000000000.0000 GFT" ]' -p eosio.token@active

cleos -u http://127.0.0.1:18881 push action eosio.token configs '["4,GFT",[{"network":"GF","transfer":0,"swapout":0}], 1, 1,1,  20,0, 180,0]' -p gf@active

cleos -u http://127.0.0.1:18881 push action eosio.token issue '[ "eosio", "700000000.0000 GFT", "" ]' -p eosio@active
cleos -u http://127.0.0.1:18881 set account permission --add-code eosio.token active

cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "eosio", "gf", "700000000.0000 GFT", "init" ]' -p eosio@active

cleos -u http://127.0.0.1:18881 push action gf.fee create '[ "GF", "GF", "eosio.token", "4,GFT" ]' -p gf.fee@active

cleos -u http://127.0.0.1:18881 get table gf.fee gf.fee fees

cleos -u http://127.0.0.1:18881 push action gf.price create '[ "eosio.token", "4,USDT", "GFT", 4]' -p gf.price@active
cleos -u http://127.0.0.1:18881 push action gf.price update '[ "eosio.token", "0.0010 USDT"]' -p gf.price@active
cleos -u http://127.0.0.1:18881 push action gf.price update '[ "eosio.token", "0.0011 USDT"]' -p gf.price@active
cleos -u http://127.0.0.1:18881 push action gf.price update '[ "eosio.token", "0.0012 USDT"]' -p gf.price@active

cleos -u http://127.0.0.1:18881 push action gf.nft cncreate '[ "First NFT collection", 0, "gf", "Description."]' -p gf.nft@active

##### deploy system contract
cleos -u http://127.0.0.1:18881 set contract eosio /var/server/contracts/build/contracts/eosio.system eosio.system.wasm eosio.system.abi -p eosio@active

cleos -u http://127.0.0.1:18881 push action eosio configfee '[15, 24]' -p eosio@active
cleos -u http://127.0.0.1:18881 get table eosio eosio configfee -l 100

cleos -u http://127.0.0.1:18881 push action eosio holdfeebp '[20]' -p eosio@active
cleos -u http://127.0.0.1:18881 get table eosio eosio holdfeebp -l 100

cleos -u http://127.0.0.1:18881 push action eosio configbp '["10000.0000 GFT", "50.0000 GFT", 0, 60]' -p eosio@active
cleos -u http://127.0.0.1:18881 get table eosio eosio configbp -l 100

cleos -u http://127.0.0.1:18881 get table eosio eosio approvebp -l 100

# init system
cleos -u http://127.0.0.1:18881 push action eosio init '[0, "4,GFT"]' -p eosio@active

cleos -u http://127.0.0.1:18881 get account eosio

# Resign
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio.prods", "permission": "active"}}]}}' -p eosio@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio.prods", "permission": "active"}}]}}' -p eosio@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.bpay", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.bpay@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.bpay", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.bpay@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.msig", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.msig@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.msig", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.msig@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.names", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.names@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.names", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.names@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.ram", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ram@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.ram", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ram@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.ramfee", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ramfee@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.ramfee", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.ramfee@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.saving", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.saving@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.saving", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.saving@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.stake", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.stake@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.stake", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.stake@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.token", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.token@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.token", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.token@active

cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.vpay", "permission": "owner", "parent": "", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.vpay@owner
cleos -u http://127.0.0.1:18881 push action eosio updateauth '{"account": "eosio.vpay", "permission": "active", "parent": "owner", "auth": {"threshold": 1, "keys": [], "waits": [], "accounts": [{"weight": 1, "permission": {"actor": "eosio", "permission": "active"}}]}}' -p eosio.vpay@active

/************************************************************************************************/
/************************************      BP       *********************************************/
/************************************************************************************************/

#####Create BPs accounts
cleos -u http://127.0.0.1:18881 create account gf testtestbpa1 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpa2 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpa3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpa4 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpa5 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpb1 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpb2 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpb3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpb4 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3
cleos -u http://127.0.0.1:18881 create account gf testtestbpb5 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3

cleos -u http://127.0.0.1:18881 create account gf testtestbpc1 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpc2 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpc3 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpc4 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpc5 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpd1 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpd2 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpd3 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpd4 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz
cleos -u http://127.0.0.1:18881 create account gf testtestbpd5 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz

cleos -u http://127.0.0.1:18881 get account testtestbpd5

# transfer token from gf
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "eosio.bpay", "1000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "eosio.vpay", "1000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "gf.reg", "1000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "gf.hold", "1000.0000 GFT", "applystake"]' -p gf@active
cleos -u http://127.0.0.1:18881 push action gf.hold stake '[ "gf", "10.0000 GFT", ""]' -p gf@active

cleos -u http://127.0.0.1:18881 get table gf.hold gf.hold stake
cleos -u http://127.0.0.1:18881 get table gf.hold gf.hold stakeconfig

	### Reg BP
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpa1", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpa2", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpa3", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpa4", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpa5", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpb1", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpb2", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpb3", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpb4", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpb5", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpc1", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpc2", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpc3", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpc4", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpc5", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpd1", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpd2", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpd3", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpd4", "100000.0000 GFT", "init" ]' -p gf@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf", "testtestbpd5", "100000.0000 GFT", "init" ]' -p gf@active

cleos -u http://127.0.0.1:18881 system regproducer testtestbpa1 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpa2 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpa3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpa4 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpa5 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpb1 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpb2 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpb3 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpb4 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpb5 EOS76LEsyLS7ReSeiY5GrhetCEcBfk1xh7eky78qgjvR24ycpk2q3 "http://" 0

cleos -u http://127.0.0.1:18881 system regproducer testtestbpc1 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpc2 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpc3 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpc4 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpc5 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpd1 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpd2 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpd3 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpd4 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0
cleos -u http://127.0.0.1:18881 system regproducer testtestbpd5 EOS69nZsgth8C3kwqWXPtH9pMLU4evnJD3bfj3JgDNpxa8Kkq6bvz "http://" 0

cleos -u http://127.0.0.1:18881 get account testtestbpa1

cleos -u http://127.0.0.1:18881 get table eosio eosio producers -l 100
cleos -u http://127.0.0.1:18881 get table eosio eosio configbp -l 100
cleos -u http://127.0.0.1:18881 get table eosio eosio approvebp -l 100



cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce1"]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce2"]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce3"]' -p globalforce3@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp1"]' -p gf.bp1@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp2"]' -p gf.bp2@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp3"]' -p gf.bp3@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp4"]' -p gf.bp4@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["krio"]' -p krio@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["world"]' -p world@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["stuff"]' -p stuff@active

cleos -u http://127.0.0.1:18881 push action gf.hold stake '[ "globalforce1", "2500000.0000 GFT", ""]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action gf.hold stake '[ "globalforce2", "2500000.0000 GFT", ""]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action gf.hold stake '[ "globalforce3", "2500000.0000 GFT", ""]' -p globalforce3@active

cleos -u http://127.0.0.1:18881 push action gf.hold unstake '[ "gf", "200010.0000 GFT", ""]' -p gf@active
cleos -u http://127.0.0.1:18881 push action gf.hold unstake '[ "globalforce1", "6000000.0000 GFT", ""]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action gf.hold claim '["gf"]' -p gf@active
cleos -u http://127.0.0.1:18881 push action gf.hold claim '[ "globalforce1" ]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action gf.hold claim '[ "globalforce2" ]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action gf.hold claim '[ "globalforce3" ]' -p globalforce3@active

cleos -u http://127.0.0.1:18881 get table eosio eosio producers -l 100

curl -X 'POST' 'http://127.0.0.1:18881/v1/chain/get_table_rows' -d '{"json":true, "code":"eosio", "scope":"eosio", "table":"producers", "limit":1000 }' | jq


### New register BP with deposit
cleos -u http://127.0.0.1:18881 push action eosio configbp '["20000000.0000 GFT", "19.3022 GFT", 12251182, 5184000]' -p eosio@active

cleos -u http://127.0.0.1:18881 system regproducer globalforce1 EOS56dNpGCjTmGaFZkcyx4QBoGyCjioZwC7CrZB8P6eTyRawf7M77 "https://globalforce.io" 0
cleos -u http://127.0.0.1:18881 system regproducer globalforce2 EOS7zwP5vfHNMAqnFnuDcUwvhQBhi4WUH5hyxotJx7uEb1wXcHeLq "https://globalforce.io" 0	
cleos -u http://127.0.0.1:18881 system regproducer globalforce3 EOS5kVy4r3FNbdC3nUcK84eMbraEAKN6UUiANDTZygiruvWuVagCw "https://globalforce.io" 0
cleos -u http://127.0.0.1:18881 system regproducer gf.bp1 EOS8KRG1vA3Rc4RxWzo8rM72ZPZCv42hJwtxmxmq8vTJi8yJNsSNN "https://" 0
cleos -u http://127.0.0.1:18881 system regproducer gf.bp2 EOS6f6eya4WrTvccVPtLhobZrYMEBXGkzMMFHviQwTT3Jhqm2CfUs "https://" 0
cleos -u http://127.0.0.1:18881 system regproducer gf.bp3 EOS8YEaqzUkzmV7djUgmSryQ688rirR7xHXv7Vf9jcEysa1qCGQM2 "https://" 0
cleos -u http://127.0.0.1:18881 system regproducer gf.bp4 EOS76ibHzV7R1uBg9TC8WyB9cwZKxgcUFPLJxX3WJn6dA8CmxkEM2 "https://" 0	
cleos -u http://127.0.0.1:18881 system regproducer krio EOS8Z4kwARuk2LGqvXX9YuBXLNY3RcUUYLGLifNvPLspaCNhxC7j4 "https://" 0	
cleos -u http://127.0.0.1:18881 system regproducer world EOS8Dw7JTigsPS179t4hA4asvPWD6CKfjHNQieZbxi7XALFv8obpo "https://" 0	
cleos -u http://127.0.0.1:18881 system regproducer stuff EOS6hFPr7CPK4qwLUurnhienZ5AzdkGuMNe3QUwFGcJfqsnAhw6mv "https://" 0	

	##### check configs if need
cleos -u http://127.0.0.1:18881 get table eosio eosio configbp -l 100
cleos -u http://127.0.0.1:18881 get table eosio eosio configfee -l 100
cleos -u http://127.0.0.1:18881 get table eosio eosio holdfeebp -l 100
cleos -u http://127.0.0.1:18881 get table eosio eosio approvebp -l 100

# Deposit and activate BP
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["globalforce1"]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["globalforce2"]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["globalforce3"]' -p globalforce3@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["gf.bp1"]' -p gf.bp1@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["gf.bp2"]' -p gf.bp2@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["gf.bp3"]' -p gf.bp3@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["gf.bp4"]' -p gf.bp4@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["krio"]' -p krio@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["world"]' -p world@active
cleos -u http://127.0.0.1:18881 push action eosio bpstake '["stuff"]' -p stuff@active

	##### check bp list who approved with stake
cleos -u http://127.0.0.1:18881 get table eosio eosio approvebp -l 100

### New unregister BP with withdraw. 
	# FIRST - stop nodeos
# Required claim if exist unclaimed tokens
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce1"]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce2"]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce3"]' -p globalforce3@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp1"]' -p gf.bp1@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp2"]' -p gf.bp2@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp3"]' -p gf.bp3@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp4"]' -p gf.bp4@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["krio"]' -p krio@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["world"]' -p world@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["stuff"]' -p stuff@active

# deactivate BP
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["globalforce1"]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["globalforce2"]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["globalforce3"]' -p globalforce3@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["gf.bp1"]' -p gf.bp1@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["gf.bp2"]' -p gf.bp2@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["gf.bp3"]' -p gf.bp3@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["gf.bp4"]' -p gf.bp4@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["krio"]' -p krio@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["world"]' -p world@active
cleos -u http://127.0.0.1:18881 push action eosio unregprod '["stuff"]' -p stuff@active

# Withdraw deposited tokens
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["globalforce1"]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["globalforce2"]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["globalforce3"]' -p globalforce3@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["gf.bp1"]' -p gf.bp1@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["gf.bp2"]' -p gf.bp2@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["gf.bp3"]' -p gf.bp3@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["gf.bp4"]' -p gf.bp4@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["krio"]' -p krio@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["world"]' -p world@active
cleos -u http://127.0.0.1:18881 push action eosio bpleave '["stuff"]' -p stuff@active







rio
EOS8W2Fhip93wPqqDfnTZhAhD8o6DaQ85B6GU91oV6UhgzzQgRjQK
5JSwgBS1oZC72ePfewvMNVsk75vwJ5rijBb3a2VDuPNp9yQwit9

cleos -u http://127.0.0.1:18881 create account gf rio EOS8W2Fhip93wPqqDfnTZhAhD8o6DaQ85B6GU91oV6UhgzzQgRjQK EOS8W2Fhip93wPqqDfnTZhAhD8o6DaQ85B6GU91oV6UhgzzQgRjQK

TEST ACCOUNTS
5KKyhkw81Ydib4ukZBFzEBwoZEUMvDgViUJabgab1jJLbgkRM2W

curl http://127.0.0.1:8900/v1/wallet/import_key -X POST -d '["gf","5KKyhkw81Ydib4ukZBFzEBwoZEUMvDgViUJabgab1jJLbgkRM2W"]'
cleos -u http://127.0.0.1:18881 create account gf.bp3 onenewaccnt1 EOS5JmTWzpbmku8oMJy7vt2ofUKkFxNmiYgquCfeYftxxboVjmPyK EOS5JmTWzpbmku8oMJy7vt2ofUKkFxNmiYgquCfeYftxxboVjmPyK

cleos -u http://127.0.0.1:18881 create account gf.bp3 onenewaccnt2 EOS5JmTWzpbmku8oMJy7vt2ofUKkFxNmiYgquCfeYftxxboVjmPyK EOS5JmTWzpbmku8oMJy7vt2ofUKkFxNmiYgquCfeYftxxboVjmPyK

cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf.bp3", "onenewaccnt1", "5000.0000 GFT", "init" ]' -p gf.bp3@active
cleos -u http://127.0.0.1:18881 push action eosio.token transfer '[ "gf.bp3", "onenewaccnt2", "5000.0000 GFT", "init" ]' -p gf.bp3@active


cleos -u http://127.0.0.1:18881 push action eosio configfee '[15, 24]' -p eosio@active
cleos -u http://127.0.0.1:18881 get table eosio eosio configfee -l 100

cleos -u http://127.0.0.1:18881 push action eosio holdfeebp '[20]' -p eosio@active
cleos -u http://127.0.0.1:18881 get table eosio eosio holdfeebp -l 100

cleos -u http://127.0.0.1:18881 push action eosio configbp '["10000000.0000 GFT", "19.3193 GFT", 11067487, 5184000]' -p eosio@active
cleos -u http://127.0.0.1:18881 get table eosio eosio configbp -l 100

cleos -u http://127.0.0.1:18881 get table eosio eosio approvebp -l 100

###### BP Pay

cleos -u http://127.0.0.1:18881 push action eosio configbp '["10000000.0000 GFT", "19.3193 GFT", 11067487, 5184000]' -p eosio@active
cleos -u http://127.0.0.1:18881 get table eosio eosio approvebp -l 100
cleos -u http://127.0.0.1:18881 get table eosio eosio configbp -l 100

cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp1"]' -p gf.bp1@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp2"]' -p gf.bp2@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp3"]' -p gf.bp3@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["gf.bp4"]' -p gf.bp4@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce1"]' -p globalforce1@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce2"]' -p globalforce2@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["globalforce3"]' -p globalforce3@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["krio"]' -p krio@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["world"]' -p world@active
cleos -u http://127.0.0.1:18881 push action eosio claimrewards '["stuff"]' -p stuff@active