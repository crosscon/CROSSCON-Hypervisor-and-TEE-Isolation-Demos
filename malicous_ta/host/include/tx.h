#ifndef TX_H
#define TX_H

#define OP_CHECKSIG 0xac
#define OP_DUP 0x76
#define OP_EQUALVERIFY 0x88
#define OP_HASH160 0xa9
#define OP_PUSHDATA(X) 0xX /* 1 to 75 only */
//#define PRINT_OPAQUE_STRUCT(p)  print_mem((p), sizeof(*(p)))
#define TXBUF_INIT_SIZE 64

/* Typedefs */
typedef struct intx_t * INTX;
typedef struct outtx_t * OUTTX;
typedef struct tx_t * TX;
typedef struct serbuf_t * TXBUF;

/* Transaction prototypes */
TX new_tx(INTX intx, OUTTX outtx);
INTX new_intx(const uint8_t *prh, const uint8_t *tmpsrc);
OUTTX new_outtx(const uint8_t *val, const uint8_t *tmpscr);
uint32_t create_unsigned_tx(INTX intx);
void print_intx(INTX intx);
void print_outtx(OUTTX outtx);
void print_tx(TX tx);
void free_intx(INTX itx);
void free_outtx(OUTTX otx);
void free_tx(TX tx);

/* OP-TEE Serialisation prototypes */
TXBUF new_txbuf(void);
void free_txbuf(TXBUF buf);
void reserve_space(TXBUF b, size_t bytes);
void serialize_bytestr(uint8_t *x, size_t len, TXBUF b);
void serialize_intx(INTX intx, TXBUF out);
void serialize_outtx(OUTTX outtx, TXBUF out);
void serialize_tx(TX tx, TXBUF out);
void print_ser_bytes(TXBUF buf);
void *get_txbuf_data(TXBUF buf);
size_t get_txbuf_size(TXBUF buf);
uint32_t get_txbuf_next(TXBUF buf);


//void print_mem(void const *vp, size_t n);



#endif
