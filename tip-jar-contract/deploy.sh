#!/bin/bash

# If deploying to a remote chain add -u <RPC_URL> to the command below

clio -u https://testnet-api-use1.dev.wire-dev.com set contract tip.jar ./tipjar -p tip.jar@active
