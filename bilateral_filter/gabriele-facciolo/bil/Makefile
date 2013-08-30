bilateral_filter:
	mkdir build; cd build; cmake ..; make
	cp build/bilateral_filter .

test: bilateral_filter
	time ./bilateral_filter samples/lena.png out.png samples/lena.png 9 5 40
