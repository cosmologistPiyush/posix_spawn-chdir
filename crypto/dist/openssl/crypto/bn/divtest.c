#include <openssl/bn.h>
#include <openssl/rand.h>

static int myrand(n)
{
    unsigned char x[2];
    RAND_pseudo_bytes(x,2);
    return (x[0] + 2*x[1]);
}

static void bug(char *m, BIGNUM *a, BIGNUM *b)
{
    printf("%s!\na=",m);
    BN_print_fp(stdout, a);
    printf("\nb=");
    BN_print_fp(stdout, b);
    printf("\n");
    fflush(stdout);
    exit(1);
}

main(int argc, char **argv)
{
    BIGNUM *a=BN_new(), *b=BN_new(), *c=BN_new(), *d=BN_new(),
	*C=BN_new(), *D=BN_new();
    BN_RECP_CTX *recp=BN_RECP_CTX_new();
    BN_CTX *ctx=BN_CTX_new();
    int end;
    int i = 0;

    if (argc > 1)
	end = atoi(argv[1]);
    else
	end = 0;

    for (i = 0; end ? i < end : 1; i++) {
	BN_pseudo_rand(a,myrand(),0,0);
	BN_pseudo_rand(b,myrand(),0,0);
	if (BN_is_zero(b)) continue;

	BN_RECP_CTX_set(recp,b,ctx);
	if (BN_div(C,D,a,b,ctx) != 1)
	    bug("BN_div failed",a,b);
	if (BN_div_recp(c,d,a,recp,ctx) != 1)
	    bug("BN_div_recp failed",a,b);
	else if (BN_cmp(c,C) != 0 || BN_cmp(c,C) != 0)
	    bug("mismatch",a,b);
    }
    exit(0);
}
