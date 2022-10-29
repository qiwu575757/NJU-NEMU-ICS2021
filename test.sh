# for pa2 to test instrcuations 

for file in $(find ./tests -name "*.c")
do
    if (test $file != "./tests/hello-str.c" && test $file != "./tests/string.c" )
    then
        len=${#file}
        # echo -e "\033[33m ${file:8:len-10} \033[0m"
        echo -e "\033[43;37m ${file:8:len-10} \033[0m" #黄底白字
        make ARCH=riscv32-nemu ALL=${file:8:len-10} run | grep -e 'HIT'
    fi
    # len=${#file}
    # echo ${file:8:len-10}
    # echo "hello"
done