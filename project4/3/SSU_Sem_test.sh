echo "Running SSU_Sem_test.c"
gcc SSU_Sem_test.c SSU_Sem.c -o SSU_Sem_test -lpthread
./SSU_Sem_test
echo
echo "Running SSU_Sem_toggle_test.c"
gcc SSU_Sem_toggle_test.c SSU_Sem.c -o SSU_Sem_toggle_test -lpthread
./SSU_Sem_toggle_test
