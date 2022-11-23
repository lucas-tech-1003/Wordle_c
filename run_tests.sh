#!/bin/bash
./solver samples/input1 > outputs/output1
./solver samples/input2 > outputs/output2
./solver samples/input3 > outputs/output3
./solver samples/input4 > outputs/output4
./solver samples/input5 > outputs/output5
./solver samples/input6 > outputs/output6
./solver samples/input8 > outputs/output8

diff -y --suppress-common-lines samples/input1.out outputs/output1 | wc -l
diff -y --suppress-common-lines samples/input2.out outputs/output2 | wc -l
diff -y --suppress-common-lines samples/input3.out outputs/output3 | wc -l
diff -y --suppress-common-lines samples/input4.out outputs/output4 | wc -l
diff -y --suppress-common-lines samples/input5.out outputs/output5 | wc -l
diff -y --suppress-common-lines samples/input6.out outputs/output6 | wc -l
diff -y --suppress-common-lines samples/input8.out outputs/output8 | wc -l
