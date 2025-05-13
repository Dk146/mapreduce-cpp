
mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug ..

make

./worker ../src/input_file/*.txt

# # cd build
cd file
sort mr-out* | grep . > mr-wc-all
# mr-correct-wc.txt is a correct answer
if cmp mr-wc-all ../../mr-correct-wc.txt
then
  echo '---' wc test: PASS
else
  echo '---' wc output is not the same as mr-correct-wc.txt
  echo '---' wc test: FAIL
  failed_any=1
fi
