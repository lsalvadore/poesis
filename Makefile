all: poesis

poesis:	main.cpp Package.cpp
	${CXX} ${.ALLSRC} -o ${.TARGET}

debug:	main.cpp Package.cpp
	${CXX} -g ${.ALLSRC} -o ${.TARGET}

clean:
	rm -f poesis
