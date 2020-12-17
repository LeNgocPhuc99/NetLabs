output: writer.o reader.o 
	gcc writer.o -o writer.out 
	gcc reader.o -o reader.out

reader.o: reader.c
	gcc -c reader.c 

writer.o: writer.c message.h
	gcc -c writer.c

clean:
	rm *.o 