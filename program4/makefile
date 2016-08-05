keygen: keygen.c
	gcc -o keygen keygen.c

otp_enc.o: otp_enc.c otp_utils.h
	gcc -c otp_enc.c

otp_enc_d.o: otp_enc_d.c otp_utils.h
	gcc -c otp_enc_d.c

otp_utils.o: otp_utils.c otp_utils.h
	gcc -c otp_utils.c

client: otp_enc.o otp_utils.o
	gcc -g otp_enc.o otp_utils.o -o client

server: otp_enc_d.o otp_utils.o
	gcc -g otp_enc_d.o otp_utils.o -o server

clean:
	rm -f *.o

all: keygen client server clean
