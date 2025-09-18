.Phony: test

test:
	cmake -S src/userver/final -B src/userver/final/build
	cmake --build src/userver/final/build
