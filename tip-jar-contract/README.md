### Tip Jar Contract

`tipjar` is a small example contract that shows how to accept and send
`sysio.token` tokens from another contract.

## Deploy

After deploying the contract, add `sysio.code` to the deployed account's active
permission. `claimtips` sends inline `sysio.token::transfer` actions, and this
permission lets the contract authorize those transfers from its own balance.

```sh
clio -u <RPC_URL> set account permission tip.jar active --add-code -p tip.jar@active
```

## Send a tip

Transfer any valid `sysio.token` token to the contract account with a memo in
this format:

```sh
clio transfer alice tip.jar "1.0000 WIRE" "tip: creator"
```

The text after `tip:` must be an existing SYSIO account. Bad memo formats and
nonexistent creator accounts reject the transfer so funds do not become
unclaimable.

Tips are stored as one claimable balance per creator and token symbol. For
example, `daniel` can have separate rows for `1.0000 WIRE` and `2.0000 LIQETH`.
To inspect a creator's balances, query the table with the creator as the scope.

```sh
clio get table tip.jar daniel tips
```

## Claim tips

The creator claims every token balance stored for their account:

```sh
clio push action tip.jar claimtips '["creator"]' -p creator@active
```

`claimtips` sends the funds with inline `sysio.token::transfer` actions signed
by the `tip.jar` contract.



## License

[FSL-1.1-Apache-2.0](./LICENSE.md)
