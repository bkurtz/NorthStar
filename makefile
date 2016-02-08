

.PHONY: all
all: python/directory_pb2.py python/route_pb2.py
	cd src && make

python/%_pb2.py: protobuf/%.proto
	protoc -Iprotobuf/ --python_out=python/ $<

