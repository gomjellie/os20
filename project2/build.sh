#!/bin/sh

make
echo "ssu_shell을 빌드 완료했습니다."

cd ./ttop && make && cd ..
echo "ttop를 빌드 완료했습니다. 홈디렉토리에 ttop 실행파일을 생성했습니다."

cd ./pps && make && cd ..
echo "pps를 빌드 완료했습니다. 홈디렉토리에 pps 실행파일을 생성했습니다."

