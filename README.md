# stasispod

## Design
Masters will accept the following commands:
 - put (data) -> hash
 - get_url (id) -> Authenticated URL to download via http
 - rebalance
 - replicate

And will route those commands to the appropriate slave.
Slaves will accept the following commands from the master:
 - put (data) -> hash
 - get_url (id) -> Authenticated URL to download via http
 - delete (id)
