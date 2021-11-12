
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

list="Utility/TimeSpec.proto Utility/LogMessage.proto Test/Ping.proto Test/Pong.proto"

includePath="--proto_path=Utility --proto_path=Test"
protocOptions="$includePath --cpp_out=../out/cpp --csharp_out=../out/csharp --python_out=../out/python"

protoc="../../Dependencies/protobuf/src/protoc"

$protoc $protocOptions $list