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
- keys : { 0 : {"key" : "..", "status" : "0"}, 1 : {"key" : "..", "status" : "0"}, ... , n : {"key" : "..", "status" : "1"} }

/api/id_to_pub
SEND
- email
RECEIVE
- key : ".."
```


TODO  
  invite
  join
  


## Dependencies
```
sudo apt-get install build-essential tesseract-ocr tesseract-ocr-dev tesseract-ocr-eng


```
