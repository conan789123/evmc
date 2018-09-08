/* EVMC: Ethereum Client-VM Connector API.
 * Copyright 2018 The EVMC Authors.
 * Licensed under the Apache License, Version 2.0. See the LICENSE file.
 */

#include "_cgo_export.h"

#include <stdlib.h>


void evmc_go_free_result_output(const struct evmc_result* result)
{
    free((void*)result->output_data);
}


/* Go does not support exporting functions with parameters with const modifiers,
 * so we have to cast function pointers to the function types defined in EVMC.
 * This disables any type checking of exported Go functions. To mitigate this
 * problem the go_exported_functions_type_checks() function simulates usage
 * of Go exported functions with expected types to check them during compilation.
 */
const struct evmc_host_interface evmc_go_host = {
    (evmc_account_exists_fn)accountExists,
    (evmc_get_storage_fn)getStorage,
    (evmc_set_storage_fn)setStorage,
    (evmc_get_balance_fn)getBalance,
    (evmc_get_code_size_fn)getCodeSize,
    (evmc_get_code_hash_fn)getCodeHash,
    (evmc_copy_code_fn)copyCode,
    (evmc_selfdestruct_fn)selfdestruct,
    (evmc_call_fn)call,
    (evmc_get_tx_context_fn)getTxContext,
    (evmc_get_block_hash_fn)getBlockHash,
    (evmc_emit_log_fn)emitLog,
};


#pragma GCC diagnostic error "-Wconversion"
static inline void go_exported_functions_type_checks()
{
    struct evmc_context* context = NULL;
    evmc_address* address = NULL;
    evmc_bytes32 bytes32;
    evmc_uint256be* uint256be = NULL;
    uint8_t* data = NULL;
    size_t size = 0;
    int64_t number = 0;
    struct evmc_message* message = NULL;

    struct evmc_tx_context tx_context;
    (void)tx_context;
    struct evmc_result result;
    (void)result;
    enum evmc_storage_status storage_status;
    (void)storage_status;
    bool bool_flag;
    (void)bool_flag;

    evmc_account_exists_fn account_exists_fn = NULL;
    bool_flag = account_exists_fn(context, address);
    bool_flag = accountExists(context, address);

    evmc_get_storage_fn get_storage_fn = NULL;
    bytes32 = get_storage_fn(context, address, &bytes32);
    bytes32 = getStorage(context, address, &bytes32);

    evmc_set_storage_fn set_storage_fn = NULL;
    storage_status = set_storage_fn(context, address, &bytes32, &bytes32);
    storage_status = setStorage(context, address, &bytes32, &bytes32);

    evmc_get_balance_fn get_balance_fn = NULL;
    bool_flag = get_balance_fn(uint256be, context, address);
    bool_flag = getBalance(uint256be, context, address);

    evmc_get_code_size_fn get_code_size_fn = NULL;
    bool_flag = get_code_size_fn(&size, context, address);
    bool_flag = getCodeSize(&size, context, address);

    evmc_get_code_hash_fn get_code_hash_fn = NULL;
    bool_flag = get_code_hash_fn(&bytes32, context, address);
    bool_flag = getCodeHash(&bytes32, context, address);

    evmc_copy_code_fn copy_code_fn = NULL;
    size = copy_code_fn(context, address, size, data, size);
    size = copyCode(context, address, size, data, size);

    evmc_selfdestruct_fn selfdestruct_fn = NULL;
    selfdestruct_fn(context, address, address);
    selfdestruct(context, address, address);

    evmc_call_fn call_fn = NULL;
    result = call_fn(context, message);
    result = call(context, message);

    evmc_get_tx_context_fn get_tx_context_fn = NULL;
    tx_context = get_tx_context_fn(context);
    tx_context = getTxContext(context);

    evmc_get_block_hash_fn get_block_hash_fn = NULL;
    bool_flag = get_block_hash_fn(&bytes32, context, number);
    bool_flag = getBlockHash(&bytes32, context, number);

    evmc_emit_log_fn emit_log_fn = NULL;
    emit_log_fn(context, address, data, size, &bytes32, size);
    emitLog(context, address, data, size, &bytes32, size);
}
