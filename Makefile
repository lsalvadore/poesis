all: poesis

poesis:	main.cpp Package.cpp
	${CXX} ${.ALLSRC} -o ${.TARGET}

clean:
	rm -f poesis
