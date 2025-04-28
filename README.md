# IMPORTANT STUFFS

## TESTING SERVER 
- RUN SERVER 
./http_server -p 8080

- Viewing port links
http://localhost:8080/ - For the default page
http://localhost:8080/static/images/logo.png - For the images stored in the static folder 

![alt text](<Screenshot 2025-04-28 at 1.06.26 AM.png>)
http://localhost:8080/static/index.html - For static files
http://localhost:8080/calc/add/5/3 - For the calculator functionality
http://localhost:8080/sleep/2 - For the sleep functionality


### Telenet test example
    telenet localhost 8080 
    in the local host terminal:
    GET /calc/add/5/3 HTTP/1.1
    Host: localhost 

### Postmant test 

![alt text](<Screenshot 2025-04-28 at 1.15.15 AM.png>)