#!/bin/bash
# rm twitter-RT-skel.txt twitter-mentions-skel.txt
for i in {2..100}
do
    echo -ne "processing $i%\r"
    #./ph data/twitter-RT-skel.data twitter-RT-skel/twitter-RT-skel.n twitter-RT-skel/twitter-RT-skel.deg $i >> twitter-RT-skel.txt
    #./ph data/twitter-mentions-skel.data twitter-mentions-skel/twitter-mentions-skel.n twitter-mentions-skel/twitter-mentions-skel.deg $i >> twitter-mentions-skel.txt
    #./ph data/comments-no-loop.data comments-no-loop/comments-no-loop.n comments-no-loop/comments-no-loop.deg $i >> comments-no-loop.txt 
    #./ph data/week.data week/week.n week/week.deg $i >> week.txt
    #./ph data/phone.data phone/phone.n phone/phone.deg $i >> phone.txt
    ./ph data/phone-skel.data phone-skel/phone-skel.n phone-skel/phone-skel.deg $i >> phone-skel.txt
done
mv phone-skel.txt ph-results/phone-skel
echo -e "done           \r"
