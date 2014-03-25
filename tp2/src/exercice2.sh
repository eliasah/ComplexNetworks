#!/bin/bash

./ph data/twitter-RT-skel.data twitter-RT-skel/twitter-RT-skel.n twitter-RT-skel/twitter-RT-skel.deg >> exercice2.txt
./ph data/twitter-mentions-skel.data twitter-mentions-skel/twitter-mentions-skel.n twitter-mentions-skel/twitter-mentions-skel.deg >> exercice2.txt
./ph data/comments-no-loop.data comments-no-loop/comments-no-loop.n comments-no-loop/comments-no-loop.deg >> exercice2.txt 
./ph data/week.data week/week.n week/week.deg >> exercice2.txt
./ph data/phone.data phone/phone.n phone/phone.deg >> exercice2.txt
./ph data/phone-skel.data phone-skel/phone-skel.n phone-skel/phone-skel.deg >> exercice2.txt

