
echo "#################################"
echo "# RUNNING PROTOBUF BUILD SCRIPT #"
echo "#################################"

if [ ! -d "out" ]; then
	mkdir out
fi

cd out

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
protocOptions="--cpp_out=../out/cpp --csharp_out=../out/csharp --python_out=../out/python"

list="TimeSpec.proto LogMessage.proto"

$protoc $protocOptions $list