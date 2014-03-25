
for i in {2..100}
do
    # ./ph data/twitter-RT-er.data twitter-RT-er/twitter-RT-er.n twitter-RT-er/twitter-RT-er.deg $i -d >> twitter-RT-er.txt
    ./ph data/twitter-RT-skel.data twitter-RT-skel/twitter-RT-skel.n twitter-RT-skel/twitter-RT-skel.deg $i -d >> twitter-RT-skel.txt
done
