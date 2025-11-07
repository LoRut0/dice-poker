.Phony: test clean

test:
	cmake -S src/userver -B src/userver/build
	cmake --build src/userver/build

clean:
	rm log.log
	rm -rf src/userver/build
