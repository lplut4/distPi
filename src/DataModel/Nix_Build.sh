
echo "#################################"
echo "# RUNNING PROTOBUF BUILD SCRIPT #"
echo "#################################"

if [ ! -d "out" ]; then
	mkdir out
fi
cd out
if [ ! -d "c" ]; then
	mkdir c
fi

if [ ! -d "cpp" ]; then
	mkdir cpp
fi
if [ ! -d "csharp" ]; then
	mkdir csharp
fi
if [ ! -d "python" ]; then
	mkdir python
fi
cd ..

cd Protos

protoc="../../Dependencies/protobuf/src/protoc"
protoc_c_plugin="--plugin=protoc-gen-c=../../Dependencies/protobuf-c/protoc-c/protoc-gen-c"
protocOptions="--cpp_out=../out/cpp --csharp_out=../out/csharp --python_out=../out/python --c_out=../out/c"

list="TimeSpec.proto LogMessage.proto"

$protoc $protoc_c_plugin $protocOptions $list