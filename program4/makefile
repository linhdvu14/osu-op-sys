otp_enc.o: otp_enc.c otp_utils.h
	gcc -c otp_enc.c

otp_enc_d.o: otp_enc_d.c otp_utils.h
	gcc -c otp_enc_d.c

otp_dec.o: otp_dec.c otp_utils.h
	gcc -c otp_dec.c

otp_dec_d.o: otp_dec_d.c otp_utils.h
	gcc -c otp_dec_d.c

otp_utils.o: otp_utils.c otp_utils.h
	gcc -c otp_utils.c

otp_enc: otp_enc.o otp_utils.o
	gcc -g otp_enc.o otp_utils.o -o otp_enc

otp_enc_d: otp_enc_d.o otp_utils.o
	gcc -g otp_enc_d.o otp_utils.o -o otp_enc_d

otp_dec: otp_dec.o otp_utils.o
	gcc -g otp_dec.o otp_utils.o -o otp_dec

otp_dec_d: otp_dec_d.o otp_utils.o
	gcc -g otp_dec_d.o otp_utils.o -o otp_dec_d

keygen: keygen.c
	gcc -o keygen keygen.c

clean:
	rm -f *.o

all: otp_enc otp_enc_d otp_dec otp_dec_d keygen clean