# API Endpoints
## POST

```
/api/register
SEND
- email
- password
- public_key
- gender
RECEIVE
- state : "success"
- state : "fail"

/api/login
SEND
- email
- password
RECEIVE
- state : "success"
- state : "fail"
```
## GET

```
/api/server_sync
SEND
- email
RECEIVE
-
[
  {
    "id": 1,
    "key": "356432465"
  },
  {
    "id": 5,
    "key": "789654123"
  },
  {
    "id": 2,
    "key": "9999999999"
  },
  {
    "id": 2,
    "key": "88888888"
  },
] 

/api/id_to_pub
send
- email
receive
[
  {
    "public_key": "3"
  }
]
```


TODO  
  invite
  join
  
