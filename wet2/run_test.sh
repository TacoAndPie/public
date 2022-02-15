rm a.out;
for i in {0..0};
do rm ../tests_out/my_out$i.txt;
done

g++ -std=c++11 -DNDEBUG -Wall *.cpp
echo compiled

for i in {0..0};
do ./a.out < ../tests/in$i.txt > ../tests_out/my_out$i.txt;
done


for i in {0..0};
do diff -s ../tests/out$i.txt  ../tests_out/my_out$i.txt;
done