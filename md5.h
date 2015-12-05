#ifndef __MD5_H__

#define __MD5_H__

#include <sys/types.h>
#include <stdint.h>

#define MD5_CTX_BUFFER_LEN	64

typedef struct md5_ctx
{
	uint32_t	state[4];					/*<! A,B,C,D */
	uint32_t	count[2];					/*<! number of bits modulo 2^64 */
	uint8_t		buffer[MD5_CTX_BUFFER_LEN]; /*<! input buffer */
} md5_ctx_t;

#define MD5_DIGEST_LEN		16


/* Initialize the MD5 context
 */
void md5_init(md5_ctx_t * ctx);


/* Update the context with a new message bloc
 */
void md5_update(md5_ctx_t * ctx, const uint8_t * input, const size_t len);


/* Finalize the MD5. Finish the MD5 hash and save the digest
 *
 * digest must be a 16 bytes array
 */
void md5_final(uint8_t * digest, md5_ctx_t * ctx);


#endif /* __MD5_H__ */
