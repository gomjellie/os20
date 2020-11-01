correct=0
total=0

echo "TESTSET: Running Testcases with reader test"
gcc reader_test.c rw_lock-r-test.c rw_lock.c -o rw_lock-r-test -lpthread

echo "CASE1: Reader Test with 5 reader and 1 writer"
total=$((total+1))
out=`./rw_lock-r-test 5 1`
echo $out
if [ "$out" = "PASSED" ]; then
    correct=$((correct+1))
fi

echo "CASE2: Reader Test with 5 reader and 3 writer"
total=$((total+1))
out=`./rw_lock-r-test 5 3`
echo $out
if [ "$out" = "PASSED" ]; then
    correct=$((correct+1))
fi

echo "CASE3: Reader Test with 5 reader and 5 writer"
total=$((total+1))
out=`./rw_lock-r-test 5 5`
echo $out
if [ "$out" = "PASSED" ]; then
    correct=$((correct+1))
fi

echo "TESTSET: Running Testcases with writer test"
gcc writer_test.c rw_lock-w-test.c rw_lock.c -o rw_lock-w-test -lpthread

echo "CASE1: Writer Test with 5 reader and 1 writer"
total=$((total+1))
out=`./rw_lock-w-test 5 1`
echo $out
if [ "$out" = "PASSED" ]; then
    correct=$((correct+1))
fi


echo "CASE2: Writer Test with 5 reader and 3 writer"
total=$((total+1))
out=`./rw_lock-w-test 5 3`
echo $out
if [ "$out" = "PASSED" ]; then
    correct=$((correct+1))
fi

echo "CASE3: Writer Test with 5 reader and 5 writer"
total=$((total+1))
out=`./rw_lock-w-test 5 5`
echo $out
if [ "$out" = "PASSED" ]; then
    correct=$((correct+1))
fi

echo "Test Cases Passed: $correct"
echo "Test Cases Total: $total"

