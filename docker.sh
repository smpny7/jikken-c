cd `dirname $0`
docker build -t jikken-c:1 .
docker run -it -d --name jikken-c -p 8081:8080 -v $(pwd):/home/jikken-c jikken-c:1