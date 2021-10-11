@echo off 

if not exist "Dependencies\Redis" (
	mkdir Dependencies\Redis
	mkdir Dependencies\_Downloads
	cd Dependencies\_Downloads
	curl --output redis.tar.gz --url https://download.redis.io/releases/redis-6.2.6.tar.gz
	tar xzf redis.tar.gz --directory ..\Redis --strip-components=1
	cd ..\..
) else (
	echo Redis already exists
)
	
if not exist "Dependencies\ServiceStack.Redis" (
	mkdir Dependencies\ServiceStack.Redis
	cd Dependencies
	git clone https://github.com/ServiceStack/ServiceStack.Redis.git
	cd ..
) else (
	echo ServiceStack.Redis already exists
)

if exist Dependencies\_Downloads ( rmdir /S /Q Dependencies\_Downloads )

pause
